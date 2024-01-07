#pragma once

#include <raylib.h>
#include <Game.h>
#include <GameScreen.h>
#include <Common.h>

class Mouse {
public:
    Mouse(Game &game, GameScreen &gameScreen);

    void update();

private:
    void handleClick(const string &objectName);

    Game &m_game;
    GameScreen &m_gameScreen;
};
