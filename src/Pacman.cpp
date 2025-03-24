#include "Labyrinth.hpp"
#include "Pacman.hpp"

#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mPosition{11*TILE_SIZE + TILE_SIZE / 2, 9*TILE_SIZE + TILE_SIZE / 2},
      mSpeed(2.),
      mVelocity{0., 0.} {

}

void Pacman::draw() {
    // #ifndef NDEBUG
    //     std::cout << "\t rendering pacman at x,  y: " << mPosition.x << ", " << mPosition.y << "\n";
    // #endif

    DrawCircle(mPosition.x, mPosition.y, 12, mColor);
}

void Pacman::move() {

    // reset velocity after each frame
    mVelocity = {0., 0.};
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    mVelocity.y = -1.0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  mVelocity.x = -1.0;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  mVelocity.y = 1.0;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) mVelocity.x = 1.0;

    mPosition = Vector2Add(mPosition, Vector2Scale(mVelocity, mSpeed + GetFrameTime()));
}