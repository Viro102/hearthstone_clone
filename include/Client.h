#pragma once

#include <Common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <LobbyState.h>
#include <nlohmann/json.hpp>
#include <thread>

using nlohmann::json;

class Client {
public:
    explicit Client(int socketFD);

    Client() = default;

    ~Client();

    int start(short port);

    void shutdown();

    void listenToServer();

    void sendMessage(const string &message) const;

    [[nodiscard]] int getSocket() const;

    [[nodiscard]] LobbyState getLobbyState() const;

    bool canStart() const;

private:
    void updateLocalLobbyState(const string &message);

    void processMessage(const string &message);


    int m_socket{-1};
    bool m_canStart{false};
    LobbyState m_lobbyState{};
    std::jthread m_serverListener;
};
