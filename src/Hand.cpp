#include "Hand.h"

void Hand::addCard(const Card &card) {
    m_cards.push_back(card);
}

void Hand::removeCard(int i) {
    std::erase(m_cards, m_cards[i]);
}

void Hand::removeCard(const Card &card) {
    std::erase(m_cards, card);
}

const Card &Hand::getCard(int i) const {
    return m_cards[i];
}

const vector<Card> &Hand::getCards() const {
    return m_cards;
}

int Hand::getNumOfCards() const {
    return m_cards.size();
}

bool Hand::isFull() const {
    return this->getNumOfCards() == MAX_CARDS;
}

void Hand::printHand() const {
    for (const Card &card: m_cards) {
        cout << card.getName() << " "
             << card.getType() << "\nHP: " << card.getHp()
             << " DMG: " << card.getDamage()
             << " COST: " << card.getCost() << "\n";
    }
}