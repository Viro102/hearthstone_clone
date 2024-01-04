#pragma once

#include "Common.h"
#include "Card.h"

class Hand {
private:
    const int MAX_CARDS{5};
public:
    void addCard(const Card &card);

    void removeCard(int i);

    void removeCard(const Card &card);

    const Card &getCard(int i) const;

    vector<std::unique_ptr<Card>> &getCards();

    int getNumOfCards() const;

    bool isFull() const;

    void printHand() const;

private:
    vector<std::unique_ptr<Card>> m_cards;
};
