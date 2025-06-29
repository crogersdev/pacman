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

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(BLACK);
        labyrinth.draw();

        Vector2 intendedDirection = { 0.f, 0.f };

        // Handle inputs
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { 
            intendedDirection = { intendedDirection.x, intendedDirection.y + 1.f };
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            intendedDirection = { intendedDirection.x, intendedDirection.y - 1.f };
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            intendedDirection = { intendedDirection.x + 1.f, intendedDirection.y };
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            intendedDirection = { intendedDirection.x - 1.f, intendedDirection.y };
        }

        int currentCol = static_cast<int>(pacman.mPosition.x / TILE_SIZE);
        int currentRow = static_cast<int>(pacman.mPosition.y / TILE_SIZE);

        int intendedCol = static_cast<int>(currentCol + intendedDirection.x);
        int intendedRow = static_cast<int>(currentRow + intendedDirection.y);

        // Rectangle intendedTileRect = {
        //     static_cast<float>(intendedCol * TILE_SIZE),
        //     static_cast<float>(intendedRow * TILE_SIZE),
        //     TILE_SIZE,
        //     TILE_SIZE
        // };

        // DrawRectangleLines(intendedCol * TILE_SIZE, intendedRow * TILE_SIZE, 26, 26, RED);

        Vector2 intendedPosition = {
            pacman.mPosition.x + (intendedDirection.x * pacman.mSpeed * GetFrameTime()),
            pacman.mPosition.y + (intendedDirection.y * pacman.mSpeed * GetFrameTime())
        };

        Vector2 intendedTile = {
            intendedPosition.x - (TILE_SIZE / 2.f),
            intendedPosition.y - (TILE_SIZE / 2.f)
        };

        // DrawCircleLines(intendedPosition.x, intendedPosition.y, pacman.mRadius, ORANGE);

        if (pacman.isCentered()) {
            if (labyrinth.isLegalMove(intendedTile)) {
                pacman.move(intendedDirection, intendedPosition);
            }
        } else {
            Vector2 newPosition = {
                pacman.mPosition.x + (pacman.mDirection.x * pacman.mSpeed * GetFrameTime()),
                pacman.mPosition.y + (pacman.mDirection.y * pacman.mSpeed * GetFrameTime())
            };
            pacman.move(pacman.mDirection, newPosition);
        }
        

        blinky.act(labyrinth);
        blinky.draw();
        pacman.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
