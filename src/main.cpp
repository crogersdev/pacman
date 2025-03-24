#include <iostream>

#include <raylib.h>

#include "Labyrinth.hpp"
#include "Pacman.hpp"

int main() {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 800;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    Labyrinth labyrinth = Labyrinth();
    Pacman pacman = Pacman();

    while(WindowShouldClose() == false) {
        pacman.move();

        ClearBackground(BLACK);
        BeginDrawing();
        labyrinth.draw();
        pacman.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
