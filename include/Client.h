#pragma once

#include <Common.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Client {
public:
    explicit Client(int socketFD);

    Client() = default;

    ~Client();

    int getSocket() const;

    bool isReady() const;

    void setReady(bool ready);

    int startClient(short port);

private:

    void shutdownClient() const;

    int m_socket{-1};
    bool m_isReady{false};
};
