#pragma once

#include <Client.h>
#include <Common.h>
#include <GameState.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <LobbyState.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <Game.h>
#include <Protocol.h>
#include <atomic>
#include <chrono>

class Server {
public:
    explicit Server(uint16_t port);

    ~Server();

    [[nodiscard]] bool isRunning() const;

    void stop();

private:
    static constexpr size_t MAX_PLAYERS = 2;

    void start(uint16_t port);

    void listenForClients();

    void handleClient(int clientSocket);

    void processMessage(int clientSocket, const string &message);

    // Every method below expects m_mutex to be held by the caller.

    void addClient(int clientSocket);

    void removeClient(int clientSocket);

    void sendMessage(const string &type, const nlohmann::json &data, int clientSocket = -1);

    void sendGameplayState(const string &type);

    void returnToLobby();

    json serializeGameplayState(int viewerId);

    json serializeLobbyState();


    int m_serverFD{-1};
    std::atomic<bool> m_isRunning{false};
    std::jthread m_listenerThread;
    std::vector<std::jthread> m_clientThreads{};

    // Guards everything below: client list, lobby and game state.
    std::mutex m_mutex;
    Game m_game;
    LobbyState m_lobbyState{};
    GameState m_currentGameState{GameState::LOBBY};
    vector<std::unique_ptr<Client>> m_clients{};
};
