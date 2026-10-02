#pragma once

#include <Common.h>
#include <LobbyState.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <functional>
#include <utility>
#include <GameState.h>
#include <Game.h>
#include <Net.h>
#include <Protocol.h>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <optional>

class Client {
public:
    explicit Client(net::Socket socket);

    Client() = default;

    ~Client();

    // Connects to the server and starts the listener thread. Must not be called while holding lockState().
    int start(uint16_t port, const string &host);

    // Disconnects and joins the listener thread. Must not be called while holding lockState().
    void shutdown();

    void sendMessage(const string &message, const json &data = json()) const;

    // The listener thread updates the lobby and game state while holding this lock;
    // hold it while reading or drawing that state, but not while waiting for the next frame.
    [[nodiscard]] std::unique_lock<std::mutex> lockState();

    // A state transition requested by the server since the last call, if any. Requires lockState().
    std::optional<GameState> takePendingState();

    // A message for the player (e.g. "Server is full"), empty if none. Requires lockState().
    [[nodiscard]] const string &getNotice() const;

    [[nodiscard]] net::Socket getSocket() const;

    // The getters below require lockState().

    [[nodiscard]] const LobbyState &getLobbyState() const;

    [[nodiscard]] Game &getGameplayState();

    [[nodiscard]] int getID() const;

    [[nodiscard]] bool isGameStateInitialized() const;

private:
    void listenToServer(net::Socket socket);

    void updateLocalLobbyState(const json &data);

    void updateLocalGameplayState(const json &data);

    void processMessage(const string &message);


    net::Socket m_socket{net::INVALID_SOCKET_HANDLE};
    std::jthread m_serverListener;
    std::atomic<bool> m_isShuttingDown{false};

    // Guards everything below.
    mutable std::mutex m_stateMutex;
    int m_ID{-1};
    LobbyState m_lobbyState{};
    Game m_gameplayState{};
    bool m_isGameStateInitialized{false};
    std::optional<GameState> m_pendingState;
    string m_notice;
};
