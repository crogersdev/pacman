#include <iostream>
#include <raylib.h>

int main() {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 800;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    while(WindowShouldClose() == false) {
        BeginDrawing();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
