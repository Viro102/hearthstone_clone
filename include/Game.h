#pragma once

#include <Player.h>
#include <Common.h>
#include <Card.h>
#include <raylib.h>

class Game {
public:
    Game(Player player1, Player player2);

    Game() = default;

    void startGame();

    void endTurn();

    void playACard(int i);

    void selectCardBoard(int i);

    void attack(int i);

    void attackFace();

    void checkGameOver() const;

    void initializeFromJson(const nlohmann::json &jsonState);

    void setSelectedCard(Card &card);

    [[nodiscard]] bool isSelected() const;

    [[nodiscard]] Player &getOnTurnPlayer() const;

    [[nodiscard]] Player &getOffTurnPlayer() const;

    [[nodiscard]] std::optional<std::reference_wrapper<Card>> getSelectedCard() const;

    array<std::unique_ptr<Player>, 2> &getPlayers();

    [[nodiscard]] Player &getPlayer(int id) const;

    void print() const;

private:
    void specialCard(Card &card);


    array<std::unique_ptr<Player>, 2> m_players;
    Card *m_selectedCard{};
    int m_turnCounter{0};
};
