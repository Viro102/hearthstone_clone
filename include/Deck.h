#pragma once

#include <fstream>
#include <sstream>
#include "Common.h"
#include "Card.h"

class Deck {
public:
    explicit Deck(const string &filename);

    void addCard(const Card &card);

    void removeCard(const Card &card);

    vector<Card> &getCards();

    Card &getCard(int i);

    int getNumOfCards() const;

    string getNumOfCardsString() const;

    Deck &makeDeck(const std::string &filename);

private:
    int m_numberOfCards{0};
    vector<Card> m_cards{};

};
