#include "Mouse.h"

Mouse::Mouse(Game &game, GameScreen &gameScreen) : m_game(game), m_gameScreen(gameScreen) {}

void Mouse::update() {
    Vector2 mousePoint = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        cout << mousePoint.x << " " << mousePoint.y << endl;
    }

    for (const auto &[name, hitbox]: m_gameScreen.getClickableObjects()) {
        if (CheckCollisionPointRec(mousePoint, hitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            handleClick(name);
        }
    }
}

void Mouse::handleClick(const string &objectName) {
    if (objectName == "endTurnButton") {
        m_game.endTurn();
    }
    if (objectName == "heroAvatar") {
        m_game.attackFace();
    }
}

