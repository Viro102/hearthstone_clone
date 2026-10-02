#pragma once

#include <Common.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <optional>

// Every message in both directions is framed as a 4-byte big-endian length followed by the JSON payload.
namespace protocol {
#ifdef MSG_NOSIGNAL
    constexpr int SEND_FLAGS = MSG_NOSIGNAL;
#else
    constexpr int SEND_FLAGS = 0;
#endif

    constexpr uint32_t MAX_MESSAGE_SIZE = 1 << 20;

    inline bool sendAll(int socket, const char *buffer, size_t length) {
        size_t totalSent = 0;
        while (totalSent < length) {
            ssize_t lastSent = send(socket, buffer + totalSent, length - totalSent, SEND_FLAGS);
            if (lastSent == -1) {
                if (errno == EINTR) continue;
                return false;
            }
            totalSent += lastSent;
        }
        return true;
    }

    inline bool recvAll(int socket, char *buffer, size_t length) {
        size_t totalReceived = 0;
        while (totalReceived < length) {
            ssize_t lastReceived = recv(socket, buffer + totalReceived, length - totalReceived, 0);
            if (lastReceived == -1) {
                if (errno == EINTR) continue;
                return false;
            }
            if (lastReceived == 0) {
                return false;
            }
            totalReceived += lastReceived;
        }
        return true;
    }

    inline bool sendFrame(int socket, const string &payload) {
        uint32_t messageLength = htonl(static_cast<uint32_t>(payload.size()));
        return sendAll(socket, reinterpret_cast<const char *>(&messageLength), sizeof(messageLength)) &&
               sendAll(socket, payload.data(), payload.size());
    }

    inline std::optional<string> recvFrame(int socket) {
        uint32_t messageLength;
        if (!recvAll(socket, reinterpret_cast<char *>(&messageLength), sizeof(messageLength))) {
            return std::nullopt;
        }
        messageLength = ntohl(messageLength);
        if (messageLength > MAX_MESSAGE_SIZE) {
            std::cerr << "Message too large: " << messageLength << " bytes" << endl;
            return std::nullopt;
        }

        string message(messageLength, '\0');
        if (!recvAll(socket, message.data(), messageLength)) {
            return std::nullopt;
        }
        return message;
    }
}
