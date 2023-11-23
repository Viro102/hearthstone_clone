#ifndef RAYLIB_TEST_DECK_H
#define RAYLIB_TEST_DECK_H

#include <fstream>
#include <sstream>
#include "Common.h"
#include "Card.h"

class Deck {
public:
    Deck(const string &filename);

    void addCard(const Card &card);

    void removeCard(const Card &card);

    vector<Card> getCards() const;

    Card getCard(int i) const;

    int getNumOfCards() const;

    string getNumOfCardsString() const;

    Deck &makeDeck(const std::string &filename);

private:
    int m_numberOfCards{0};
    vector<Card> m_cards{};

};

#endif //RAYLIB_TEST_DECK_H
