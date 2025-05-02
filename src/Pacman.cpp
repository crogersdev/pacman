#include "Pacman.hpp"

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDebugTileColor(GetColor(0x000000FF)),
      mDirection{0., 0.},
      mPacmanSprite("res/pacman-face-right.png", 32, 32, 0, .5),  // TRICKY: Path is relative to 
      mRadius(12),
      mPosition{11*TILE_SIZE + TILE_SIZE / 2, 14*TILE_SIZE + TILE_SIZE / 2},
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

    // DrawCircle(mPosition.x, mPosition.y, mRadius, mColor);
    // DrawRectangle(mPosition.x, mPosition.y, TILE_SIZE, TILE_SIZE, GetColor(0x882200FF));
    mPacmanSprite.draw(mPosition);
}

void Pacman::move(Vector2 newDirection, const Labyrinth &rLabyrinth) {  // NOLINT
    // std::cout << "direction you're trying to move: [" << newDirection.x << ", " << newDirection.y << "]\n";

    int currentCol = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentRow = static_cast<int>(mPosition.y / TILE_SIZE);

    Vector2 newPosition = {
        mPosition.x + (newDirection.x * mSpeed * GetFrameTime()),
        mPosition.y + (newDirection.y * mSpeed * GetFrameTime())
    };

    // int rgb = (ColorToInt(mDebugTileColor) & 0xFFFFFF00);
    // int opacity = (ColorToInt(mDebugTileColor) & 0x000000FF);

    for (int row = currentRow-1; row <= currentRow+1; ++row) {
        for (int col = currentCol-1; col <= currentCol+1; ++col) {
            // rgb += 0x000F0000;
            // auto debugColor = (rgb & 0xFFFFFF00) | opacity;
            // DrawRectangle(col*TILE_SIZE, row*TILE_SIZE, TILE_SIZE, TILE_SIZE, GetColor(debugColor));

            if (row != TUNNEL_ROW && (row < 0 || col < 0 || col >= rLabyrinth.getWidth() || row >= rLabyrinth.getHeight())) {  // NOLINT
                std::cout << "out of bounds\n";
                continue;
            }

            if (rLabyrinth.at(row, col) == Labyrinth::Tile::WALL || rLabyrinth.at(row, col) == Labyrinth::Tile::GATE) {
                Rectangle wallRect = {
                    static_cast<float>(col * TILE_SIZE),
                    static_cast<float>(row * TILE_SIZE),
                    TILE_SIZE,
                    TILE_SIZE
                };

                if (CheckCollisionCircleRec(newPosition, mRadius, wallRect)) {
                    DrawRectangle(wallRect.x, wallRect.y, wallRect.width, wallRect.height, GetColor(0xFFAA11FF));
                    return;
                }
            }
            // std::cout << "checking row, col: " << row << ", " << col << " -- " << rLabyrinth.at(row, col) << "\n";
        }
    }

    mDirection = newDirection;
    mPosition = newPosition;
}
