#ifndef RAYLIB_TEST_HAND_H
#define RAYLIB_TEST_HAND_H

#include "Common.h"
#include "Card.h"

class Hand {
private:
    static const int MAX_CARDS = 5;
public:
    void addCard(const Card &card);

    void removeCard(int i);

    void removeCard(const Card &card);

    Card getCard(int i) const;

    const vector<Card> &getCards() const;

    int getNumOfCards() const;

    bool isFull() const;

    void printHand() const;

private:
    vector<Card> m_cards;

};

#endif //RAYLIB_TEST_HAND_H
