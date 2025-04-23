#include "Pacman.hpp"

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDebugTileColor(GetColor(0x000000FF)),
      mDirection{0., 0.},
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

    DrawCircle(mPosition.x, mPosition.y, mRadius, mColor);
    // DrawRectangle(mPosition.x, mPosition.y, TILE_SIZE, TILE_SIZE, GetColor(0x882200FF));
}

void Pacman::move(const Labyrinth &rLabyrinth) {  // NOLINT
    // reset velocity after each frame
    Vector2 desiredDirection = {0., 0.};

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    desiredDirection.y = -1.0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  desiredDirection.x = -1.0;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  desiredDirection.y = 1.0;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) desiredDirection.x = 1.0;

    int currentCol = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentRow = static_cast<int>(mPosition.y / TILE_SIZE);

    /*
    if (currentRow != TUNNEL_ROW && (currentRow < 0 || currentCol < 0 || currentCol >= rLabyrinth.getWidth() || currentRow >= rLabyrinth.getHeight())) {  // NOLINT
        std::cout << "out of bounds\n";
        return;
    }

    if (rLabyrinth.at(currentRow, currentCol + desiredDirection.x) == Labyrinth::Tile::WALL) { desiredDirection.x = 0.; }  // NOLINT
    if (rLabyrinth.at(currentRow + desiredDirection.y, currentCol) == Labyrinth::Tile::WALL) { desiredDirection.y = 0.; }  // NOLINT

    Vector2 newPosition = {
        mPosition.x + (desiredDirection.x * mSpeed * GetFrameTime()),
        mPosition.y + (desiredDirection.y * mSpeed * GetFrameTime())
    };

    mDirection = desiredDirection;
    mPosition = newPosition;
    */



    // this way examines all 8 possibilities - up, down, left, right + diagonals
    // and if it's open, you move
    // but: it will go diagonally at 4 way junctions.
    // but: if you're going down a hallway and you hold down the direction
    //      the hallway is in and then also add a perpendicular direction
    //      with another keypress,

    Vector2 newPosition = {
        mPosition.x + (desiredDirection.x * mSpeed * GetFrameTime()),
        mPosition.y + (desiredDirection.y * mSpeed * GetFrameTime())
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

    std::cout << "direction you're trying to move: [" << desiredDirection.x << ", " << desiredDirection.y << "]\n";

    mPosition = newPosition;
}
