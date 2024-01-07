#pragma once

#include <Player.h>
#include <Common.h>
#include <Card.h>
#include <raylib.h>

class Game {
public:
    void startGame(const string &player1, const string &player2);

    void endTurn();

    void playACard(int i);

    void selectCardBoard(int i);

    void attack(int i);

    void attackFace();

    [[nodiscard]] Player &getOnTurnPlayer() const;

    [[nodiscard]] Player &getOffTurnPlayer() const;

    array<std::unique_ptr<Player>, 2> &getPlayers();

    [[nodiscard]] bool isSelected() const;

    void isGameOver() const;

    void specialCard(const Card &card);


private:
    const int PLAYER_MAX_HP{20};
    array<std::unique_ptr<Player>, 2> m_players{};
    std::unique_ptr<Card> m_selectedCard{};
    int m_turnCounter{0};
};
