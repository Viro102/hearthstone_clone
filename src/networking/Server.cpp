#include <Server.h>

void Server::startServer(short port) {
    struct sockaddr_in address{};
    int opt = 1;

    // Create socket
    m_serverFD = socket(AF_INET, SOCK_STREAM, 0);

    // Attach socket to the port 8080
    setsockopt(m_serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind socket to the port 8080
    bind(m_serverFD, (struct sockaddr *) &address, sizeof(address));

    if (listen(m_serverFD, 3) == 0) {
        cout << "Server is live!" << endl;
        m_isRunning = true;
    }


    std::jthread(&Server::listenForClients, this).detach();
}

void Server::listenForClients() {
    struct sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);

    while (m_isRunning) {
        int newClientSocket = accept(m_serverFD, (struct sockaddr *) &clientAddress, &clientAddressLength);
        if (newClientSocket >= 0) {
            std::scoped_lock lock(m_clientMutex);
            m_clients.push_back(std::make_unique<Client>(newClientSocket));
            cout << "Client " << newClientSocket << " has connected" << endl;
            updateLobbyStateWithNewClient(newClientSocket);
            std::jthread(&Server::handleClient, this, newClientSocket).detach();
        } else {
            cout << "Client couldn't connect" << endl;
        }
    }
}

void Server::handleClient(int clientSocket) {
    while (m_isRunning) {
        broadcastLobbyState();
        checkAllClientsReady();
        char buffer[1024] = {0};
        auto valread = recv(clientSocket, buffer, 1024, 0);
        if (valread > 0) {
            std::string message(buffer, valread);
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
    if (message == "playCard") {
        // TODO
    }
    if (message == "ready") {
        for (auto &player: m_lobbyState.players) {
            cout << player.name << " " << clientSocket << endl;
            if (std::stoi(player.name) == clientSocket) {
                player.isReady = true;
                cout << "Client " << clientSocket << " marked as ready." << endl;
                break;
            }
        }
    }
}

void Server::broadcastLobbyState() {
    string state = serializeLobbyState(m_lobbyState);
    std::scoped_lock lock(m_clientMutex);
    for (const auto &client: m_clients) {
        send(client->getSocket(), state.c_str(), state.length(), 0);
    }
}

string Server::serializeLobbyState(const LobbyState &state) {
    nlohmann::json j;
    for (const auto &player: state.players) {
        nlohmann::json playerJson = {
                {"name",    player.name},
                {"isReady", player.isReady}
        };
        j["players"].push_back(playerJson);
    }
    return j.dump();
}

void Server::broadcastGameState() {
    string state = serializeGameState();
    std::scoped_lock lock(m_clientMutex);
    for (const auto &client: m_clients) {
        int sock = client->getSocket();
        send(sock, state.c_str(), state.length(), 0);
    }
}

void Server::updateLobbyStateWithNewClient(int clientSocket) {
    LobbyState::PlayerInfo newPlayer;
    newPlayer.name = std::to_string(clientSocket);
    newPlayer.isReady = false;

    m_lobbyState.players.push_back(newPlayer);
}


void Server::removeClient(int clientSocket) {
    std::scoped_lock lock(m_clientMutex);
    const auto [first, last] = std::ranges::remove_if(m_clients,
                                                      [clientSocket](const auto &client) {
                                                          return client->getSocket() == clientSocket;
                                                      });
    m_clients.erase(first, last);
    close(clientSocket);
}

void Server::checkAllClientsReady() {
    std::scoped_lock lock(m_clientMutex);
    for (const auto &client: m_clients) {
        if (!client->isReady()) {
            return;
        }
    }
    cout << "Clients are ready!" << endl;
}


std::string Server::serializeGameState() {
    // TODO
    return "game state";
}

void Server::stopServer() {
    m_isRunning = false;
    for (const auto &client: m_clients) {
        close(client->getSocket());
    }
    close(m_serverFD);
    cout << "Server is shutting down..." << endl;
}

Server::Server(short port) {
    startServer(port);
}

Server::~Server() {
    stopServer();
}
