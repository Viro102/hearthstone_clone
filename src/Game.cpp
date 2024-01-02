#include "Game.h"

const int Game::PLAYER_MAX_HP{20};

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
//            JOptionPaneshowMessageDialog(panel, "Game over, no one won");
//            System.exit(0);
    }
    auto &currentPlayer = getOnTurnPlayer();
    auto &offPlayer = getOffTurnPlayer();

    currentPlayer.setTurn(false);
    offPlayer.setTurn(true);
    offPlayer.setMana(offPlayer.getCurrentMaxMana() + 1);
    offPlayer.drawCard();

    for (auto &card: currentPlayer.getBoard().getCards()) {
        if (card != nullptr) {
            card->setHasAttacked(false);
        }
    }
    if (m_selectedCard != nullptr) {
        m_selectedCard = nullptr;
    }
//        panel.removeGlow();
//        panel.update();
    m_turnCounter++;
    cout << "Turn ended!\n";
//        LoggingprintStateHand(players);
}


void Game::playACard(Player &player, int i) {
    auto card = player.playCard(i);
    if (card != nullptr) {
        specialCard(player, *card);
//            panel.update();
    }
}

void Game::selectCardBoard(Player &player, int i) {
    if (m_selectedCard == nullptr) {
        m_selectedCard = std::make_unique<Card>(player.getBoard().getCard(i));
//            panel.addGlow(i, "m_board");

    } else if (*m_selectedCard == player.getBoard().getCard(i)) {
        m_selectedCard = nullptr;
//            panel.removeGlow();
    } else {
//            JOptionPaneshowMessageDialog(panel, "Can't select two m_cards at once");
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
    auto &opponent = getOffTurnPlayer();
    auto &currentPlayer = getOnTurnPlayer();
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
//            JOptionPaneshowMessageDialog(panel, "Card has already attacked!");
    }

    m_selectedCard = nullptr;
//        panel.update();
}

void Game::attackFace() {
    auto &target = getOffTurnPlayer();
    auto &attacker = getOnTurnPlayer();

    if (m_selectedCard == nullptr) {
        cout << "No card selected";
        return;
    }

//        panel.removeGlow();

    for (auto &c: target.getBoard().getCards()) {
        if (c == nullptr) {
            continue;
        }
        if (c->getType() == "taunt") {
//                JOptionPaneshowMessageDialog(panel, "Taunt card in play, cannot attack hero");
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
//            JOptionPaneshowMessageDialog(panel, "Card has already attacked!");
    }

    m_selectedCard = nullptr;
//        panel.update();
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

void Game::specialCard(Player &player, const Card &card) {
    if (card.getType() == "buff") {
        int buffAmount = card.getBuffAmount();
        for (auto &c: player.getBoard().getCards()) {
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
        for (auto &target: getOffTurnPlayer().getBoard().getCards()) {
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