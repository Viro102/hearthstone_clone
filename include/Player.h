#pragma once

#include <random>
#include <memory>
#include "Common.h"
#include "Card.h"
#include "Deck.h"
#include <algorithm>

class Player {
private:
    const int MAX_MANA{10};

public:
    Player(int hp, int id, string archetype);

    std::unique_ptr<Card> drawCard();

    std::unique_ptr<Card> playCard(int i);

    void shuffleDeck();

    CardContainer &getBoard() const;

    Deck &getDeck() const;

    CardContainer & getHand() const;

    int getHp() const;

    int getId() const;

    int getMana() const;

    int getCurrentMaxMana() const;

    string getHpString() const;

    string getManaString() const;

    string getArchetype() const;

    void setHp(int hp);

    void setMana(int mana);

    bool isTurn() const;

    void setTurn(bool turn);

private:
    string m_archetype;
    int m_hp;
    int m_id;
    int m_mana{0};
    int m_currentMaxMana{0};
    bool m_turn{false};
    std::unique_ptr<CardContainer> m_hand{std::make_unique<CardContainer>()};
    std::unique_ptr<CardContainer> m_board{std::make_unique<CardContainer>()};
    std::unique_ptr<Deck> m_deck{std::make_unique<Deck>(R"(..\assets\cards.txt)")};
};
