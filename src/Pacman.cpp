#include "Pacman.hpp"

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDirection{0., 0.},
      mRadius(12),
      mPosition{11*TILE_SIZE + TILE_SIZE / 2, 9*TILE_SIZE + TILE_SIZE / 2},
      mSpeed(100.)
{}

void Pacman::draw() {
    // #ifndef NDEBUG
    // std::cout << "\t rendering pacman at row,  col: "
    //           << static_cast<int>(mPosition.y / TILE_SIZE)
    //           << ", "
    //           << static_cast<int>(mPosition.x / TILE_SIZE)
    //           << "\n";
    // #endif

    DrawCircle(mPosition.x, mPosition.y, mRadius, mColor);
    DrawRectangle(mPosition.x, mPosition.y, TILE_SIZE, TILE_SIZE, GetColor(0x882200FF));
}

void Pacman::move(const Labyrinth &rLabyrinth) {  // NOLINT
    // reset velocity after each frame
    mDirection = {0., 0.};

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    mDirection.y = -1.0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  mDirection.x = -1.0;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  mDirection.y = 1.0;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) mDirection.x = 1.0;

    if (mDirection.x == 0. && mDirection.y == 0.) {
        return;
    }

    Vector2 newPosition = {
        mPosition.x + (mDirection.x * mSpeed * GetFrameTime()),
        mPosition.y + (mDirection.y * mSpeed * GetFrameTime())
    };

    int currentCol = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentRow = static_cast<int>(mPosition.y / TILE_SIZE);

    for (int row = currentRow-1; row <= currentRow+1; ++row) {
        for (int col = currentCol-1; col <= currentCol+1; ++col) {
            if (row != TUNNEL_ROW && (row < 0 || col < 0 || col >= rLabyrinth.getWidth() || row >= rLabyrinth.getHeight())) {  // NOLINT
                std::cout << "row: " << row << "  col: " << col << "   width: " << rLabyrinth.getWidth() << "   height: " << rLabyrinth.getHeight() << "\n";  // NOLINT
                std::cout << "out of bounds\n";
                continue;
            }

            if (rLabyrinth.at(row, col) == Labyrinth::Tile::WALL || rLabyrinth.at(row, col) == Labyrinth::Tile::GATE) {
                Rectangle wallRect = {
                    static_cast<float>(row * TILE_SIZE),
                    static_cast<float>(col * TILE_SIZE),
                    TILE_SIZE,
                    TILE_SIZE
                };

                if (CheckCollisionCircleRec(newPosition, mRadius, wallRect)) {
                    DrawRectangle(mPosition.x, mPosition.y, TILE_SIZE, TILE_SIZE, GetColor(0xFF0000FF));
                    DrawRectangle(mPosition.x, mPosition.y, TILE_SIZE, TILE_SIZE, GetColor(0xFF0000FF));
                    std::cout << "you're colliding\n";
                    return;
                }
            }
            std::cout << "checking row, col: " << row << ", " << col << " -- " << rLabyrinth.at(row, col) << "\n";
        }
    }

    mPosition = newPosition;
}
