#include "Pacman.hpp"

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDirection{0., 0.},
      mPosition{11*TILE_SIZE + TILE_SIZE / 2, 9*TILE_SIZE + TILE_SIZE / 2},
      mSpeed(2.)
{}

void Pacman::draw() {
    // #ifndef NDEBUG
    std::cout << "\t rendering pacman at row,  col: " << (int) (mPosition.y / 24 << ", " << (int) mPosition.x / 23 << "\n";
    // #endif

    DrawCircle(mPosition.x, mPosition.y, 12, mColor);
}

void Pacman::move(const Labyrinth &rLabyrinth) {

    // reset velocity after each frame
    mDirection = {0., 0.};
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    mDirection.y = -1.0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  mDirection.x = -1.0;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  mDirection.y = 1.0;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) mDirection.x = 1.0;

    std::vector<Vector2> cardinalDirections = {
        {0., -1.},   // up
        {0., 1.},    // down
        {1., 0.},    // left
        {-1., 0.}    // right
    };

    int currentCol = (int) mPosition.x / LABYRINTH_COLS;
    int currentRow = (int) mPosition.y / LABYRINTH_ROWS;

    for (const auto& dir : cardinalDirections) {
        int checkingRow = (int) dir.y + currentRow;
        int checkingCol = (int) dir.x + currentCol;
        
        std::cout << "(" << checkingRow << ", " << checkingCol << ") == " << rLabyrinth.at(checkingRow, checkingCol) << "\n";
    }

    mPosition = Vector2Add(
        mPosition, 
        Vector2Scale(mDirection, mSpeed + GetFrameTime())
    );


}