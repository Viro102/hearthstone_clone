#include "../../include/Game.h"
#include "../../include/Server.h"

void Game::startGame(const string &player1, const string &player2) {
    m_players[0] = std::make_unique<Player>(PLAYER_MAX_HP, 0, player1);
    m_players[1] = std::make_unique<Player>(PLAYER_MAX_HP, 1, player2);
    m_players[0]->setTurn(true);
    m_players[0]->setMana(1);
    for (int i = 0; i < 3; i++) {
        m_players[0]->drawCard();
        m_players[1]->drawCard();
    }
}

void Game::endTurn() {
    if (m_turnCounter >= 30) {
        cout << "Game over, no one won" << endl;
        // TODO: exit to menu
    }
    auto &currentPlayer = getOnTurnPlayer();
    auto &offPlayer = getOffTurnPlayer();

    currentPlayer.setTurn(false);
    offPlayer.setTurn(true);
    offPlayer.setMana(offPlayer.getCurrentMaxMana() + 1);
    offPlayer.drawCard();

    for (const auto &card: currentPlayer.getBoard().getCards()) {
        if (card != nullptr) {
            card->setHasAttacked(false);
        }
    }
    if (m_selectedCard != nullptr) {
        m_selectedCard = nullptr;
    }
//        panel.removeGlow();
    m_turnCounter++;
    cout << "Turn ended!\n";
}


void Game::playACard(Player &player, int i) {
    auto card = player.playCard(i);
    if (card != nullptr) {
        specialCard(player, *card);
    }
}

void Game::selectCardBoard(const Player &player, int i) {
    if (m_selectedCard == nullptr) {
        m_selectedCard = std::make_unique<Card>(player.getBoard().getCard(i));
//            panel.addGlow(i, "m_board");

    } else if (*m_selectedCard == player.getBoard().getCard(i)) {
        m_selectedCard = nullptr;
//            panel.removeGlow();
    }
}

void Game::selectCardHand(const Player &player, int i) {
    if (*m_selectedCard == player.getHand().getCard(i)) {
        m_selectedCard = nullptr;
//            panel.removeGlow();
    } else {
        m_selectedCard = std::make_unique<Card>(player.getHand().getCard(i));
//            panel.addGlow(i, "m_hand");
    }
}

void Game::attack(int i) {
    if (m_selectedCard == nullptr) {
        cout << "No card selected!\n";
        return;
    }
//        panel.removeGlow();
    const auto &opponent = getOffTurnPlayer();
    const auto &currentPlayer = getOnTurnPlayer();
    auto &targetCard = opponent.getBoard().getCard(i);

    if (!m_selectedCard->getHasAttacked()) {
        targetCard.setHp(targetCard.getHp() - m_selectedCard->getDamage());
        m_selectedCard->setHp(m_selectedCard->getHp() - targetCard.getDamage());
        m_selectedCard->setHasAttacked(true);
        if (targetCard.getHp() <= 0) {
            opponent.getBoard().removeCard(i);
        }
        if (m_selectedCard->getHp() <= 0) {
            currentPlayer.getBoard().removeCard(*m_selectedCard);
        }
    } else {
        cout << "Card has already attacked!" << endl;
    }

    m_selectedCard = nullptr;
}

void Game::attackFace() {
    auto &target = getOffTurnPlayer();
    const auto &attacker = getOnTurnPlayer();

    if (m_selectedCard == nullptr) {
        cout << "No card selected";
        return;
    }

//        panel.removeGlow();

    for (const auto &c: target.getBoard().getCards()) {
        if (c == nullptr) {
            continue;
        }
        if (c->getType() == "taunt") {
            cout << "Taunt card in play, cannot attack hero" << endl;
            return;
        }
    }
    if (!m_selectedCard->getHasAttacked()) {
        target.setHp(target.getHp() - m_selectedCard->getDamage());
        m_selectedCard->setHasAttacked(true);
        if (m_selectedCard->getType() == "spell") {
            attacker.getBoard().removeCard(*m_selectedCard);
        }
    } else {
        cout << "Card has already attacked" << endl;
    }

    m_selectedCard = nullptr;
    isGameOver();
}

Player &Game::getOnTurnPlayer() const {
    for (auto &player: m_players) {
        if (player != nullptr && player->isTurn()) {
            return *player;
        }
    }
    throw std::runtime_error("No player is currently on turn.");
}


Player &Game::getOffTurnPlayer() const {
    for (auto &player: m_players) {
        if (player != nullptr && !player->isTurn()) {
            return *player;
        }
    }
    throw std::runtime_error("No player is currently off turn.");
}


array<std::unique_ptr<Player>, 2> &Game::getPlayers() {
    return m_players;
}

bool Game::isSelected() const {
    return m_selectedCard != nullptr;
}

void Game::isGameOver() const {
    if (m_players[0]->getHp() <= 0) {
        cout << "Player 2 (" + m_players[1]->getArchetype() + ") wins\n";
    } else if (m_players[1]->getHp() <= 0) {
        cout << "Player 1 (" + m_players[0]->getArchetype() + ") wins\n";
    }
}

void Game::specialCard(const Player &player, const Card &card) {
    if (card.getType() == "buff") {
        int buffAmount = card.getBuffAmount();
        for (const auto &c: player.getBoard().getCards()) {
            if (c == nullptr) {
                continue;
            }
            c->setHp(c->getHp() + buffAmount);
            c->setDamage(c->getDamage() + buffAmount);
        }
        return;
    }

    if (card.getType() == "spell") {
        getOnTurnPlayer().getHand().removeCard(card);
        m_selectedCard = std::make_unique<Card>(card);
        return;
    }

    if (card.getType() == "aoe") {
        getOnTurnPlayer().getHand().removeCard(card);
        m_selectedCard = nullptr;
        for (const auto &target: getOffTurnPlayer().getBoard().getCards()) {
            if (target == nullptr) {
                continue;
            }
            target->setHp(target->getHp() - card.getDamage());
            if (target->getHp() <= 0) {
                getOffTurnPlayer().getBoard().removeCard(*target);
            }
        }
    }
}

// NETWORKING
// Initialize and start the server
void Game::startServer(short port) {
    m_server_fd = Server::startServer(port);

    // After setting up the server socket, start listening and accepting clients
    m_is_running = true;
    std::jthread(&Game::listenForClients, this).detach();
}

// Main loop for listening to clients
void Game::listenForClients() {
    struct sockaddr_in client_addr{}; // Structure to hold client's address
    socklen_t client_addr_len = sizeof(client_addr); // Length of the client's address structure

    while (m_is_running) {
        int new_socket = accept(m_server_fd, (struct sockaddr *) &client_addr, &client_addr_len);

        // Check if accept was successful
        if (new_socket >= 0) {
            // Successfully accepted a new connection
            std::lock_guard<std::mutex> lock(m_client_sockets_mutex);
            m_client_sockets.push_back(new_socket);
            std::thread(&Game::handleClient, this, new_socket).detach();
        } else {
            // Error occurred while trying to accept a new connection
            perror("accept");
            // Decide how you want to handle this error. You might just print an error message and continue,
            // or you might want to implement some kind of recovery or shutdown procedure.
        }
    }
}

// Handle individual client
void Game::handleClient(int client_socket) {
    while (m_is_running) {
        char buffer[1024] = {0};
        int valread = recv(client_socket, buffer, 1024, 0);
        if (valread > 0) {
            std::string message(buffer, valread);
            processMessage(client_socket, message);
        } else if (valread == 0) {
            close(client_socket);
            break;
        }
    }
}

// Process incoming messages from clients
void Game::processMessage(int client_socket, const std::string &message) {
    // ... (parse the message and perform actions based on it)
    if (message == "play card") {
        // ... (extract which card)
//        playACard(*m_players[client_socket], cardIndex);
        broadcastGameState();
    }
    // Add more commands as needed
}

// Send the game state to all connected clients
void Game::broadcastGameState() {
    std::string state = serializeGameState();
    std::lock_guard<std::mutex> lock(m_client_sockets_mutex);
    for (int client_socket: m_client_sockets) {
        send(client_socket, state.c_str(), state.length(), 0);
    }
}

// Convert the game state to a string for sending
std::string Game::serializeGameState() {
    // ... (convert the game state into a string or JSON format)
    return "game state";
}

// Safely shutdown the server
void Game::stopServer() {
    m_is_running = false;
    close(m_server_fd);
}
