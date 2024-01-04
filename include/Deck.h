#pragma once

#include <fstream>
#include <sstream>
#include "CardContainer.h"

class Deck : public CardContainer {
public:
    explicit Deck(const string &filename);

    Deck() = default;

    ~Deck() override = default;

    Deck &makeDeck(const std::string &filename);

    int getMaxCards() const override;
};
