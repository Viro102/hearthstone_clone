#ifndef RAYLIB_TEST_PLAYER_H
#define RAYLIB_TEST_PLAYER_H

#include "Common.h"
#include "Card.h"
#include "Board.h"
#include "Deck.h"
#include "Hand.h"

class Player {
private:
    static const int MAX_MANA = 10;

public:
    Player(int hp, int id, string archetype);

    Card drawCard();

    Card playCard(int i);

    Board getBoard() const;

    Deck getDeck() const;

    Hand getHand() const;

    int getHp() const;

    int getId() const;

    string getHpString() const;

    int getMana() const;

    string getManaString() const;

    int getCurrentMaxMana() const;

    string getArchetype() const;

    void setHp(int hp);

    void setMana(int mana);

    bool isTurn() const;

    void setTurn(bool turn);

private:
    string m_archetype;
    int m_hp;
    int m_id;
    int m_mana;
    int m_currentMaxMana;
    bool m_turn;
    Hand m_hand;
    Board m_board;
    Deck m_deck;
    Random m_random;

};


#endif //RAYLIB_TEST_PLAYER_H
