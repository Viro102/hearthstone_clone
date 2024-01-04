#pragma once

#include "Player.h"
#include "Common.h"
#include "Card.h"
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <atomic>

class Game {
public:
    void startGame(const string &player1, const string &player2);

    void endTurn();

    void playACard(Player &player, int i);

    void selectCardBoard(const Player &player, int i);

    void selectCardHand(const Player &player, int i);

    void attack(int i);

    void attackFace();

    Player &getOnTurnPlayer() const;

    Player &getOffTurnPlayer() const;

    array<std::unique_ptr<Player>, 2> &getPlayers();

    bool isSelected() const;

    void isGameOver() const;

    void specialCard(const Player &player, const Card &card);

    // NETWORKING
    // --------------------------------------------------------------------------------------
    // Initialize and start the server
    void startServer(short port);

    // Main loop for listening to clients
    void listenForClients();

    // Handle individual client
    void handleClient(int client_socket);

    // Process incoming messages from clients
    void processMessage(int client_socket, const std::string &message);

    // Send the game state to all connected clients
    void broadcastGameState();

    // Convert the game state to a string for sending
    std::string serializeGameState();

    // Safely shutdown the server
    void stopServer();

    // --------------------------------------------------------------------------------------

private:
    const int PLAYER_MAX_HP{20};
    array<std::unique_ptr<Player>, 2> m_players{};
    std::unique_ptr<Card> m_selectedCard;
    int m_turnCounter{0};

    // NETWORKING
    int m_server_fd{-1};
    std::vector<int> m_client_sockets;  // Keep track of each client's socket
    std::atomic<bool> m_is_running{false};     // For safely stopping the server
    std::mutex m_client_sockets_mutex;
};
