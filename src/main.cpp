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

        Rectangle intendedTile = {
            static_cast<float>(intendedCol * TILE_SIZE),
            static_cast<float>(intendedRow * TILE_SIZE),
            TILE_SIZE,
            TILE_SIZE
        };

        // DrawRectangleLines(intendedCol * TILE_SIZE, intendedRow * TILE_SIZE, 26, 26, RED);

        Vector2 intendedPosition = {
            pacman.mPosition.x + (intendedDirection.x * pacman.mSpeed * GetFrameTime()),
            pacman.mPosition.y + (intendedDirection.y * pacman.mSpeed * GetFrameTime())
        };

        // DrawCircleLines(intendedPosition.x, intendedPosition.y, pacman.mRadius, ORANGE);

        float centerTileX, centerTileY;
        bool canMove = true;
        Rectangle tmpRect;

        for (int row = currentRow-1; row <= currentRow+1; ++row) {
            for (int col = currentCol-1; col <= currentCol+1; ++col) {
                centerTileX = col * TILE_SIZE + TILE_SIZE / 2;
                centerTileY = row * TILE_SIZE + TILE_SIZE / 2;

                Labyrinth::Tile tile = labyrinth.at(row, col);
                switch (tile) {
                case Labyrinth::Tile::WALL:
                case Labyrinth::Tile::GATE:
                    tmpRect = { static_cast<float>(col * TILE_SIZE), static_cast<float>(row * TILE_SIZE), TILE_SIZE, TILE_SIZE };
                    if (CheckCollisionCircleRec(intendedPosition, pacman.mRadius, tmpRect)) {
                        DrawRectangle(tmpRect.x, tmpRect.y, 26, 26, ORANGE);
                        canMove = false;
                    }
                    break;
                case Labyrinth::Tile::PELLET:
                    if (!labyrinth.mPellets.count(std::make_pair(col * TILE_SIZE, row * TILE_SIZE))) break;
                    if (CheckCollisionCircles(intendedPosition, pacman.mRadius - 6, { centerTileX, centerTileY }, 3)) {
                        labyrinth.mPellets.erase(std::make_pair(col * TILE_SIZE, row * TILE_SIZE)); 
                    }
                    break;
                case Labyrinth::Tile::POWERUP:
                    if (!labyrinth.mPowerups.count(std::make_pair(col * TILE_SIZE, row * TILE_SIZE))) break;
                    if (CheckCollisionCircles(intendedPosition, pacman.mRadius - 10, { centerTileX, centerTileY }, 8)) {
                        labyrinth.mPowerups.erase(std::make_pair(col * TILE_SIZE, row * TILE_SIZE));
                    }
                    break;
                default:
                    std::cout << "duh\n";
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
