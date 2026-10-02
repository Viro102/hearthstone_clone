#include <GameScreen.h>
#include <Client.h>
#include <Button.h>
#include <chrono>

int main() {
    // Initialization
    const int screenWidth = 1400;
    const int screenHeight = 750;
    const int screenCenterX = screenWidth / 2;
    const int screenCenterY = screenHeight / 2;

    using Clock = std::chrono::high_resolution_clock;
    auto lastPrintTime = Clock::now();
    std::chrono::seconds printInterval(3); // Print every 3 seconds

    string choice;
    string ipAddress;
    cout << "Which server to connect to: " << endl;
    cout << "1. frios2" << endl;
    cout << "2. localhost" << endl;
    std::getline(std::cin, choice);
    cout << "You entered: " << choice << endl;

    if (choice == "1") {
        ipAddress = "158.193.128.160";
    } else if (choice == "2") {
        ipAddress = "127.0.0.1";
    } else {
        cout << "error" << endl;
        return -1;
    }

    InitWindow(screenWidth, screenHeight, "Hearthstone");

    Client client;
    GameState gameState = GameState::MENU;
    std::unique_ptr<GameScreen> gameScreen{};

    // Menu buttons
    Button lobbyBtn({screenCenterX - 100, 200, 200, 50}, "Lobby");
    Button exitBtn({screenCenterX - 100, 300, 200, 50}, "Exit");

    vector<Button> buttonsMenu{lobbyBtn, exitBtn};

    // Lobby buttons
    Button readyBtn({screenCenterX - 150, 350, 100, 50}, "Ready");
    Button startBtn({screenCenterX - 50, 350, 100, 50}, "Start");
    Button exitBtnLobby({screenCenterX + 50, 350, 100, 50}, "Exit");

    vector<Button> buttonsLobby{readyBtn, startBtn, exitBtnLobby};

    // End game screen buttons
    Button exitBtnEnd(Rectangle(screenCenterX - 110, 400 - 25, 220, 50), "Back to Main Menu");

    SetTargetFPS(60);
    SetExitKey(KEY_NULL);


    // Main game loop
    while (!WindowShouldClose() && gameState != GameState::END) {
        // Connecting and disconnecting join the listener thread, so they happen outside the state lock.
        bool connect = false;
        bool disconnect = false;
        {
            auto stateLock = client.lockState();

            if (auto newState = client.takePendingState(); newState.has_value()) {
                gameState = *newState;
            }

            // Update
            if (IsKeyPressed(KEY_ESCAPE) && gameState != GameState::MENU) {
                gameState = GameState::MENU;
                disconnect = true;
            }

            switch (gameState) {
                case GameState::MENU:
                    // Update menu logic
                    if (exitBtn.isClicked()) {
                        gameState = GameState::END;
                    }
                    if (lobbyBtn.isClicked()) {
                        connect = true;
                    }
                    break;
                case GameState::LOBBY:
                    // Update lobby logic
                    if (readyBtn.isClicked()) {
                        client.sendMessage("ready");
                    }
                    if (exitBtnLobby.isClicked()) {
                        gameState = GameState::MENU;
                        disconnect = true;
                    }
                    // The server switches everyone to GAMEPLAY with a "startGame" message
                    if (startBtn.isClicked() && client.getLobbyState().canStart()) {
                        client.sendMessage("startGame");
                    }
                    break;
                case GameState::WIN:
                case GameState::LOSE:
                    if (exitBtnEnd.isClicked()) {
                        gameState = GameState::MENU;
                        disconnect = true;
                    }
                    break;

                case GameState::GAMEPLAY:
                    if (client.isGameStateInitialized() && !gameScreen) {
                        gameScreen = std::make_unique<GameScreen>(client);
                    }

                    if (gameScreen && client.isGameStateInitialized()) {
                        gameScreen->update();

                        // Check if 3 seconds have passed
                        auto currentTime = Clock::now();
                        if (currentTime - lastPrintTime >= printInterval) {
                            cout << endl << endl;
                            gameScreen->print();
                            cout << endl << endl;

                            // Reset the timer
                            lastPrintTime = currentTime;
                        }
                    }
                    break;
                case GameState::END:
                    break;
            }

            // A new game builds a new screen, so the heroes match the new players
            if (gameState != GameState::GAMEPLAY) {
                gameScreen.reset();
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
                    if (!client.getNotice().empty()) {
                        const char *notice = client.getNotice().c_str();
                        DrawText(notice, screenCenterX - MeasureText(notice, 20) / 2, 140, 20, RED);
                    }
                    break;
                case GameState::LOBBY:
                    DrawText("Game Lobby", screenCenterX - MeasureText("Game Lobby", 20) / 2, 20, 20, BLACK);

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

                case GameState::WIN:
                    DrawText("Congratulations, You Won!",
                             screenCenterX - MeasureText("Congratulations, You Won!", 40) / 2,
                             screenCenterY - 45, 40, RED);
                    exitBtnEnd.draw();
                    break;

                case GameState::LOSE:
                    DrawText("Oh no, You Lost!",
                             screenCenterX - MeasureText("Oh no, You Lost!", 40) / 2,
                             screenCenterY - 45, 40, RED);
                    exitBtnEnd.draw();
                    break;
                case GameState::GAMEPLAY:
                    if (gameScreen && client.isGameStateInitialized()) {
                        gameScreen->draw();
                    }
                    break;
                case GameState::END:
                    cout << "INFO: Ending the game..." << endl;
                    break;
            }
        }

        // EndDrawing waits for the next frame, so the listener thread gets the state lock during that time.
        EndDrawing();

        if (disconnect) {
            client.shutdown();
        }
        if (connect) {
            if (client.start(10322, ipAddress) != -1) {
                gameState = GameState::LOBBY;
            }
        }
    }

    // Textures must be released while the window still exists
    gameScreen.reset();
    client.shutdown();
    CloseWindow();
    return 0;
}
