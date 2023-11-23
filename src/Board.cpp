#include "Board.h"

void Board::addCard(const Card &card) {
    m_cards.push_back(card);
}

void Board::removeCard(const Card &card) {
    std::erase(m_cards, card);
}

void Board::removeCard(int i) {
    if (i < m_cards.size()) {
        m_cards.erase(m_cards.begin() + i);
    } else {
        std::cout << "Invalid index." << std::endl;
    }
}

Card Board::getCard(int i) {
    return m_cards[i];
}

const vector<Card> &Board::getCards() const {
    return m_cards;
}

bool Board::isFull() const {
    return m_cards.size() == MAX_CARDS_ON_BOARD;
}

void Board::printBoard() const {
    for (const Card &card: m_cards) {
        cout << card.getName() << " " << card.getType() << "\n";
        cout << "HP: " << card.getHp() << " DMG: " << card.getDamage() << " COST: " << card.getCost() << "\n";
    }
}
