#include <iostream>

#include <raylib.h>

#include "Ghost.hpp"
#include "Labyrinth.hpp"
#include "Pacman.hpp"
#include "helpers/CollisionDetection.hpp"
#include "helpers/Movement.hpp"

int main() {
    const int SCREEN_WIDTH = LABYRINTH_COLS * static_cast<int>(TILE_SIZE);
    const int SCREEN_HEIGHT = 800;

    // std::cout << "Current working directory: " << GetWorkingDirectory() << "\n";

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    Labyrinth labyrinth = Labyrinth();
    Pacman pacman = Pacman();
    Ghost blinky = Ghost("res/blinky.png", Vector2{ 8, 13 }); 
    blinky.mState = Ghost::State::CHASE;
    Ghost pinky = Ghost("res/pinky.png", Vector2{ 8, 12 });
    pinky.mState = Ghost::State::MEANDER;

    Vector2 intendedDirection = { 0.f, 0.f };

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(BLACK);
        labyrinth.draw();

        intendedDirection = { 0.f, 0.f };
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            intendedDirection = { 0.f, 1.f };
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            intendedDirection = { 0.f, -1.f };
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            intendedDirection = { 1.f, 0.f };
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            intendedDirection = { -1.f, 0.f };
        }

        // std::cout << "pacman's position: (" << pacman.mPosition.y << "," << pacman.mPosition.x << ")\n";
        // std::cout << "pacman's intended position: (" << intendedPosition.y << ", " << intendedPosition.x << ")\n";

        //std::cout << "Current tile: (" << currentTileX << "," << currentTileY << ")\n";
        //std::cout << "Intended tile: (" << static_cast<int>(intendedPosition.x/TILE_SIZE) 
        //          << "," << static_cast<int>(intendedPosition.y/TILE_SIZE) << ")\n";
        //std::cout << "Is centered: " << (pacman.isCentered() ? "yes" : "no") << "\n";

        pacman.move(intendedDirection, labyrinth);


        blinky.act(labyrinth, pacman.mPosition);
        blinky.draw();
        pinky.act(labyrinth, pacman.mPosition);
        pinky.draw();
        pacman.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
