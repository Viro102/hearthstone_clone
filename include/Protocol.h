#pragma once

#include <Common.h>
#include <Net.h>
#include <cstdint>
#include <optional>

// Every message in both directions is framed as a 4-byte big-endian length followed by the JSON payload.
namespace protocol {
    constexpr uint32_t MAX_MESSAGE_SIZE = 1 << 20;

    inline bool sendFrame(net::Socket socket, const string &payload) {
        // Header and payload go out in a single write: two small writes would trigger Nagle's algorithm
        // together with delayed ACKs and add ~40ms to every message.
        auto length = static_cast<uint32_t>(payload.size());
        string frame{
                static_cast<char>(length >> 24), static_cast<char>(length >> 16),
                static_cast<char>(length >> 8), static_cast<char>(length)};
        frame += payload;
        return net::sendAll(socket, frame.data(), frame.size());
    }

    inline std::optional<string> recvFrame(net::Socket socket) {
        unsigned char header[4];
        if (!net::recvAll(socket, reinterpret_cast<char *>(header), sizeof(header))) {
            return std::nullopt;
        }
        uint32_t messageLength = (uint32_t{header[0]} << 24) | (uint32_t{header[1]} << 16) |
                                 (uint32_t{header[2]} << 8) | uint32_t{header[3]};
        if (messageLength > MAX_MESSAGE_SIZE) {
            std::cerr << "Message too large: " << messageLength << " bytes" << endl;
            return std::nullopt;
        }

        string message(messageLength, '\0');
        if (!net::recvAll(socket, message.data(), messageLength)) {
            return std::nullopt;
        }
        return message;
    }
}
