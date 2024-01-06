#pragma once

#include <Common.h>
#include <Card.h>

class CardContainer {
public:

    CardContainer() = default;

    virtual ~CardContainer() = default;

    void addCard(const Card &card);

    void removeCard(int i);

    void removeCard(const Card &card);

    Card &getCard(int i);

    vector<std::unique_ptr<Card>> &getCards();

    [[nodiscard]] int getNumOfCards() const;

    [[nodiscard]] string getNumOfCardsString() const;

    [[nodiscard]] bool isFull() const;

    void print() const;

    [[nodiscard]] virtual int getMaxCards() const;

private:
    vector<std::unique_ptr<Card>> m_cards;
    int m_numberOfCards{};
};
