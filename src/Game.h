#ifndef RAYLIB_TEST_GAME_H
#define RAYLIB_TEST_GAME_H

#include "Player.h"
#include "Common.h"
#include "Card.h"

class Game {
public:
    Game();

    void startGame(string player1, string player2);

    void endTurn();

    void playACard(Player player, int i);

    void selectCardBoard(Player player, int i);

    void selectCardHand(Player player, int i);

    void attack(int i);

    void attackFace();

    Player getOnTurnPlayer() const;

    Player getOffTurnPlayer() const;

    array<Player, 2> getPlayers() const;

    bool isSelected() const;

    void isGameOver() const;

    void specialCard(Player player, Card card);

private:
    static const int PLAYER_MAX_HP = 20;
    array<Player, 2> players{};
    Panel panel;
    Card selectedCard;
    int turnCounter{0};
};


#endif //RAYLIB_TEST_GAME_H
