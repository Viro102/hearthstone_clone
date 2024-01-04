#include "../include/Game.h"
#include "../include/Panel.h"

int main() {
    // Initialization
    const int screenWidth = 1400;
    const int screenHeight = 750;
    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;

    InitWindow(screenWidth, screenHeight, "Hearthstone");

    Game game;
    Panel panel(game);

    GameState gameState = GameState::MENU;

    // Define the buttons
    Rectangle lobbyBtn = {screenCenterX - 100, 200, 200, 50};
    Rectangle exitBtn = {screenCenterX - 100, 300, 200, 50};
    Rectangle gameBtn = {screenCenterX - 100, 300, 200, 50};
    auto lobbyBtnColor = GRAY;
    auto exitBtnColor = GRAY;
    auto gameBtnColor = GRAY;
    auto hoverColor = DARKGRAY;

    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    game.startGame("mage","warrior");

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsKeyPressed(KEY_ESCAPE)) {
            gameState = GameState::MENU;
        }

        switch (gameState) {
            case GameState::MENU:
                // Update menu logic
                // Check for button clicks and react
                if (CheckCollisionPointRec(GetMousePosition(), lobbyBtn)) {
                    lobbyBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        gameState = GameState::LOBBY;
                    }
                    // Enter lobby logic here
                } else {
                    lobbyBtnColor = GRAY;
                }
                if (CheckCollisionPointRec(GetMousePosition(), exitBtn)) {
                    exitBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        gameState = GameState::END;
                    }
                } else {
                    exitBtnColor = GRAY;
                }
                break;
            case GameState::LOBBY:
                if (CheckCollisionPointRec(GetMousePosition(), gameBtn)) {
                    gameBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        gameState = GameState::GAMEPLAY;
                    }
                    // Enter game logic here
                } else {
                    gameBtnColor = GRAY;
                }
                break;
            case GameState::GAMEPLAY:
                panel.update();
                break;
            case GameState::END:
                CloseWindow();
                return 0;
        }


        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        switch (gameState) {
            using enum GameState;
            case MENU:
                // Draw buttons
                DrawRectangleRec(lobbyBtn, lobbyBtnColor);
                DrawRectangleRec(exitBtn, exitBtnColor);

                // Draw button text
                DrawText("Lobby", lobbyBtn.x + 70, lobbyBtn.y + 15, 20, BLACK);
                DrawText("Exit", exitBtn.x + 80, exitBtn.y + 15, 20, BLACK);
                break;
            case LOBBY:
                DrawRectangleRec(gameBtn, gameBtnColor);
                DrawText("Game", gameBtn.x + 80, gameBtn.y + 15, 20, BLACK);
                break;
            case GAMEPLAY:
                panel.draw();
                break;
            case END:
                cout << "INFO: Ending the game..." << endl;
                break;
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}