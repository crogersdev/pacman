#include <iostream>

#include <raylib.h>

#include "Labyrinth.hpp"
#include "Pacman.hpp"
#include "helpers/CollisionDetection.hpp"

int main() {
    const int SCREEN_WIDTH = 23*25;
    const int SCREEN_HEIGHT = 800;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    Labyrinth labyrinth = Labyrinth();
    Pacman pacman = Pacman();

    while (WindowShouldClose() == false) {
        ClearBackground(BLACK);
        BeginDrawing();
        labyrinth.draw();

        // Handle inputs
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  pacman.move({0, 1.}, labyrinth);
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    pacman.move({0, -1.}, labyrinth);
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) pacman.move({1., 0}, labyrinth);
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  pacman.move({-1., 0}, labyrinth);

        pacman.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
