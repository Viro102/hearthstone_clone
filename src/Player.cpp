#include "Player.h"

Player::Player(int hp, int id, string archetype) : m_archetype(archetype), m_hp(hp), m_id(id), m_mana(0),
                                                   m_currentMaxMana(0), m_turn(false), m_hand(new Hand()),
                                                   m_deck(new Deck("res/m_cards.txt")), m_board(new Board()),
                                                   m_random(new Random()) {
}

Card Player::drawCard() {
    if (!m_hand.isFull()) {
        if (!m_deck.getCards().empty()) {
            auto drawnCard = m_deck.getCard(m_random.nextInt(m_deck.getNumOfCards()));
            m_hand.addCard(drawnCard);
            m_deck.removeCard(drawnCard);
            return drawnCard;
        } else {
            cout << "You have no more m_cards in your m_deck\n";
            return nullptr;
        }
    }
    cout << "Your m_hand is full\n";
    return nullptr;
}

Card Player::playCard(int i) {
    auto card = m_hand.getCard(i);
    if (m_board.isFull()) {
        cout << "Your m_board is full\n";
        return nullptr;
    }
    if (m_mana >= card.getCost()) {
        m_mana -= card.getCost();
        if (card.getType() == "spell" || card.getType() == "aoe") {
            return card;
        }
        m_hand.removeCard(i);
        m_board.addCard(card);
        return card;
    } else {
        cout << "Not enough m_mana\n";
        return nullptr;
    }
}

Board Player::getBoard() const {
    return m_board;
}

Deck Player::getDeck() const {
    return m_deck;
}

Hand Player::getHand() const {
    return m_hand;
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
        cout << "You have reached the maximum m_mana";
    }
    m_currentMaxMana = m_mana;
}

bool Player::isTurn() const {
    return m_turn;
}

void Player::setTurn(bool turn) {
    m_turn = turn;
}
