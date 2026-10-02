#pragma once

#include <Common.h>
#include <Card.h>
#include <optional>

template<int MAX_CARDS>
class CardContainer {
public:
    void print() const {
        for (const auto &card: m_cards) {
            if (card) {
                card->print();
                cout << endl;
            }
        }
    }

    void addCard(const Card &card) {
        if (!isFull()) {
            for (auto &slot: m_cards) {
                if (!slot) {
                    slot = std::make_unique<Card>(card);
                    m_numberOfCards++;
                    return;
                }
            }
        } else {
            cout << "Container is full!" << endl;
        }
    };

    void placeCard(int i, const Card &card) {
        if (i < 0 || i >= MAX_CARDS) {
            return;
        }
        if (!m_cards[i]) {
            m_numberOfCards++;
        }
        m_cards[i] = std::make_unique<Card>(card);
    }

    void removeCard(int i) {
        if (i < 0 || i >= MAX_CARDS || m_cards[i] == nullptr) {
            cout << "No card to remove at index " << i << endl;
            return;
        }
        m_cards[i] = nullptr;
        m_numberOfCards--;
    };

    std::optional<std::reference_wrapper<Card>> getCard(int i) {
        if (i >= 0 && i < MAX_CARDS && m_cards[i] != nullptr) {
            return *m_cards[i];
        }
        return std::nullopt;
    };

    [[nodiscard]] int getFirstCardIndex() const {
        for (int i = 0; i < MAX_CARDS; ++i) {
            if (m_cards[i]) {
                return i;
            }
        }
        return -1;
    }

    [[nodiscard]] const array<std::unique_ptr<Card>, MAX_CARDS> &getCards() const {
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

    [[nodiscard]] bool isEmpty() const {
        return m_numberOfCards <= 0;
    };

    // Empty slots are serialized as null so that indices stay identical on server and client.
    [[nodiscard]] json serialize() const {
        json cardsJson = json::array();
        for (const auto &card: m_cards) {
            cardsJson.push_back(card ? card->serialize() : json(nullptr));
        }
        return cardsJson;
    };

protected:
    array<std::unique_ptr<Card>, MAX_CARDS> m_cards{};
    int m_numberOfCards{};
};

template<typename ContainerType>
std::unique_ptr<ContainerType> deserialize(const json &jsonArray) {
    auto container = std::make_unique<ContainerType>();
    int i = 0;
    for (const auto &cardJson: jsonArray) {
        if (!cardJson.is_null()) {
            Card newCard = Card::createFromJson(cardJson);
            if (!newCard.getName().empty()) {
                container->placeCard(i, newCard);
            }
        }
        i++;
    }
    return container;
}
