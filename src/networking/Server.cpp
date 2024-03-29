#include <Server.h>

Server::Server(short port) {
    start(port);
}

Server::~Server() {
    stop();
}

void Server::start(short port) {
    struct sockaddr_in address{};
    int opt = 1;

    // Create socket
    m_serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverFD == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << endl;
        return;
    }

    // Attach socket to the port
    if (setsockopt(m_serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << endl;
        close(m_serverFD);
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind socket to the port 8080
    if (bind(m_serverFD, (struct sockaddr *) &address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind to port " << port << ": " << strerror(errno) << endl;
        close(m_serverFD);
        return;
    }

    // Start listening
    if (listen(m_serverFD, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << endl;
        close(m_serverFD);
        return;
    }

    cout << "Server is live on port " << port << "!" << endl;
    m_isRunning = true;

    m_clientThreads.emplace_back(&Server::listenForClients, this);
}

void Server::listenForClients() {
    struct sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);

    while (m_isRunning) {
        int newClientSocket = accept(m_serverFD, (struct sockaddr *) &clientAddress, &clientAddressLength);
        if (newClientSocket >= 0) {
            std::unique_lock lock(m_clientsMutex);
            m_clients.push_back(std::make_unique<Client>(newClientSocket));
            cout << "Client " << newClientSocket << " has connected" << endl;
            lock.unlock();
            updateLobbyStateWithNewClient(newClientSocket);
            m_clientThreads.emplace_back(&Server::handleClient, this, newClientSocket);
        } else {
            cout << "Client couldn't connect" << endl;
        }
    }
}

void Server::handleClient(int clientSocket) {
    while (m_isRunning) {
        array<char, 10240> buffer{};
        auto valread = recv(clientSocket, buffer.data(), 10240, 0);
        if (valread > 0) {
            string message(buffer.data(), valread);
            processMessage(clientSocket, message);
        } else if (valread == -1) {
            cout << "Error when receiving message: " << strerror(errno) << endl;
        } else {
            cout << "Client " << clientSocket << " disconnected" << endl;
            removeClient(clientSocket);

            if (m_currentGameState == GameState::GAMEPLAY) {
                sendMessage("opponentDisconnected", json());
                m_currentGameState = GameState::LOBBY;
            }
            break;
        }
    }
}

void Server::processMessage(int clientSocket, const string &message) {
    try {
        json jsonMessage = json::parse(message);

        string type = jsonMessage["type"];
        string data = jsonMessage["data"].dump();

        json parsedData = json::parse(data);

        cout << "Incoming message from client " << clientSocket << ": " << jsonMessage.dump(4) << endl;

        if (type == "ready") {
            std::unique_lock lock(m_lobbyStateMutex);
            for (auto &player: m_lobbyState.players) {
                if (std::stoi(player.name) == clientSocket) {
                    player.isReady = !player.isReady;
                    cout << "Client " << clientSocket << " toggled ready state to "
                         << (player.isReady ? "ready" : "not ready") << endl;
                    break;
                }
            }
            lock.unlock();
            sendMessage("updateLobbyState", serializeLobbyState());
        }

        if (type == "startGame" && m_lobbyState.canStart()) {
            m_currentGameState = GameState::GAMEPLAY;
            m_game = Game(Player(m_clients[0]->getSocket(), "mage"), Player(m_clients[1]->getSocket(), "warrior"));
            m_game.startGame();
            sendMessage("startGame", serializeGameplayState());
        }

        if (type == "updateGameState") {
            sendMessage("updateGameState", serializeGameplayState());
        }

        if (type == "attackFace") {
            m_game.attackFace();
            if (m_game.checkGameOver()) {
                sendMessage("endGame", json());
            }
        }

        if (type == "attack") {
            m_game.attack(parsedData["index"]);
        }

        if (type == "playCard") {
            m_game.playACard(parsedData["index"]);
        }

        if (type == "selectCardBoard") {
            m_game.selectCardBoard(parsedData["index"]);
        }

        if (type == "endTurn") {
            m_game.endTurn();
        }

        if (type == "attack") {
            m_game.attack(parsedData["index"]);
        }

        if (m_currentGameState == GameState::GAMEPLAY) {
            sendMessage("updateGameState", serializeGameplayState());
        }

    } catch (json::parse_error &e) {
        std::cerr << "Received an invalid JSON message: " << message << " error:" << e.what() << endl;
    }
    checkAllClientsReady();
}

bool Server::sendAll(int socket, const char *buffer, size_t length) {
    size_t totalSent = 0;
    ssize_t lastSent;
    while (totalSent < length) {
        lastSent = send(socket, buffer + totalSent, length - totalSent, 0);
        if (lastSent == -1) {
            std::cerr << "Failed to send data." << std::endl;
            return false;
        }
        totalSent += lastSent;
    }
    return true;
}

void Server::sendMessage(const string &type, const json &data, int clientSocket) {
    json message;
    message["type"] = type;
    message["data"] = data;
    string serializedMessage = message.dump();
    uint32_t messageLength = htonl(serializedMessage.size());

    std::scoped_lock lock(m_clientsMutex);
    if (clientSocket == -1) {
        // Broadcast
        for (const auto &client: m_clients) {
            // Prefix message with its length
            if (!sendAll(client->getSocket(), reinterpret_cast<const char *>(&messageLength), sizeof(messageLength))) {
                std::cerr << "Failed to send message length." << std::endl;
                return;
            }
            // Send the actual message
            if (!sendAll(client->getSocket(), serializedMessage.c_str(), serializedMessage.size())) {
                std::cerr << "Failed to send message." << std::endl;
            } else {
//                cout << "Server sent message: " << message.dump(4) << endl;
            }
        }
    } else {
        // Directed
        for (const auto &client: m_clients) {
            if (clientSocket == client->getSocket()) {
                // Prefix message with its length
                if (!sendAll(client->getSocket(), reinterpret_cast<const char *>(&messageLength),
                             sizeof(messageLength))) {
                    std::cerr << "Failed to send message length." << std::endl;
                    return;
                }
                // Send the actual message
                if (!sendAll(client->getSocket(), serializedMessage.c_str(), serializedMessage.size())) {
                    std::cerr << "Failed to send message." << std::endl;
                } else {
//                    cout << "Server sent message: " << message.dump(4) << endl;
                }
            }
        }
    }
}

json Server::serializeLobbyState() {
    json j;
    std::scoped_lock lock(m_lobbyStateMutex);
    for (const auto &player: m_lobbyState.players) {
        json playerJson = {
                {"name",    player.name},
                {"isReady", player.isReady}
        };
        j["players"].push_back(playerJson);
    }
    return j;
}

json Server::serializeGameplayState() {
    json j;
    for (int i = 0; i < m_clients.size(); i++) {
        const auto &player = m_game.getPlayers()[i];
        json playerStatsJson = {
                {"archetype", player->getArchetype()},
                {"onTurn",    player->isTurn()},
                {"id",        m_clients[i]->getSocket()},
                {"hp",        player->getHp()},
                {"mana",      player->getMana()},
                {"deck",      player->getDeck().serialize()},
                {"hand",      player->getHand().serialize()},
                {"board",     player->getBoard().serialize()},
        };
        j["players"].push_back(playerStatsJson);

    }
    if (m_game.getSelectedCard().has_value()) {
        const auto card = m_game.getSelectedCard().value();
        j += {"selectedCard", card.serialize()};
    }
    cout << "Serializing game state" << j.dump(4) << endl;
    return j;
}

void Server::updateLobbyStateWithNewClient(int clientSocket) {
    LobbyState::PlayerInfo newPlayer;
    newPlayer.name = std::to_string(clientSocket);
    newPlayer.isReady = false;
    m_lobbyState.players.push_back(newPlayer);

    json serializedMessage = serializeLobbyState();
    sendMessage("updateLobbyState", serializedMessage);
    sendMessage("yourID", clientSocket, clientSocket);
}


void Server::removeClient(int clientSocket) {
    std::scoped_lock lock(m_clientsMutex);
    const auto [first, last] = std::ranges::remove_if(m_clients,
                                                      [clientSocket](const auto &client) {
                                                          return client->getSocket() == clientSocket;
                                                      });
    const auto [b, e] = std::ranges::remove_if(m_lobbyState.players,
                                               [clientSocket](const auto &player) {
                                                   return std::stoi(player.name) == clientSocket;
                                               });
    m_clients.erase(first, last);
    m_lobbyState.players.erase(b, e);
    close(clientSocket);
}

void Server::checkAllClientsReady() {
    std::scoped_lock lock(m_lobbyStateMutex);
    for (const auto &player: m_lobbyState.players) {
        if (!player.isReady) {
            return;
        }
    }
    cout << "Clients are ready!" << endl;
}

void Server::stop() {
    m_isRunning = false;

    if (m_serverFD >= 0) {
        close(m_serverFD);
        m_serverFD = -1;
    }

    std::scoped_lock lock(m_clientsMutex);
    for (const auto &client: m_clients) {
        if (client->getSocket() >= 0) {
            removeClient(client->getSocket());
        }
    }

    for (auto &thread: m_clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    cout << "Server is shutting down..." << endl;
}

bool Server::isRunning() const {
    return m_isRunning;
}
