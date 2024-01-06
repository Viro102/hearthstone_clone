#include <CardContainer.h>

void CardContainer::addCard(const Card &card) {
    if (!isFull()) {
        m_numberOfCards++;
        m_cards.push_back(std::make_unique<Card>(card));
    } else {
        cout << "Container is full!" << endl;
    }
}

void CardContainer::removeCard(int i) {
    if (m_numberOfCards > 0) {
        m_numberOfCards--;
        std::erase(m_cards, m_cards[i]);
    } else {
        cout << "Container is empty!" << endl;
    }
}

void CardContainer::removeCard(const Card &card) {
    auto iterator = std::ranges::find_if(m_cards, [&card](const auto &ptr) {
        return ptr.get() == &card;  // Compare addresses
    });

    if (iterator != m_cards.end()) {
        m_cards.erase(iterator);  // Remove the found card
        m_numberOfCards--;
    }
}

Card &CardContainer::getCard(int i) {
    return *m_cards[i];
}

vector<std::unique_ptr<Card>> &CardContainer::getCards() {
    return m_cards;
}

int CardContainer::getNumOfCards() const {
    return m_numberOfCards;
}

string CardContainer::getNumOfCardsString() const {
    return std::to_string(m_numberOfCards);
}

bool CardContainer::isFull() const {
    return m_numberOfCards >= getMaxCards();
}

void CardContainer::print() const {
    for (auto &card: m_cards) {
        cout << card->getName() << " "
             << card->getType() << " "
             << " HP: " << card->getHp()
             << " DMG: " << card->getDamage()
             << " COST: " << card->getCost() << endl;
    }
}

int CardContainer::getMaxCards() const {
    return 5;
}
