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

using nlohmann::json;

class Client {
public:
    explicit Client(int socketFD);

    Client() = default;

    ~Client();

    int start(short port, const string &ipAddr);

    void shutdown();

    void listenToServer();

    void sendMessage(const string &message) const;

    [[nodiscard]] int getSocket() const;

    [[nodiscard]] LobbyState getLobbyState() const;

    using StateChangeCallback = std::function<void(GameState)>;

    void setStateChangeCallback(const StateChangeCallback &callback);

private:

    StateChangeCallback stateChangeCallback;

    void updateLocalLobbyState(const string &message);

    void processMessage(const string &message);


    int m_socket{-1};
    LobbyState m_lobbyState{};
    std::jthread m_serverListener;
};
