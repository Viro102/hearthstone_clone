#pragma once

#include <Client.h>
#include <Common.h>
#include <GameState.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <LobbyState.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <Game.h>

class Server {
public:
    explicit Server(short port);

    ~Server();

    // Main loop for listening to clients
    void listenForClients();

    [[nodiscard]] bool isRunning() const;

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

    json serializeGameplayState();
    
    bool sendAll(int socket, const char *buffer, size_t length);

    void sendMessage(const string &type, const nlohmann::json &data, int clientSocket = -1);

    GameState currentGameState = GameState::LOBBY;

    json serializeLobbyState();

    int m_serverFD{-1};
    Game m_game{};
    LobbyState m_lobbyState{};
    vector<std::unique_ptr<Client>> m_clients{};
    std::vector<std::jthread> m_clientThreads{};
    std::atomic<bool> m_isRunning{false};
    std::mutex m_clientsMutex;
    std::mutex m_lobbyStateMutex;
};
