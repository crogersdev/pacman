#include "Pacman.hpp"

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDebugTileColor(GetColor(0x000000FF)),
      mDirection{0.f, 0.f},
      mPacmanSprite("res/pacman.png", 26, 26, 3, 10),
      mPosition{ 11 * TILE_SIZE + TILE_SIZE / 2, 14 * TILE_SIZE + TILE_SIZE / 2 },
      mRadius(12),
      mSpeed(100.f)
{}

Pacman::~Pacman() { std::cout << "PACMAN DESTROYED\n"; }

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

void Pacman::move(Vector2 newDirection, Vector2 newPosition) {

    mPacmanSprite.update();
    mDirection = newDirection;
    mPosition = newPosition;

    if (newDirection == (Vector2){1., 0.})  { mPacmanSprite.setZeroFrame(0); }
    if (newDirection == (Vector2){0., 1.})  { mPacmanSprite.setZeroFrame(3); }
    if (newDirection == (Vector2){-1., 0.}) { mPacmanSprite.setZeroFrame(6); }
    if (newDirection == (Vector2){0., -1.}) { mPacmanSprite.setZeroFrame(9); }

}
