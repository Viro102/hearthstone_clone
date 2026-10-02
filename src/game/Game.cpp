#include <Game.h>

Game::Game(Player player1, Player player2) {
    m_players[0] = std::make_unique<Player>(std::move(player1));
    m_players[1] = std::make_unique<Player>(std::move(player2));
}

void Game::startGame() {
    // Only the server runs the game; clients receive decks through the serialized state.
    for (const auto &player: m_players) {
        player->getDeck().makeDeck(assetPath("cards.txt"));
    }
    m_players[0]->setTurn(true);
    m_players[0]->setMana(1);
    for (int i = 0; i < 3; i++) {
        m_players[0]->drawCard();
        m_players[1]->drawCard();
    }
}

void Game::endTurn() {
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
    deselectCard();
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
    auto card = getOnTurnPlayer().getBoard().getCard(i);
    if (card.has_value()) {
        if (m_selectedBoardIndex == i) {
            deselectCard();
        } else if (!m_selectedCard.has_value()) {
            // Select this minion, or switch to it from another minion. A selected spell stays selected
            // because its mana is already spent.
            m_selectedBoardIndex = i;
        }
    } else {
        cout << "No card on board with index " << i << endl;
    }
}

void Game::attack(int i) {
    Card *attacker = resolveSelectedCard();
    if (attacker == nullptr) {
        cout << "No card selected!\n";
        return;
    }
    const auto &opponent = getOffTurnPlayer();
    const auto &currentPlayer = getOnTurnPlayer();

    if (auto targetCard = opponent.getBoard().getCard(i); targetCard.has_value()) {
        auto &tc = targetCard->get();
        // Minions must attack a taunt first; spells can target anything
        if (m_selectedBoardIndex >= 0 && tc.getType() != "taunt" && hasTaunt(opponent)) {
            cout << "Taunt card in play, attack it first" << endl;
            return;
        }
        if (!attacker->getHasAttacked()) {
            tc.setHp(tc.getHp() - attacker->getDamage());
            attacker->setHp(attacker->getHp() - tc.getDamage());
            attacker->setHasAttacked(true);
            if (tc.getHp() <= 0) {
                opponent.getBoard().removeCard(i);
            }
            if (m_selectedBoardIndex >= 0 && attacker->getHp() <= 0) {
                currentPlayer.getBoard().removeCard(m_selectedBoardIndex);
            }
        } else {
            cout << "Card has already attacked!" << endl;
        }
    }

    deselectCard();
}

void Game::attackFace() {
    auto &target = getOffTurnPlayer();
    Card *attacker = resolveSelectedCard();

    if (attacker == nullptr) {
        cout << "No card selected" << endl;
        return;
    }

    if (m_selectedBoardIndex >= 0 && hasTaunt(target)) {
        cout << "Taunt card in play, cannot attack hero" << endl;
        return;
    }
    if (!attacker->getHasAttacked()) {
        target.setHp(target.getHp() - attacker->getDamage());
        attacker->setHasAttacked(true);
    } else {
        cout << "Card has already attacked" << endl;
    }

    deselectCard();
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

bool Game::hasTaunt(const Player &player) {
    return std::ranges::any_of(player.getBoard().getCards(), [](const auto &card) {
        return card != nullptr && card->getType() == "taunt";
    });
}

bool Game::isSelected() const {
    return m_selectedCard.has_value() || m_selectedBoardIndex >= 0;
}

Card *Game::resolveSelectedCard() {
    if (m_selectedBoardIndex >= 0) {
        auto card = getOnTurnPlayer().getBoard().getCard(m_selectedBoardIndex);
        return card.has_value() ? &card->get() : nullptr;
    }
    return m_selectedCard.has_value() ? &*m_selectedCard : nullptr;
}

bool Game::checkGameOver() const {
    if (m_players[0]->getHp() <= 0) {
        cout << "Player 2 (" + m_players[1]->getArchetype() + ") wins\n";
        return true;
    } else if (m_players[1]->getHp() <= 0) {
        cout << "Player 1 (" + m_players[0]->getArchetype() + ") wins\n";
        return true;
    }
    return false;
}

bool Game::isDraw() const {
    return m_turnCounter >= MAX_TURNS && getWinnerId() == -1;
}

int Game::getWinnerId() const {
    if (m_players[0]->getHp() <= 0) {
        return m_players[1]->getId();
    }
    if (m_players[1]->getHp() <= 0) {
        return m_players[0]->getId();
    }
    return -1;
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
        m_selectedCard = card;
        m_selectedBoardIndex = -1;
        return;
    }

    if (card.getType() == "aoe") {
        deselectCard();
        auto &board = getOffTurnPlayer().getBoard();
        for (int i = 0; i < static_cast<int>(board.getCards().size()); i++) {
            if (auto target = board.getCard(i); target.has_value()) {
                auto &t = target->get();
                t.setHp(t.getHp() - card.getDamage());
                if (t.getHp() <= 0) {
                    board.removeCard(i);
                }
            }
        }
    }
}

void Game::print() const {
    cout << "---- Game State ----" << endl;
    cout << "Turn Counter: " << m_turnCounter << endl;

    if (auto selected = getSelectedCard(); selected.has_value()) {
        cout << "Selected Card: " << selected->getName()
             << " | HP: " << selected->getHp()
             << " | Damage: " << selected->getDamage() << endl;
    } else {
        cout << "No card selected" << endl;
    }

    for (size_t i = 0; i < m_players.size(); i++) {
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

void Game::initializeFromJson(const json &jsonState) {
    array<int, 2> id{};
    array<string, 2> archetype{};

    if (jsonState.contains("players")) {
        // A game has exactly two players; never trust the message to say otherwise
        for (size_t i = 0; i < std::min<size_t>(jsonState["players"].size(), id.size()); i++) {
            const auto &j = jsonState["players"][i];
            id[i] = j["id"];
            archetype[i] = j["archetype"];
        }
    }

    m_players = {std::make_unique<Player>(id[0], archetype[0]), std::make_unique<Player>(id[1], archetype[1])};
}

Player &Game::getPlayer(int id) const {
    static Player empty(-1, "");
    for (const auto &player: m_players) {
        if (player->getId() == id) {
            return *player;
        }
    }
    return empty;
}

std::optional<Card> Game::getSelectedCard() const {
    if (m_selectedBoardIndex >= 0) {
        auto card = getOnTurnPlayer().getBoard().getCard(m_selectedBoardIndex);
        if (card.has_value()) {
            return card->get();
        }
        return std::nullopt;
    }
    return m_selectedCard;
}

void Game::setSelectedCard(Card &card) {
    m_selectedCard = card;
    m_selectedBoardIndex = -1;
}

void Game::deselectCard() {
    m_selectedCard = std::nullopt;
    m_selectedBoardIndex = -1;
}
