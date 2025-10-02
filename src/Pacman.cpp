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
      mPosition{ 11.f * TILE_SIZE + TILE_SIZE / 2.f, 14.f * TILE_SIZE + TILE_SIZE / 2.f },
      mRadius(12),
      mSpeed(50.f)
{}

Pacman::~Pacman() { std::cout << "PACMAN DESTROYED\n"; }

void Pacman::draw() {
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

void Pacman::move(Vector2 intendedDirection, std::shared_ptr<Labyrinth> labyrinth) {

    int currentTileX = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentTileY = static_cast<int>(mPosition.y / TILE_SIZE);

    Vector2 intendedPosition = {
        (currentTileX + intendedDirection.x) * TILE_SIZE + TILE_SIZE / 2.f,
        (currentTileY + intendedDirection.y) * TILE_SIZE + TILE_SIZE / 2.f
    };

    if (isCentered()) {
        if (intendedDirection.x != 0.f || intendedDirection.y != 0.f) {
            if (labyrinth->isLegalMove(intendedPosition)) {
                mDirection = intendedDirection;
            }
        } else {
            // no more input 
            mDirection = { 0.f, 0.f };
        }

        Vector2 intendedTileFromMomentum = {
            (currentTileX + mDirection.x) * TILE_SIZE + TILE_SIZE / 2.f,
            (currentTileY + mDirection.y) * TILE_SIZE + TILE_SIZE / 2.f,
        };

        if (!labyrinth->isLegalMove(intendedTileFromMomentum)) {
            // prevents hitting illegal tile
            mDirection = { 0.f, 0.f };
        }
    }

    if (mDirection.x == 0.f && mDirection.y == 0.f) {
        return;
    }

    if (mDirection == Vector2{  1.f,  0.f}) { mPacmanSprite.setZeroFrame(0); }
    if (mDirection == Vector2{  0.f,  1.f}) { mPacmanSprite.setZeroFrame(3); }
    if (mDirection == Vector2{ -1.f,  0.f}) { mPacmanSprite.setZeroFrame(6); }
    if (mDirection == Vector2{  0.f, -1.f}) { mPacmanSprite.setZeroFrame(9); }

    Vector2 newPosition = {
        mPosition.x + (mDirection.x * mSpeed * GetFrameTime()),
        mPosition.y + (mDirection.y * mSpeed * GetFrameTime())
    };

    int newTileX = static_cast<int>(newPosition.x / TILE_SIZE);
    int newTileY = static_cast<int>(newPosition.y / TILE_SIZE);

    if (newTileY == TUNNEL_ROW) {
        if (newTileX < 0) {
            newPosition.x = (LABYRINTH_COLS * TILE_SIZE);
        }
        if (newTileX > LABYRINTH_COLS) {
            newPosition.x = 0;
        }
    }

    mPosition = newPosition;
    mPacmanSprite.update();
}