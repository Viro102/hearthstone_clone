#include <Game.h>

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
    m_turnCounter++;
    cout << "Turn ended!\n";
}


void Game::playACard(int i) {
    auto card = getOnTurnPlayer().playCard(i);
    if (card != nullptr) {
        specialCard(*card);
    }
}

void Game::selectCardBoard(int i) {
    if (m_selectedCard == nullptr) {
        m_selectedCard = std::make_unique<Card>(getOnTurnPlayer().getBoard().getCard(i));

    } else if (*m_selectedCard == getOnTurnPlayer().getBoard().getCard(i)) {
        m_selectedCard = nullptr;
    }
}

void Game::attack(int i) {
    if (m_selectedCard == nullptr) {
        cout << "No card selected!\n";
        return;
    }
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

void Game::specialCard(const Card &card) {
    if (card.getType() == "buff") {
        int buffAmount = card.getBuffAmount();
        for (const auto &c: getOnTurnPlayer().getBoard().getCards()) {
            if (c != nullptr) {
                c->setHp(c->getHp() + buffAmount);
                c->setDamage(c->getDamage() + buffAmount);
            }
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
            if (target != nullptr) {
                target->setHp(target->getHp() - card.getDamage());
                if (target->getHp() <= 0) {
                    getOffTurnPlayer().getBoard().removeCard(*target);
                }
            }
        }
    }
}

void Game::addPlayer(Player player, int i) {
    m_players[i] = std::make_unique<Player>(std::move(player));
}

void Game::print() const {
    cout << "---- Game State ----" << endl;
    cout << "Turn Counter: " << m_turnCounter << endl;

    if (isSelected()) {
        cout << "Selected Card: " << m_selectedCard->getName()
             << " | HP: " << m_selectedCard->getHp()
             << " | Damage: " << m_selectedCard->getDamage() << endl;
    } else {
        cout << "No card selected" << endl;
    }

    for (int i = 0; i < m_players.size(); i++) {
        if (m_players[i]) {
            cout << "Player " << i + 1 << " (" << m_players[i]->getArchetype() << "):" << endl;
            cout << "  HP: " << m_players[i]->getHp() << endl;
            cout << "  Mana: " << m_players[i]->getMana() << endl;
            cout << "  Cards in Hand: " << m_players[i]->getHand().getNumOfCards() << endl;
            for (const auto &card: m_players[i]->getHand().getCards()) {
                if (card != nullptr) {
                    cout << "    Card: " << card->getName()
                         << " | HP: " << card->getHp()
                         << " | Damage: "
                         << card->getDamage() << endl;
                }
            }
            cout << "  Board Cards: " << m_players[i]->getBoard().getNumOfCards() << endl;
            for (const auto &card: m_players[i]->getBoard().getCards()) {
                if (card != nullptr) {
                    cout << "    Card: " << card->getName()
                         << " | HP: " << card->getHp()
                         << " | Damage: "
                         << card->getDamage() << endl;
                }
            }
            cout << (m_players[i]->isTurn() ? "  Current Turn" : "  Waiting") << endl;
        }
    }

    cout << "----------------------------------------------" << endl << endl;
}
