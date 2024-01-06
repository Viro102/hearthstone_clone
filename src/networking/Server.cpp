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
        char buffer[1024] = {0};
        auto valread = recv(clientSocket, buffer, 1024, 0);
        if (valread > 0) {
            string message(buffer, valread);
            cout << "Incoming message from client " << clientSocket << ": " << message << endl;
            processMessage(clientSocket, message);
        } else if (valread == -1) {
            cout << "Error when receiving message" << strerror(errno) << endl;
        } else {
            cout << "Client " << clientSocket << " disconnected" << endl;
            removeClient(clientSocket);
            break;
        }
    }
}

void Server::processMessage(int clientSocket, const string &message) {
    try {
        json jsonMessage = json::parse(message);

        string type = jsonMessage["type"];
        json data = jsonMessage["data"];

        if (type == "playCard") {
            // TODO
        } else if (type == "ready") {
            std::unique_lock lock(m_lobbyStateMutex);
            for (auto &player: m_lobbyState.players) {
                if (std::stoi(player.name) == clientSocket) {
                    player.isReady = true;
                    cout << "Client " << clientSocket << " marked as ready." << endl;
                    break;
                }
            }
            lock.unlock();
            broadcastMessage("updateLobbyState", serializeLobbyState());
        } else if (type == "startGame") {
            broadcastMessage("startGame", json());
        }


    } catch (json::parse_error &e) {
        std::cerr << "Received an invalid JSON message: " << message << " error:" << e.what() << endl;
    }
    checkAllClientsReady();
}

void Server::broadcastMessage(const string &type, const json &data) {
    json message;
    message["type"] = type;
    message["data"] = data;

    string serializedMessage = message.dump();
    std::scoped_lock lock(m_clientsMutex);
    for (const auto &client: m_clients) {
        send(client->getSocket(), serializedMessage.c_str(), serializedMessage.length(), 0);
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

void Server::updateLobbyStateWithNewClient(int clientSocket) {
    LobbyState::PlayerInfo newPlayer;
    newPlayer.name = std::to_string(clientSocket);
    newPlayer.isReady = false;

    m_lobbyState.players.push_back(newPlayer);

    json serializedMessage = serializeLobbyState();
    broadcastMessage("updateLobbyState", serializedMessage);
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

    broadcastMessage("allReady", json());
    cout << "Clients are ready!" << endl;
}


string Server::serializeGameState() {
    // TODO
    return "game state";
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
