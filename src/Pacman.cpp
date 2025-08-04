#include "Pacman.hpp"

#include <cmath>

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDebugTileColor(GetColor(0x000000FF)),
      mDirection{ 0.f, 0.f },
      mPacmanSprite("res/pacman.png", 26, 26, 3, 10),
      mPosition{ 11 * TILE_SIZE + TILE_SIZE / 2, 14 * TILE_SIZE + TILE_SIZE / 2 },
      mRadius(12),
      mSpeed(50.f)
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

bool Pacman::isCentered() {
    const float ALIGNMENT_THRESHOLD = 1.5f;
    const float TILE_CENTER_OFFSET = TILE_SIZE / 2.f;
    
    float distFromTileCenterX = fabs(
        (mPosition.x - TILE_CENTER_OFFSET) -
        (static_cast<int>(mPosition.x / TILE_SIZE) * TILE_SIZE));
    float distFromTileCenterY = fabs(
        (mPosition.y - TILE_CENTER_OFFSET) -
        (static_cast<int>(mPosition.y / TILE_SIZE) * TILE_SIZE));

    return (distFromTileCenterX < ALIGNMENT_THRESHOLD &&
            distFromTileCenterY < ALIGNMENT_THRESHOLD);
}

Vector2 Pacman::getTilePosition() const {
    return Vector2{ mPosition.x / TILE_SIZE, mPosition.y / TILE_SIZE };
}

void Pacman::move(Vector2 intendedDirection, const Labyrinth &rLabyrinth) {

    // std::cout << "{ " << intendedDirection.x << ", " << intendedDirection.y << " }\n";
    int currentTileX = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentTileY = static_cast<int>(mPosition.y / TILE_SIZE);

    Vector2 intendedPosition = {
        (currentTileX + intendedDirection.x) * TILE_SIZE + TILE_SIZE / 2.f,
        (currentTileY + intendedDirection.y) * TILE_SIZE + TILE_SIZE / 2.f
    };

    if (isCentered()) {
        if (intendedDirection.x != 0.f || intendedDirection.y != 0.f) {
            if (rLabyrinth.isLegalMove(intendedPosition)) {
                mDirection = intendedDirection;
            } else {
                std::cout << "move not legal\n";
            }
        } else {
            // no more input 
            mDirection = { 0.f, 0.f };
        }

        Vector2 intendedTileFromMomentum = {
            (currentTileX + mDirection.x) * TILE_SIZE + TILE_SIZE / 2.f,
            (currentTileY + mDirection.y) * TILE_SIZE + TILE_SIZE / 2.f,
        };

        if (!rLabyrinth.isLegalMove(intendedTileFromMomentum)) {
            // prevents hitting illegal tile
            mDirection = { 0.f, 0.f };
        }
    }

    if (mDirection.x == 0.f && mDirection.y == 0.f) {
        return;
    }

    mPacmanSprite.update();

    if (mDirection == Vector2{  1.,  0.}) { mPacmanSprite.setZeroFrame(0); }
    if (mDirection == Vector2{  0.,  1.}) { mPacmanSprite.setZeroFrame(3); }
    if (mDirection == Vector2{ -1.,  0.}) { mPacmanSprite.setZeroFrame(6); }
    if (mDirection == Vector2{  0., -1.}) { mPacmanSprite.setZeroFrame(9); }

    mPosition.x += mDirection.x * mSpeed * GetFrameTime();
    mPosition.y += mDirection.y * mSpeed * GetFrameTime();
}