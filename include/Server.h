#pragma once

#include <Client.h>
#include <Common.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <LobbyState.h>
#include <nlohmann/json.hpp>
#include <mutex>

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

    void updateLobbyStateWithNewClient(int clientSocket);

    void broadcastLobbyState();

    string serializeLobbyState(const LobbyState &state);


    int m_serverFD{-1};
    vector<std::unique_ptr<Client>> m_clients{};
    std::atomic<bool> m_isRunning{false};
    std::mutex m_clientMutex;
    LobbyState m_lobbyState{};
};
