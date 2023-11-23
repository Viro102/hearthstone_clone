#include "Game.h"

int main() {
    InitWindow(800, 450, "basic window");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
        auto game = std::make_unique<Game>();
        game->startGame("mage", "warrior");
    }

    CloseWindow();

    return 0;
}