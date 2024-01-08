#pragma once

#include <Common.h>
#include <Game.h>
#include <map>
#include <Button.h>

class GameScreen {
public:
    explicit GameScreen(Game &gameplayState);

    void draw();

    void update();

private:
    void paintHero(int pos, const Player &player) const;

    void paintUI() const;

    static void paintCards(const vector<Card> &cards);


    const int END_TURN_BUTTON_POSITION_X = 910;
    const int END_TURN_BUTTON_POSITION_Y = 350;
    const int HEROES_POSITION_X = 900;
    const int FIRST_HERO_POSITION_Y = 460;
    const int SECOND_HERO_POSITION_Y = 20;
    Game &m_gameplayState;
    vector<Texture2D> m_images{};
    vector<Card> m_cardsHand{};
    vector<Card> m_cardsBoard{};
    array<Button, 5> m_slotsHand;
    array<array<Button, 5>, 2> m_slotsBoard;
    std::map<string, Button> m_buttons;
};
