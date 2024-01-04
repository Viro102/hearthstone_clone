#pragma once

#include "Player.h"
#include "Common.h"
#include "Card.h"

class Game {
public:
    void startGame(const string &player1, const string &player2);

    void endTurn();

    void playACard(Player &player, int i);

    void selectCardBoard(Player &player, int i);

    void selectCardHand(const Player &player, int i);

    void attack(int i);

    void attackFace();

    Player &getOnTurnPlayer() const;

    Player &getOffTurnPlayer() const;

    array<std::unique_ptr<Player>, 2> &getPlayers();

    bool isSelected() const;

    void isGameOver() const;

    void specialCard(Player &player, const Card &card);

private:
    const int PLAYER_MAX_HP{20};
    array<std::unique_ptr<Player>, 2> m_players{};
    std::unique_ptr<Card> m_selectedCard;
    int m_turnCounter{0};
};