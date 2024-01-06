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
    Player(int hp, int id, string archetype);

    std::unique_ptr<Card> drawCard();

    std::unique_ptr<Card> playCard(int i);

    void shuffleDeck();

    [[nodiscard]] CardContainer &getBoard() const;

    [[nodiscard]] Deck &getDeck() const;

    [[nodiscard]] CardContainer &getHand() const;

    [[nodiscard]] int getHp() const;

    [[nodiscard]] int getId() const;

    [[nodiscard]] int getMana() const;

    [[nodiscard]] int getCurrentMaxMana() const;

    [[nodiscard]] string getHpString() const;

    [[nodiscard]] string getManaString() const;

    [[nodiscard]] string getArchetype() const;

    void setHp(int hp);

    void setMana(int mana);

    [[nodiscard]] bool isTurn() const;

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
    std::unique_ptr<Deck> m_deck{std::make_unique<Deck>("../assets/cards.txt")};
};
