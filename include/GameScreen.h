#pragma once

#include <Common.h>
#include <Game.h>
#include <map>
#include <Button.h>
#include <Client.h>

class GameScreen {
public:
    explicit GameScreen(Client &client);

    void draw();

    void update();

    void print();

private:
    void paintUI();

    void paintHero(const Player *player, int posYHero, int offsetYHP, int offsetYMana, const string &buttonKey);

    static void paintCards(const vector<Card> &cards);

    
    const int END_TURN_BUTTON_POSITION_X = 910;
    const int END_TURN_BUTTON_POSITION_Y = 350;
    const int HEROES_POSITION_X = 900;
    const int FIRST_HERO_POSITION_Y = 460;
    const int SECOND_HERO_POSITION_Y = 20;
    Client &m_client;
    vector<Texture2D> m_images{};
    vector<Card> m_cardsHand{};
    vector<Card> m_cardsBoard{};
    array<Button, 5> m_slotsHand;
    array<array<Button, 5>, 2> m_slotsBoard;
    std::map<string, Button> m_buttons;
};
