#include "../../include/Board.h"

void Board::addCard(const Card &card) {
    m_cards.push_back(std::make_unique<Card>(card));
}

void Board::removeCard(const Card &card) {
    std::erase(m_cards, std::make_unique<Card>(card));
}

void Board::removeCard(int i) {
    if (i < m_cards.size()) {
        m_cards.erase(m_cards.begin() + i);
    } else {
        cout << "Invalid index." << std::endl;
    }
}

Card &Board::getCard(int i) const {
    if (i >= 0 && i < m_cards.size()) {
        return *m_cards[i];
    }
    throw std::out_of_range("Index out of range in Board::getCard");
}

const vector<std::unique_ptr<Card>> &Board::getCards() const {
    return m_cards;
}

bool Board::isFull() const {
    return m_cards.size() == MAX_CARDS_ON_BOARD;
}

void Board::printBoard() const {
    for (auto &card: m_cards) {
        cout << card->getName() << " " << card->getType() << "\n";
        cout << "HP: " << card->getHp() << " DMG: " << card->getDamage() << " COST: " << card->getCost() << "\n";
    }
}
