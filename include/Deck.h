#pragma once

#include <fstream>
#include <sstream>
#include <CardContainer.h>
#include <random>


class Deck : public CardContainer<30> {
public:
    Deck &makeDeck(const string &filename);

    void shuffleDeck();

};
