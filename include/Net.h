#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

// Minimal portable TCP socket layer (POSIX sockets / Winsock).
// This header deliberately includes no platform headers: <winsock2.h> pulls in <windows.h>, whose
// names (Rectangle, CloseWindow, DrawText, ...) clash with raylib.
namespace net {
    // Wide enough for a POSIX file descriptor and a Windows SOCKET handle.
    using Socket = std::intptr_t;

    constexpr Socket INVALID_SOCKET_HANDLE = -1;

    // Connects a TCP socket to host:port. Returns INVALID_SOCKET_HANDLE on failure.
    Socket connectTo(const std::string &host, uint16_t port);

    // Sends or receives exactly `length` bytes. Returns false on error or when the peer closed.
    bool sendAll(Socket socket, const char *buffer, size_t length);

    bool recvAll(Socket socket, char *buffer, size_t length);

    // Stops both directions, waking any thread blocked in recv() on this socket.
    void shutdownSocket(Socket socket);

    void closeSocket(Socket socket);

    // Description of the last socket error on this thread.
    std::string lastError();
}
