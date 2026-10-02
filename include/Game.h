#pragma once

#include <Player.h>
#include <Common.h>
#include <Card.h>

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

    bool checkGameOver() const;

    // Id of the player who won, or -1 while both heroes are alive.
    [[nodiscard]] int getWinnerId() const;

    // The game ends in a draw once MAX_TURNS turns have been played without a winner.
    [[nodiscard]] bool isDraw() const;

    void initializeFromJson(const nlohmann::json &jsonState);

    void setSelectedCard(Card &card);

    void deselectCard();

    [[nodiscard]] bool isSelected() const;

    [[nodiscard]] Player &getOnTurnPlayer() const;

    [[nodiscard]] Player &getOffTurnPlayer() const;

    [[nodiscard]] std::optional<Card> getSelectedCard() const;

    array<std::unique_ptr<Player>, 2> &getPlayers();

    [[nodiscard]] Player &getPlayer(int id) const;

    void print() const;

private:
    void specialCard(const Card &card);

    static bool hasTaunt(const Player &player);

    // Resolves the selection to the live card on the board (minion) or to the stored copy (spell).
    Card *resolveSelectedCard();


    static constexpr int MAX_TURNS = 30;

    array<std::unique_ptr<Player>, 2> m_players;
    // Spells are not on the board, so they are kept as a copy; minions are referenced by board index.
    std::optional<Card> m_selectedCard;
    int m_selectedBoardIndex{-1};
    int m_turnCounter{0};
};
