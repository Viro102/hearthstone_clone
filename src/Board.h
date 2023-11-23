#ifndef RAYLIB_TEST_BOARD_H
#define RAYLIB_TEST_BOARD_H

#include "Card.h"
#include <ranges>
#include <algorithm>

class Board {
private:
    static const int MAX_CARDS_ON_BOARD = 5;
public:
    void addCard(const Card &card);

    void removeCard(const Card &card);

    void removeCard(int i);

    Card getCard(int i);

    const vector<Card> &getCards() const;

    bool isFull() const;

    void printBoard() const;

private:
    vector<Card> m_cards;
};

#endif //RAYLIB_TEST_BOARD_H
