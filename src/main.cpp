#include <iostream>

#include <raylib.h>

#include "Labyrinth.hpp"
#include "Pacman.hpp"
#include "helpers/CollisionDetection.hpp"

int main() {
    const int SCREEN_WIDTH = LABYRINTH_COLS * static_cast<int>(TILE_SIZE);
    const int SCREEN_HEIGHT = 800;

    // std::cout << "Current working directory: " << GetWorkingDirectory() << "\n";

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    Labyrinth labyrinth = Labyrinth();
    Pacman pacman = Pacman();

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(BLACK);
        labyrinth.draw();

        Vector2 intendedDirection = { 0., 0. };

        // Handle inputs
        // todo: how do we allow two directions at the same time if we end up
        //       overwriting intendedDirection every time?
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) { 
            intendedDirection = labyrinth.directionLut[static_cast<int>(Labyrinth::Direction::DOWN)];
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            intendedDirection = labyrinth.directionLut[static_cast<int>(Labyrinth::Direction::UP)];
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            intendedDirection = labyrinth.directionLut[static_cast<int>(Labyrinth::Direction::RIGHT)];
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            intendedDirection = labyrinth.directionLut[static_cast<int>(Labyrinth::Direction::LEFT)];
        }

        int currentCol = static_cast<int>(pacman.mPosition.x / TILE_SIZE);
        int currentRow = static_cast<int>(pacman.mPosition.y / TILE_SIZE);

        int intendedCol = static_cast<int>(currentCol + intendedDirection.x);
        int intendedRow = static_cast<int>(currentRow + intendedDirection.y);

        Rectangle intendedTile = {
            static_cast<float>(intendedCol * TILE_SIZE),
            static_cast<float>(intendedRow * TILE_SIZE),
            TILE_SIZE,
            TILE_SIZE
        };

        // this is our intended tile - it's the goal of where we'd move but it isn't the immediate next position
        DrawRectangleLines(intendedCol * TILE_SIZE, intendedRow * TILE_SIZE, 26, 26, RED);

        Vector2 intendedPosition = {
            pacman.mPosition.x + (intendedDirection.x * pacman.mSpeed * GetFrameTime()),
            pacman.mPosition.y + (intendedDirection.y * pacman.mSpeed * GetFrameTime())
        };

        // this is where pacman will go
        DrawCircleLines(intendedPosition.x, intendedPosition.y, pacman.mRadius, ORANGE);

        bool canMove = true;
        for (int row = currentRow-1; row <= currentRow+1; ++row) {
            for (int col = currentCol-1; col <= currentCol+1; ++col) {
                if (labyrinth.at(row, col) == Labyrinth::Tile::WALL || labyrinth.at(row, col) == Labyrinth::Tile::GATE) {
                    Rectangle tmpRect = { static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE), TILE_SIZE, TILE_SIZE };
                    if (CheckCollisionCircleRec(intendedPosition, pacman.mRadius, tmpRect)) {
                        DrawRectangle(tmpRect.x, tmpRect.y, 26, 26, ORANGE);
                        canMove = false;
                    }
                }
            }
        }

        if (canMove) {
            pacman.move(intendedDirection, intendedPosition);
        }

        pacman.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
