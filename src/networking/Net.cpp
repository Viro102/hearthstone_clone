#include <Net.h>

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

#include <algorithm>
#include <climits>
#include <iostream>
#include <string>

namespace net {
#ifdef _WIN32
    namespace {
        // Winsock must be initialized once per process before any socket call.
        struct WinsockSession {
            bool ok;

            WinsockSession() {
                WSADATA data;
                ok = WSAStartup(MAKEWORD(2, 2), &data) == 0;
            }

            ~WinsockSession() {
                if (ok) {
                    WSACleanup();
                }
            }
        };

        bool ensureInitialized() {
            static WinsockSession session;
            return session.ok;
        }

        SOCKET native(Socket socket) {
            return static_cast<SOCKET>(socket);
        }

        bool interrupted() {
            return WSAGetLastError() == WSAEINTR;
        }

        constexpr int SEND_FLAGS = 0;
    }

    std::string lastError() {
        return "Winsock error " + std::to_string(WSAGetLastError());
    }

    void shutdownSocket(Socket socket) {
        ::shutdown(native(socket), SD_BOTH);
    }

    void closeSocket(Socket socket) {
        ::closesocket(native(socket));
    }
#else
    namespace {
        bool ensureInitialized() {
            return true;
        }

        int native(Socket socket) {
            return static_cast<int>(socket);
        }

        bool interrupted() {
            return errno == EINTR;
        }

        // Without MSG_NOSIGNAL, writing to a closed connection raises SIGPIPE and kills the process.
#ifdef MSG_NOSIGNAL
        constexpr int SEND_FLAGS = MSG_NOSIGNAL;
#else
        constexpr int SEND_FLAGS = 0;
#endif
    }

    std::string lastError() {
        return std::strerror(errno);
    }

    void shutdownSocket(Socket socket) {
        ::shutdown(native(socket), SHUT_RDWR);
    }

    void closeSocket(Socket socket) {
        ::close(native(socket));
    }
#endif

    Socket connectTo(const std::string &host, uint16_t port) {
        if (!ensureInitialized()) {
            std::cerr << "Failed to initialize networking" << std::endl;
            return INVALID_SOCKET_HANDLE;
        }

        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo *addresses = nullptr;
        if (int error = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &addresses); error != 0) {
            std::cerr << "Cannot resolve " << host << ": " << gai_strerror(error) << std::endl;
            return INVALID_SOCKET_HANDLE;
        }

        Socket result = INVALID_SOCKET_HANDLE;
        for (addrinfo *address = addresses; address != nullptr; address = address->ai_next) {
            auto fd = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol);
#ifdef _WIN32
            if (fd == INVALID_SOCKET) {
                continue;
            }
#else
            if (fd < 0) {
                continue;
            }
#endif
            if (::connect(fd, address->ai_addr, static_cast<int>(address->ai_addrlen)) == 0) {
                result = static_cast<Socket>(fd);
                break;
            }
            closeSocket(static_cast<Socket>(fd));
        }
        freeaddrinfo(addresses);
        return result;
    }

    bool sendAll(Socket socket, const char *buffer, size_t length) {
        size_t totalSent = 0;
        while (totalSent < length) {
            // Winsock takes an int length; frames are capped far below INT_MAX anyway.
            size_t chunk = std::min<size_t>(length - totalSent, INT_MAX);
            auto lastSent = ::send(native(socket), buffer + totalSent, static_cast<int>(chunk), SEND_FLAGS);
            if (lastSent < 0) {
                if (interrupted()) continue;
                return false;
            }
            totalSent += static_cast<size_t>(lastSent);
        }
        return true;
    }

    bool recvAll(Socket socket, char *buffer, size_t length) {
        size_t totalReceived = 0;
        while (totalReceived < length) {
            size_t chunk = std::min<size_t>(length - totalReceived, INT_MAX);
            auto lastReceived = ::recv(native(socket), buffer + totalReceived, static_cast<int>(chunk), 0);
            if (lastReceived < 0) {
                if (interrupted()) continue;
                return false;
            }
            if (lastReceived == 0) {
                return false;
            }
            totalReceived += static_cast<size_t>(lastReceived);
        }
        return true;
    }
}
