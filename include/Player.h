#pragma once

#include <random>
#include <memory>
#include <Common.h>
#include <Card.h>
#include <Deck.h>

class Player {
private:
    const int MAX_MANA{10};

public:
    Player(int id, string archetype);

    std::unique_ptr<Card> drawCard();

    std::unique_ptr<Card> playCard(int i);

    [[nodiscard]] CardContainer<5> &getBoard() const;

    [[nodiscard]] Deck &getDeck() const;

    [[nodiscard]] CardContainer<5> &getHand() const;

    [[nodiscard]] int getHp() const;

    [[nodiscard]] int getId() const;

    [[nodiscard]] int getMana() const;

    [[nodiscard]] int getCurrentMaxMana() const;

    [[nodiscard]] string getHpString() const;

    [[nodiscard]] string getManaString() const;

    [[nodiscard]] string getArchetype() const;

    [[nodiscard]] bool isTurn() const;

    void setHp(int hp);

    void setMana(int mana);

    void setDeck(std::unique_ptr<Deck> deck);

    void setHand(std::unique_ptr<CardContainer<5>> hand);

    void setBoard(std::unique_ptr<CardContainer<5>> board);

    void setTurn(bool turn);

private:
    string m_archetype;
    int m_hp{20};
    int m_id{-1};
    int m_mana{0};
    int m_currentMaxMana{0};
    bool m_turn{false};
    std::unique_ptr<CardContainer<5>> m_hand{std::make_unique<CardContainer<5>>()};
    std::unique_ptr<CardContainer<5>> m_board{std::make_unique<CardContainer<5>>()};
    std::unique_ptr<Deck> m_deck{std::make_unique<Deck>()};
};
