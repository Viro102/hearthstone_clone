#include <Game.h>
#include <GameScreen.h>
#include <Client.h>
#include <Button.h>

int main() {
    // Initialization
    const int screenWidth = 1400;
    const int screenHeight = 750;
    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;

    InitWindow(screenWidth, screenHeight, "Hearthstone");

    Client client;
    GameState gameState = GameState::MENU;
    std::unique_ptr<GameScreen> gameScreen{};

    client.setStateChangeCallback([&gameState](GameState newState) {
        gameState = newState;
    });

    bool hasInit = false;

    // Menu buttons
    Button lobbyBtn({screenCenterX - 100, 200, 200, 50}, "Lobby");
    Button exitBtn({screenCenterX - 100, 300, 200, 50}, "Exit");

    lobbyBtn.setOnClick([&client, &gameState] {
        if (client.start(10322) != -1) {
            gameState = GameState::LOBBY;
        }
        // single player
        // gameState = GameState::LOBBY;
    });

    exitBtn.setOnClick([&gameState] {
        gameState = GameState::END;
    });

    vector<Button> buttonsMenu{lobbyBtn, exitBtn};

    // Lobby buttons
    Button readyBtn({screenCenterX - 150, 350, 100, 50}, "Ready");
    Button startBtn({screenCenterX - 50, 350, 100, 50}, "Start");
    Button exitBtnLobby({screenCenterX + 50, 350, 100, 50}, "Exit");

    readyBtn.setOnClick([&client] {
        client.sendMessage("ready");
    });

    exitBtnLobby.setOnClick([&client, &gameState] {
        gameState = GameState::MENU;
        client.shutdown();
    });

    startBtn.setOnClick([&client, &gameState] {
        if (client.getLobbyState().canStart()) {
            client.sendMessage("startGame");
            gameState = GameState::GAMEPLAY;
        }
        // single player
        // gameState = GameState::GAMEPLAY;
    });

    vector<Button> buttonsLobby{readyBtn, startBtn, exitBtnLobby};


    SetTargetFPS(60);
    SetExitKey(KEY_NULL);


    // Main game loop
    while (!WindowShouldClose()) {
        // Update
        if (IsKeyPressed(KEY_ESCAPE)) {
            gameState = GameState::MENU;
        }

        switch (gameState) {
            case GameState::MENU:
                // Update menu logic
                for (const auto &button: buttonsMenu) {
                    button.update();
                }
                break;
            case GameState::LOBBY:
                // Update lobby logic
                for (const auto &button: buttonsLobby) {
                    button.update();
                }
                break;
            case GameState::GAMEPLAY:
                if (client.isGameStateInitialized() && !hasInit) {
                    gameScreen = std::make_unique<GameScreen>(client.getGameplayState());
                    hasInit = true;
                } else if (client.isGameStateInitialized()) {
                    gameScreen->update();
                }
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
                for (const auto &button: buttonsMenu) {
                    button.draw();
                }
                break;
            case GameState::LOBBY:
                DrawText("Game Lobby", screenWidth / 2 - MeasureText("Game Lobby", 20) / 2, 20, 20, BLACK);

                // Draw player states
                for (int i = 0; i < client.getLobbyState().players.size(); ++i) {
                    const auto &player = client.getLobbyState().players[i];
                    DrawText(TextFormat("Player %d: %s", i + 1, player.isReady ? "Ready" : "Not Ready"), 100,
                             150 + 50 * i, 20, BLACK);
                }

                // Draw buttons
                for (const auto &button: buttonsLobby) {
                    button.draw();
                }
                break;
            case GameState::GAMEPLAY:
                if (hasInit && client.isGameStateInitialized()) {
                    gameScreen->draw();
                }
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
