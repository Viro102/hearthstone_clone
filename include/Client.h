#pragma once

#include <Common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <LobbyState.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <functional>
#include <utility>
#include <GameState.h>
#include <Game.h>

using StateChangeCallback = std::function<void(GameState)>;

class Client {
public:
    explicit Client(int socketFD);

    Client() = default;

    ~Client();

    int start(short port, const string &ipAddr);

    void shutdown();

    void listenToServer();

    void sendMessage(const string &message, const json &data = "") const;

    [[nodiscard]] int getSocket() const;

    [[nodiscard]] LobbyState getLobbyState() const;

    [[nodiscard]] Game &getGameplayState();

    [[nodiscard]] int getID() const;

    [[nodiscard]] bool isGameStateInitialized() const;

    void setStateChangeCallback(const StateChangeCallback &callback);

private:
    StateChangeCallback stateChangeCallback;

    void updateLocalLobbyState(const string &message);

    void updateLocalGameplayState(const string &message);

    void processMessage(const string &message);

    bool recvAll(int socket, char *buffer, size_t length);


    int m_socket{-1};
    int m_ID{-1};
    LobbyState m_lobbyState{};
    Game m_gameplayState{};
    std::jthread m_serverListener;
    bool m_isGameStateInitialized{false};
};
