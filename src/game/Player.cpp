#include <Player.h>

Player::Player(int id, string archetype) : m_archetype(std::move(archetype)), m_id(id) {
    m_deck->makeDeck("../assets/cards.txt");
}

std::unique_ptr<Card> Player::drawCard() {
    if (!m_hand->isFull()) {
        if (!m_deck->isEmpty()) {
            m_deck->shuffleDeck();
            auto drawnCard = m_deck->getCard(0);
            if (drawnCard.has_value()) {
                auto &dc = drawnCard->get();
                m_hand->addCard(dc);
                m_deck->removeCard(dc);
                return std::make_unique<Card>(dc);
            }
        } else {
            cout << "You have no more cards in your deck\n";
            return nullptr;
        }
    }
    cout << "Your hand is full!\n";
    return nullptr;
}

std::unique_ptr<Card> Player::playCard(int i) {
    auto card = m_hand->getCard(i);
    if (card.has_value()) {
        auto &c = card->get();

        if (m_board->isFull()) {
            cout << "Your board is full!\n";
            return nullptr;
        }
        if (m_mana >= c.getCost()) {
            m_mana -= c.getCost();
            if (c.getType() == "spell" || c.getType() == "aoe") {
                return std::make_unique<Card>(c);
            }
            m_board->addCard(c);
            m_hand->removeCard(i);
            return std::make_unique<Card>(c);
        } else {
            cout << "Not enough Mana!\n";
            return nullptr;
        }
    } else {
        cout << "No card at index " << i << endl;
        return nullptr;
    }
}

CardContainer<5> &Player::getBoard() const {
    return *m_board;
}

Deck &Player::getDeck() const {
    return *m_deck;
}

CardContainer<5> &Player::getHand() const {
    return *m_hand;
}

int Player::getHp() const {
    return m_hp;
}

int Player::getId() const {
    return m_id;
}

string Player::getHpString() const {
    return std::to_string(m_hp);
}

int Player::getMana() const {
    return m_mana;
}

string Player::getManaString() const {
    return std::to_string(m_mana);
}

int Player::getCurrentMaxMana() const {
    return m_currentMaxMana;
}

string Player::getArchetype() const {
    return m_archetype;
}

void Player::setHp(int hp) {
    m_hp = hp;
}


void Player::setMana(int mana) {
    if (MAX_MANA >= mana) {
        m_mana = mana;
    } else {
        m_mana = MAX_MANA;
        cout << "You have reached the maximum Mana";
    }
    m_currentMaxMana = m_mana;
}

bool Player::isTurn() const {
    return m_turn;
}

void Player::setTurn(bool turn) {
    m_turn = turn;
}

void Player::setDeck(std::unique_ptr<Deck> deck) {
    m_deck = std::move(deck);
}

void Player::setHand(std::unique_ptr<CardContainer<5>> hand) {
    m_hand = std::move(hand);
}

void Player::setBoard(std::unique_ptr<CardContainer<5>> board) {
    m_board = std::move(board);
}
