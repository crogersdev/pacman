#include <iostream>

#include <raylib.h>

#include "Labyrinth.hpp"

int main() {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 800;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    Labyrinth labyrinth = Labyrinth();

    while(WindowShouldClose() == false) {
        BeginDrawing();
        labyrinth.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
