#include <Server.h>

Server::Server(uint16_t port) {
    start(port);
}

Server::~Server() {
    stop();
}

void Server::start(uint16_t port) {
    struct sockaddr_in address{};
    int opt = 1;

    // Create socket
    m_serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverFD == -1) {
        std::cerr << "Failed to create socket: " << strerror(errno) << endl;
        return;
    }

    // Allow quick restarts while old connections are in TIME_WAIT
    if (setsockopt(m_serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Failed to set socket options: " << strerror(errno) << endl;
        close(m_serverFD);
        m_serverFD = -1;
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(m_serverFD, (struct sockaddr *) &address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind to port " << port << ": " << strerror(errno) << endl;
        close(m_serverFD);
        m_serverFD = -1;
        return;
    }

    // Start listening
    if (listen(m_serverFD, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket: " << strerror(errno) << endl;
        close(m_serverFD);
        m_serverFD = -1;
        return;
    }

    cout << "Server is live on port " << port << "!" << endl;
    m_isRunning = true;

    m_listenerThread = std::jthread(&Server::listenForClients, this);
}

void Server::listenForClients() {
    while (m_isRunning) {
        int newClientSocket = accept(m_serverFD, nullptr, nullptr);
        if (newClientSocket < 0) {
            if (!m_isRunning) {
                break;
            }
            cout << "Client couldn't connect: " << strerror(errno) << endl;
            if (errno == EMFILE || errno == ENFILE || errno == ENOBUFS || errno == ENOMEM) {
                // Out of resources: back off instead of spinning
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            continue;
        }

        // Declared before the lock, so finished threads are joined after it is released.
        std::vector<std::jthread> finishedThreads;
        std::scoped_lock lock(m_mutex);
        finishedThreads = takeFinishedThreads();

        if (m_clients.size() >= MAX_PLAYERS || m_currentGameState == GameState::GAMEPLAY) {
            cout << "Rejecting client " << newClientSocket << ": lobby is full" << endl;
            json full = {{"type", "serverFull"}, {"data", json()}};
            protocol::sendFrame(newClientSocket, full.dump());
            close(newClientSocket);
            continue;
        }

        addClient(newClientSocket);
        m_clientThreads.emplace_back(&Server::handleClient, this, newClientSocket);
    }
}

void Server::handleClient(int clientSocket) {
    while (true) {
        auto message = protocol::recvFrame(clientSocket);
        if (!message.has_value()) {
            break;
        }
        processMessage(clientSocket, *message);
    }

    cout << "Client " << clientSocket << " disconnected" << endl;
    std::scoped_lock lock(m_mutex);
    removeClient(clientSocket);

    if (m_currentGameState == GameState::GAMEPLAY) {
        sendMessage("opponentDisconnected", json());
        returnToLobby();
    }
    sendMessage("updateLobbyState", serializeLobbyState());

    // Last step under the lock: from here on this thread needs nothing and can be joined.
    m_finishedThreads.push_back(std::this_thread::get_id());
}

std::vector<std::jthread> Server::takeFinishedThreads() {
    std::vector<std::jthread> finished;
    for (auto it = m_clientThreads.begin(); it != m_clientThreads.end();) {
        if (std::ranges::find(m_finishedThreads, it->get_id()) != m_finishedThreads.end()) {
            finished.push_back(std::move(*it));
            it = m_clientThreads.erase(it);
        } else {
            ++it;
        }
    }
    m_finishedThreads.clear();
    return finished;
}

void Server::processMessage(int clientSocket, const string &message) {
    try {
        json jsonMessage = json::parse(message);

        string type = jsonMessage["type"];
        json parsedData = jsonMessage["data"];

        cout << "Incoming message from client " << clientSocket << ": " << jsonMessage.dump() << endl;

        std::scoped_lock lock(m_mutex);

        if (type == "ready" && m_currentGameState == GameState::LOBBY) {
            for (auto &player: m_lobbyState.players) {
                if (player.name == std::to_string(clientSocket)) {
                    player.isReady = !player.isReady;
                    cout << "Client " << clientSocket << " toggled ready state to "
                         << (player.isReady ? "ready" : "not ready") << endl;
                    break;
                }
            }
            sendMessage("updateLobbyState", serializeLobbyState());
            return;
        }

        if (type == "startGame") {
            if (m_currentGameState == GameState::LOBBY && m_lobbyState.canStart() && m_clients.size() == MAX_PLAYERS) {
                m_currentGameState = GameState::GAMEPLAY;
                m_game = Game(Player(m_clients[0]->getSocket(), "mage"), Player(m_clients[1]->getSocket(), "warrior"));
                m_game.startGame();
                sendGameplayState("startGame");
            }
            return;
        }

        // Game actions are only valid while a game is running; m_game has no players before that.
        if (m_currentGameState != GameState::GAMEPLAY) {
            return;
        }

        if (type == "updateGameState") {
            sendGameplayState("updateGameState");
            return;
        }

        // Every remaining message changes the game, so only the player on turn may send it.
        if (m_game.getOnTurnPlayer().getId() != clientSocket) {
            cout << "Ignoring " << type << " from client " << clientSocket << ": not their turn" << endl;
            return;
        }

        if (type == "attackFace") {
            m_game.attackFace();
        } else if (type == "attack") {
            m_game.attack(parsedData["index"]);
        } else if (type == "playCard") {
            m_game.playACard(parsedData["index"]);
        } else if (type == "selectCardBoard") {
            m_game.selectCardBoard(parsedData["index"]);
        } else if (type == "endTurn") {
            m_game.endTurn();
        } else {
            cout << "Unknown message type: " << type << endl;
            return;
        }

        sendGameplayState("updateGameState");

        if (m_game.checkGameOver() || m_game.isDraw()) {
            // winner is -1 for a draw
            sendMessage("endGame", {{"winner", m_game.getWinnerId()}});
            returnToLobby();
            sendMessage("updateLobbyState", serializeLobbyState());
        }

    } catch (json::parse_error &e) {
        std::cerr << "Received an invalid JSON message: " << message << " error:" << e.what() << endl;
    } catch (std::exception &e) {
        // A malformed or out-of-order message must not take down the whole server.
        std::cerr << "Failed to process message: " << message << " error:" << e.what() << endl;
    }
}

void Server::addClient(int clientSocket) {
    // A client that stops reading must not block the server forever while it holds m_mutex.
    timeval timeout{.tv_sec = 2, .tv_usec = 0};
    setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    m_clients.push_back(std::make_unique<Client>(clientSocket));
    cout << "Client " << clientSocket << " has connected" << endl;

    LobbyState::PlayerInfo newPlayer;
    newPlayer.name = std::to_string(clientSocket);
    newPlayer.isReady = false;
    m_lobbyState.players.push_back(newPlayer);

    sendMessage("updateLobbyState", serializeLobbyState());
    sendMessage("yourID", clientSocket, clientSocket);
}

void Server::removeClient(int clientSocket) {
    // Destroying the Client closes its socket, so it must not be closed again here.
    std::erase_if(m_clients, [clientSocket](const auto &client) {
        return client->getSocket() == clientSocket;
    });
    std::erase_if(m_lobbyState.players, [clientSocket](const auto &player) {
        return player.name == std::to_string(clientSocket);
    });
}

void Server::returnToLobby() {
    m_currentGameState = GameState::LOBBY;
    for (auto &player: m_lobbyState.players) {
        player.isReady = false;
    }
}

void Server::sendMessage(const string &type, const json &data, int clientSocket) {
    json message;
    message["type"] = type;
    message["data"] = data;
    string serializedMessage = message.dump();

    for (const auto &client: m_clients) {
        if (clientSocket != -1 && clientSocket != client->getSocket()) {
            continue;
        }
        if (!protocol::sendFrame(client->getSocket(), serializedMessage)) {
            std::cerr << "Failed to send message to client " << client->getSocket() << " ("
                      << strerror(errno) << "), disconnecting it" << std::endl;
            // Wakes up the client's handler thread, which then removes it.
            net::shutdownSocket(client->getSocket());
        }
    }
}

void Server::sendGameplayState(const string &type) {
    for (const auto &client: m_clients) {
        sendMessage(type, serializeGameplayState(client->getSocket()), client->getSocket());
    }
}

json Server::serializeLobbyState() {
    json j;
    j["players"] = json::array();
    for (const auto &player: m_lobbyState.players) {
        json playerJson = {
                {"name",    player.name},
                {"isReady", player.isReady}
        };
        j["players"].push_back(playerJson);
    }
    return j;
}

json Server::serializeGameplayState(int viewerId) {
    json j;
    for (const auto &player: m_game.getPlayers()) {
        // Only the viewer's own hand and deck are sent; the opponent's stay hidden.
        bool isViewer = player->getId() == viewerId;
        json playerStatsJson = {
                {"archetype", player->getArchetype()},
                {"onTurn",    player->isTurn()},
                {"id",        player->getId()},
                {"hp",        player->getHp()},
                {"mana",      player->getMana()},
                {"deck",      isViewer ? player->getDeck().serialize() : json::array()},
                {"hand",      isViewer ? player->getHand().serialize() : json::array()},
                {"board",     player->getBoard().serialize()},
        };
        j["players"].push_back(playerStatsJson);
    }
    if (auto card = m_game.getSelectedCard(); card.has_value()) {
        j["selectedCard"] = card->serialize();
    }
    return j;
}

void Server::stop() {
    if (!m_isRunning.exchange(false)) {
        return;
    }

    // shutdown() (unlike close()) reliably wakes threads blocked in accept()/recv() on these sockets.
    ::shutdown(m_serverFD, SHUT_RDWR);
    if (m_listenerThread.joinable()) {
        m_listenerThread.join();
    }
    close(m_serverFD);
    m_serverFD = -1;

    {
        std::scoped_lock lock(m_mutex);
        for (const auto &client: m_clients) {
            net::shutdownSocket(client->getSocket());
        }
    }

    // Handler threads remove their own clients, so join without holding the lock.
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
