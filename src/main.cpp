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
    Ghost blinky = Ghost(
        "res/blinky.png", 
        { 8 * TILE_SIZE - TILE_SIZE / 2, 13 * TILE_SIZE - TILE_SIZE / 2 }
    );

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

        // std::cout << "{ " << intendedDirection.x << ", " << intendedDirection.y << " }\n";
        int currentTileX = static_cast<int>(pacman.mPosition.x / TILE_SIZE);
        int currentTileY = static_cast<int>(pacman.mPosition.y / TILE_SIZE);

        Vector2 intendedPosition = {
            (currentTileX + intendedDirection.x) * TILE_SIZE + TILE_SIZE / 2,
            (currentTileY + intendedDirection.y) * TILE_SIZE + TILE_SIZE / 2
        };

        // std::cout << "pacman's position: (" << pacman.mPosition.y << "," << pacman.mPosition.x << ")\n";
        // std::cout << "pacman's intended position: (" << intendedPosition.y << ", " << intendedPosition.x << ")\n";

        //std::cout << "Current tile: (" << currentTileX << "," << currentTileY << ")\n";
        //std::cout << "Intended tile: (" << static_cast<int>(intendedPosition.x/TILE_SIZE) 
        //          << "," << static_cast<int>(intendedPosition.y/TILE_SIZE) << ")\n";
        //std::cout << "Is centered: " << (pacman.isCentered() ? "yes" : "no") << "\n";

        if (pacman.isCentered()) {
            if (intendedDirection.x != 0.f || intendedDirection.y != 0.f) {
                if (labyrinth.isLegalMove(intendedPosition)) {
                    pacman.mDirection = intendedDirection;
                } else {
                    std::cout << "move not legal\n";
                }
            } else {
                pacman.mDirection = { 0.f, 0.f };
            }

           Vector2 intendedDirectionFromMomentum = {
            (currentTileX + pacman.mDirection.x) * TILE_SIZE + TILE_SIZE / 2,
            (currentTileY + pacman.mDirection.y) * TILE_SIZE + TILE_SIZE / 2,
           };

           if (!labyrinth.isLegalMove(intendedDirectionFromMomentum)) {
            pacman.mDirection = { 0.f, 0.f };
           }
        }

        if (pacman.mDirection.x != 0.f || pacman.mDirection.y != 0.f) {
            pacman.move();
        }

        blinky.act(labyrinth);
        blinky.draw();
        pacman.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
