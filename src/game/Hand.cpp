#include "../../include/Hand.h"

void Hand::addCard(const Card &card) {
    m_cards.push_back(std::make_unique<Card>(card));
}

void Hand::removeCard(int i) {
    std::erase(m_cards, m_cards[i]);
}

void Hand::removeCard(const Card &card) {
    auto it = std::find_if(m_cards.begin(), m_cards.end(), [&card](const std::unique_ptr<Card> &ptr) {
        return ptr.get() == &card;  // Compare addresses
    });

    if (it != m_cards.end()) {
        m_cards.erase(it);  // Remove the found card
    }
}

const Card &Hand::getCard(int i) const {
    return *m_cards[i];
}

vector<std::unique_ptr<Card>> &Hand::getCards() {
    return m_cards;
}

int Hand::getNumOfCards() const {
    return m_cards.size();
}

bool Hand::isFull() const {
    return this->getNumOfCards() == MAX_CARDS;
}

void Hand::printHand() const {
    for (auto &card: m_cards) {
        cout << card->getName() << " "
             << card->getType() << "\nHP: " << card->getHp()
             << " DMG: " << card->getDamage()
             << " COST: " << card->getCost() << "\n";
    }
}