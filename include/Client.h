#pragma once

#include <Common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <LobbyState.h>
#include <nlohmann/json.hpp>
#include <thread>


class Client {
public:
    explicit Client(int socketFD);

    Client() = default;

    ~Client();

    int getSocket() const;

    bool isReady() const;

    void setReady(bool ready);

    int startClient(short port);

    void listenToServer();

    LobbyState getLobbyState();

private:
    void shutdownClient() const;

    void updateLocalLobbyState(const string &message);

    int m_socket{-1};
    bool m_isReady{false};
    LobbyState m_lobbyState{};
};
