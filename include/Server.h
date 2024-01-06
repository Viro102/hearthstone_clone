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

using nlohmann::json;

class Server {
public:
    explicit Server(short port);

    ~Server();

    // Main loop for listening to clients
    void listenForClients();

private:
    void start(short port);

    void stop();

    void removeClient(int clientSocket);

    void checkAllClientsReady();

    // Handle individual client
    void handleClient(int clientSocket);

    // Process incoming messages from clients
    void processMessage(int clientSocket, const string &message);

    void updateLobbyStateWithNewClient(int clientSocket);

    // Send the game state to all connected clients
    void broadcastGameState();

    // Convert the game state to a string for sending
    string serializeGameState();

    void broadcastMessage(const string &type, const nlohmann::json &data);

    json serializeLobbyState();

    int m_serverFD{-1};
    LobbyState m_lobbyState{};
    vector<std::unique_ptr<Client>> m_clients{};
    std::vector<std::jthread> m_clientThreads{};
    std::atomic<bool> m_isRunning{false};
    std::mutex m_clientsMutex;
    std::mutex m_lobbyStateMutex;
};
