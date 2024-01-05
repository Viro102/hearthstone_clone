#include <Server.h>

// Initialize and start the server
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

// Main loop for listening to clients
void Server::listenForClients() {
    struct sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);

    while (m_isRunning) {
        int newClientSocket = accept(m_serverFD, (struct sockaddr *) &clientAddress, &clientAddressLength);
        // Check if accept was successful
        if (newClientSocket >= 0) {
            std::scoped_lock lock(m_clientMutex);
            m_clients.push_back(std::make_unique<Client>(newClientSocket));
            cout << "Client " << newClientSocket << " has connected" << endl;
            std::jthread(&Server::handleClient, this, newClientSocket).detach();
        } else {
            cout << "Client couldn't connect" << endl;
        }
    }
}

// Handle individual client
void Server::handleClient(int clientSocket) {
    while (m_isRunning) {
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

// Process incoming messages from clients
void Server::processMessage(int clientSocket, const string &message) {
    if (message == "playCard") {
//        playACard(*m_players[clientSocket], cardIndex);
//        broadcastGameState();
    }
    if (message == "ready") {
        std::scoped_lock lock(m_clientMutex);
        for (const auto &client: m_clients) {
            if (client->getSocket() == clientSocket) {
                client->setReady(true);
                cout << "Client " << clientSocket << " marked as ready." << endl;
                break;
            }
        }
    }
    checkAllClientsReady();
}

// Send the game state to all connected clients
void Server::broadcastGameState() {
    std::string state = serializeGameState();
    std::scoped_lock lock(m_clientMutex);
    for (const auto &client: m_clients) {
        auto sock = client->getSocket();
        send(sock, state.c_str(), state.length(), 0);
    }
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


// Convert the game state to a string for sending
std::string Server::serializeGameState() {
    return "game state";
}

// Safely shutdown the server
void Server::stopServer() {
    m_isRunning = false;
    for (const auto &client: m_clients) {
        close(client->getSocket());
    }
    close(m_serverFD);
    cout << "Server is down" << endl;
}

Server::Server(short port) {
    startServer(port);
}

Server::~Server() {
    stopServer();
}
