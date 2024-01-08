#include <Mouse.h>

Mouse::Mouse(Game &game, GameScreen &gameScreen) : m_game(game), m_gameScreen(gameScreen) {}

void Mouse::update() {
    Vector2 mousePoint = GetMousePosition();

    for (const auto &[name, hitbox]: m_gameScreen.getClickableObjects()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePoint, hitbox)) {
            handleClick(name);
        }
    }
}

void Mouse::handleClick(const string &objectName) {
    if (objectName == "endTurnButton") {
        cout << "clicked on end turn" << endl;
        m_game.endTurn();
        m_gameScreen.removeGlow();
    } else if (objectName == "heroOpponent") {
        cout << "clicked on opponent" << endl;
        m_game.attackFace();
    } else if (objectName == "heroPlayer") {
        cout << "clicked on yourself" << endl;
    } else if (objectName.find("slotHand") != string::npos) {
        const auto &index = Common::extractNumbers(objectName);
        cout << "clicked on hand with index: " << index[0] << endl;
        m_game.playACard(index[0]);
    } else if (objectName.find("slotBoard[0]") != string::npos) {
        const auto &index = Common::extractNumbers(objectName);
        cout << "clicked on your board [" << index[1] << "]" << endl;
        m_game.selectCardBoard(index[1]);
        m_gameScreen.addGlow(index[1]);
    } else if (objectName.find("slotBoard[1]") != string::npos) {
        const auto &index = Common::extractNumbers(objectName);
        cout << "clicked on enemy board [" << index[1] << "]" << endl;
    }
}
