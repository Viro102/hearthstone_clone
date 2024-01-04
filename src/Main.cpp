#include "../include/Game.h"

enum class GameState {
    MENU,
    LOBBY,
    GAMEPLAY,
    END
};

int main() {
    auto game = std::make_unique<Game>();
    game->startGame("mage", "warrior");

    // Initialization
    const int screenWidth = 1200;
    const int screenHeight = 800;
    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;

    InitWindow(screenWidth, screenHeight, "Hearthstone");

    GameState gameState = GameState::MENU;

    // Define the buttons
    Rectangle lobbyBtn = {screenCenterX - 100, 200, 200, 50};
    Rectangle exitBtn = {screenCenterX - 100, 300, 200, 50};
    Color lobbyBtnColor = GRAY;
    Color exitBtnColor = GRAY;
    Color hoverColor = DARKGRAY;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        switch (gameState) {
            using enum GameState;
            case MENU:
                // Update menu logic
                // Check for button clicks and react
                if (CheckCollisionPointRec(GetMousePosition(), lobbyBtn)) {
                    lobbyBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        gameState = GAMEPLAY;
                    }
                    // Enter lobby logic here
                } else {
                    lobbyBtnColor = GRAY;
                }
                if (CheckCollisionPointRec(GetMousePosition(), exitBtn)) {
                    exitBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        gameState = END;
                    }
                    // Exit game logic here
                } else {
                    exitBtnColor = GRAY;
                }
                break;
            case LOBBY:
                // TODO
                break;
            case GAMEPLAY:
                // Update game logic
                if (IsKeyPressed(KEY_ESCAPE)) {
                    gameState = END;
                }
                break;
            case END:
                // Cleanup and close
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
                // TODO
                break;
            case GAMEPLAY:
                // Draw your game here
                DrawText("Gameplay! Press ESC to exit", screenCenterX, screenCenterY, 20, LIGHTGRAY);
                break;
            case END:
                cout << "INFO: Ending the game..\n";
                break;
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}