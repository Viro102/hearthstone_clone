#pragma once

#include <Common.h>
#include <Game.h>
#include <Slot.h>
#include <map>

class GameScreen {
public:
    explicit GameScreen(Game &game);

    ~GameScreen();

    void draw();

    void update();

    const std::map<string, Rectangle> &getClickableObjects() const;

    void addGlow(int i, const string &where);
    
    void removeGlow();

private:
    const int END_TURN_BUTTON_POSITION_X = 910;
    const int END_TURN_BUTTON_POSITION_Y = 350;
    const int HEROES_POSITION_X = 900;
    const int FIRST_HERO_POSITION_Y = 460;
    const int SECOND_HERO_POSITION_Y = 20;
    Game &m_game;
    vector<Texture2D> m_images{};
    Rectangle m_endTurnButtonHitbox{};
    Rectangle m_heroHitbox{};
    vector<Card> m_cardsHand{};
    vector<Card> m_cardsBoard{};
    array<Slot, 5> m_slotsHand;
    array<array<Slot, 5>, 2> m_slotsBoard;
    std::map<string, Rectangle> m_clickableObjects{};

    void registerAsClickable(const string &name, const Rectangle &hitbox);

    void paintHero(int pos, Texture2D hero, const Player &player) const;

    void paintUI() const;

    static void paintCards(const vector<Card> &cards);
};
