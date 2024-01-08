#pragma once

#include <Common.h>
#include <Card.h>

template<int MAX_CARDS>
class CardContainer {
public:
    CardContainer() {
        for (auto &card: m_cards) {
            card = std::make_unique<Card>();
        }
    };

    void print() const {
        for (const auto &card: m_cards) {
            card->print();
            cout << endl;
        }
    }

    void addCard(const Card &card) {
        if (!isFull()) {
            for (auto &slot: m_cards) {
                if (slot->getName().empty()) {
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
        for (int i = 0; i < MAX_CARDS; ++i) {
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

    [[nodiscard]] json serialize() const {
        json cardsJson = json::array();
        for (const auto &card: m_cards) {
            if (card) {
                cardsJson.push_back(card->serialize());
            }
        }
        return cardsJson;
    };

private:
    array<std::unique_ptr<Card>, MAX_CARDS> m_cards{};
    int m_numberOfCards{};
};

template<typename ContainerType>
std::unique_ptr<ContainerType> deserialize(const json &jsonArray) {
    auto container = std::make_unique<ContainerType>();
    for (const auto &cardJson: jsonArray) {
        Card newCard = Card::createFromJson(cardJson);
        if (!newCard.getName().empty()) {
            container->addCard(newCard);
        }
    }
    return container;
}
