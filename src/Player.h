#ifndef RAYLIB_TEST_PLAYER_H
#define RAYLIB_TEST_PLAYER_H

#include <random>
#include <memory>
#include "Common.h"
#include "Card.h"
#include "Board.h"
#include "Deck.h"
#include "Hand.h"

class Player {
private:
    static const int MAX_MANA = 10;

public:
    Player(int hp, int id, const string &archetype);

    std::unique_ptr<Card> drawCard();

    std::unique_ptr<Card> playCard(int i);

    void shuffleDeck();

    Board &getBoard();

    Deck &getDeck() const;

    Hand &getHand() const;

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
    int m_mana{0};
    int m_currentMaxMana{0};
    bool m_turn{false};
    std::unique_ptr<Hand> m_hand{std::make_unique<Hand>()};
    std::unique_ptr<Board> m_board{std::make_unique<Board>()};
    std::unique_ptr<Deck> m_deck{std::make_unique<Deck>(R"(..\src\res\cards.txt)")};
};


#endif //RAYLIB_TEST_PLAYER_H
