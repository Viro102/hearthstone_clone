#pragma once

#include <Client.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <mutex>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

using std::string;
using std::cout;
using std::vector;
using std::endl;

class Server {
public:
    explicit Server(short port);

    ~Server();

    // Main loop for listening to clients
    void listenForClients();

private:
    void startServer(short port);

    void stopServer();

    void removeClient(int clientSocket);

    void checkAllClientsReady();

    // Handle individual client
    void handleClient(int clientSocket);

    // Process incoming messages from clients
    void processMessage(int clientSocket, const string &message);

    // Send the game state to all connected clients
    void broadcastGameState();

    // Convert the game state to a string for sending
    string serializeGameState();


    int m_serverFD{-1};
    vector<std::unique_ptr<Client>> m_clients{};
    std::atomic<bool> m_isRunning{false};     // For safely stopping the server
    std::mutex m_clientMutex;
};
