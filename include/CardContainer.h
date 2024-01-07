#pragma once

#include <Common.h>
#include <Card.h>

template<int MAX_CARDS>
class CardContainer {
public:
    void addCard(const Card &card) {
        if (!isFull()) {
            for (auto &slot: m_cards) {
                if (slot == nullptr) {
                    slot = std::make_unique<Card>(card);
                    m_numberOfCards++;
                    return;
                }
            }
        } else {
            cout << "Container is full!" << endl;
        }
    };

    void removeCard(int i) {
        if (m_numberOfCards > 0) {
            m_numberOfCards--;
            m_cards[i] = nullptr;
        } else {
            cout << "Container is empty!" << endl;
        }
    };

    void removeCard(const Card &card) {
        for (size_t i = 0; i < MAX_CARDS; ++i) {
            if (m_cards[i] != nullptr && m_cards[i].get() == &card) {
                m_cards[i] = nullptr;
                m_numberOfCards--;
                return;
            }
        }
    };

    Card &getCard(int i) {
        static Card empty;
        if (i >= 0 && i < MAX_CARDS && m_cards[i] != nullptr) {
            return *m_cards[i];
        } else {
            return empty;
        }
    };

    array<std::unique_ptr<Card>, MAX_CARDS> &getCards() {
        return m_cards;
    };

    [[nodiscard]] int getNumOfCards() const {
        return m_numberOfCards;
    };

    [[nodiscard]] string getNumOfCardsString() const {
        return std::to_string(m_numberOfCards);
    };

    [[nodiscard]] bool isFull() const {
        return m_numberOfCards >= MAX_CARDS;
    };

    void print() const {
        for (auto &card: m_cards) {
            cout << card->getName() << " "
                 << card->getType() << " "
                 << " HP: " << card->getHp()
                 << " DMG: " << card->getDamage()
                 << " COST: " << card->getCost() << endl;
        }
    };

    [[nodiscard]] int getMaxCards() const {
        return MAX_CARDS;
    };

private:
    array<std::unique_ptr<Card>, MAX_CARDS> m_cards;
    int m_numberOfCards{};
};
