#include <Game.h>
#include <Panel.h>
#include <Client.h>

enum class GameState {
    MENU,
    LOBBY,
    GAMEPLAY,
    END
};

int main() {
    // Initialization
    const int screenWidth = 1400;
    const int screenHeight = 750;
    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;

    InitWindow(screenWidth, screenHeight, "Hearthstone");

    Game game;
    Panel panel(game);
    Client client;

    GameState gameState = GameState::MENU;

    // Define the buttons
    Rectangle lobbyBtn = {screenCenterX - 100, 200, 200, 50};
    Rectangle exitBtn = {screenCenterX - 100, 300, 200, 50};
    Rectangle readyBtn = {screenCenterX - 150, 350, 100, 50};
    Rectangle startBtn = {screenCenterX - 50, 350, 100, 50};
    Rectangle exitBtnLobby = {screenCenterX + 50, 350, 100, 50};

    auto lobbyBtnColor = GRAY;
    auto exitBtnColor = GRAY;
    auto readyBtnColor = GRAY;
    auto startBtnColor = GRAY;
    auto exitBtnLobbyColor = GRAY;
    auto hoverColor = DARKGRAY;

    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    game.startGame("mage", "warrior");


    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsKeyPressed(KEY_ESCAPE)) {
            gameState = GameState::MENU;
        }

        switch (gameState) {
            case GameState::MENU:
                // Update menu logic
                if (CheckCollisionPointRec(GetMousePosition(), lobbyBtn)) {
                    lobbyBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        client.startClient(8080);
                        if (client.getSocket() != -1) {
                            gameState = GameState::LOBBY;
                        }
                    }
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
                if (CheckCollisionPointRec(GetMousePosition(), readyBtn)) {
                    readyBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        string readyMsg = "ready";
                        send(client.getSocket(), readyMsg.c_str(), readyMsg.length(), 0);
                    }
                } else {
                    readyBtnColor = GRAY;
                }
                if (CheckCollisionPointRec(GetMousePosition(), exitBtnLobby)) {
                    exitBtnLobbyColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        gameState = GameState::MENU;
                    }
                } else {
                    exitBtnLobbyColor = GRAY;
                }
                if (CheckCollisionPointRec(GetMousePosition(), startBtn)) {
                    startBtnColor = hoverColor;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        gameState = GameState::GAMEPLAY;
                    }
                } else {
                    startBtnColor = GRAY;
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
            case GameState::MENU:
                // Draw buttons
                DrawRectangleRec(lobbyBtn, lobbyBtnColor);
                DrawRectangleRec(exitBtn, exitBtnColor);

                // Draw button text
                DrawText("Lobby", lobbyBtn.x + 70, lobbyBtn.y + 15, 20, BLACK);
                DrawText("Exit", exitBtn.x + 80, exitBtn.y + 15, 20, BLACK);
                break;
            case GameState::LOBBY:
                DrawText("Game Lobby", screenWidth / 2 - MeasureText("Game Lobby", 20) / 2, 20, 20, BLACK);

                // Draw player states
                for (size_t i = 0; i < client.getLobbyState().players.size(); ++i) {
                    const auto &player = client.getLobbyState().players[i];
                    DrawText(TextFormat("Player %zu: %s", i + 1, player.isReady ? "Ready" : "Not Ready"), 100,
                             150 + 50 * i, 20,
                             BLACK);
                }

                // Draw buttons
                DrawRectangleRec(readyBtn, readyBtnColor);
                DrawRectangleRec(startBtn, startBtnColor);
                DrawRectangleRec(exitBtnLobby, exitBtnLobbyColor);
                DrawText("Ready", readyBtn.x + 20, readyBtn.y + 15, 20, BLACK);
                DrawText("Start", startBtn.x + 20, startBtn.y + 15, 20, BLACK);
                DrawText("Exit", exitBtnLobby.x + 20, exitBtnLobby.y + 15, 20, BLACK);
                break;
            case GameState::GAMEPLAY:
                panel.draw();
                break;
            case GameState::END:
                cout << "INFO: Ending the game..." << endl;
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
