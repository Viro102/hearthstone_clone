#pragma once

#include <ranges>
#include <algorithm>
#include <memory>
#include "Card.h"

class Board {
private:
    const int MAX_CARDS_ON_BOARD{5};
public:
    void addCard(const Card &card);

    void removeCard(int i);

    void removeCard(const Card &card);

    Card &getCard(int i) const;

    const vector<std::unique_ptr<Card>> &getCards() const;

    bool isFull() const;

    void printBoard() const;

private:
    vector<std::unique_ptr<Card>> m_cards;
};
