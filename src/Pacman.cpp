#include <cmath>

#include "Pacman.hpp"
#include "helpers/Movement.hpp"

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDebugTileColor(GetColor(0x000000FF)),
      mDirection{ 0.f, 0.f },
      mDyingSprite("res/pacman-dying.png", 26, 26, 12, 8),
      mInitialPosition{ 11.f * TILE_SIZE + TILE_SIZE / 2.f, 14.f * TILE_SIZE + TILE_SIZE / 2.f },
      mPlayingSprite("res/pacman.png", 26, 26, 3, 10),
      mPosition{ 11.f * TILE_SIZE + TILE_SIZE / 2.f, 14.f * TILE_SIZE + TILE_SIZE / 2.f },
      mRadius(12),
      mState(State::PLAYING),
      mSpeed(50.f)
{}

Pacman::~Pacman() { std::cout << "PACMAN DESTROYED\n"; }

void Pacman::checkMomentumCollision(int currentTileX, int currentTileY, shared_ptr<Labyrinth> labyrinth) {
    Vector2 intendedTileFromMomentum = {
        (currentTileX + mDirection.x) * TILE_SIZE + TILE_SIZE / 2.f,
        (currentTileY + mDirection.y) * TILE_SIZE + TILE_SIZE / 2.f,
    };

    if (!labyrinth->isLegalMove(intendedTileFromMomentum)) {
        // prevents hitting illegal tile
        mDirection = Vector2Zero();
    }
}

void Pacman::draw() {
    if (mState == State::PLAYING) {
        mPlayingSprite.draw(mPosition);
    }
    else if (mState == State::DYING) {
        mDyingSprite.update();
        mDyingSprite.draw(mPosition);
    }
}

bool Pacman::finishedDying() {
    if (mDyingSprite.onFinalFrame() == false) return false;

    mDyingSprite.reset();
    return true;
}

std::pair<int, int> Pacman::getTilePosition() const {
    int x = static_cast<int>(mPosition.x / TILE_SIZE);
    int y = static_cast<int>(mPosition.y / TILE_SIZE);
    return std::make_pair(x, y);
}

void Pacman::move(Vector2 intendedDirection, shared_ptr<Labyrinth> labyrinth) {
    int currentTileX = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentTileY = static_cast<int>(mPosition.y / TILE_SIZE);

    Vector2 intendedPosition = {
        (currentTileX + intendedDirection.x) * TILE_SIZE + TILE_SIZE / 2.f,
        (currentTileY + intendedDirection.y) * TILE_SIZE + TILE_SIZE / 2.f
    };

    if (!isCentered(mPosition, TILE_SIZE)) {
        updateSpriteFrameAndMove();
        return;
    }

    mDirection = handleDirectionInput(intendedDirection, intendedPosition, labyrinth);
    checkMomentumCollision(currentTileX, currentTileY, labyrinth);
    updateSpriteFrameAndMove();
}

Vector2 Pacman::handleDirectionInput(Vector2 intendedDirection, Vector2 intendedPosition, shared_ptr<Labyrinth> labyrinth) {
    if (intendedDirection.x == 0.f && intendedDirection.y == 0.f) {
        return Vector2Zero();
    }

    if (labyrinth->isLegalMove(intendedPosition)) {
        return intendedDirection;
    }

    return mDirection;
}

void Pacman::updateSpriteFrameAndMove() {
    if (mDirection.x == 0.f && mDirection.y == 0.f) {
        return;
    }

    if (mDirection == Vector2{  1.f,  0.f}) { mPlayingSprite.setZeroFrame(0); }
    if (mDirection == Vector2{  0.f,  1.f}) { mPlayingSprite.setZeroFrame(3); }
    if (mDirection == Vector2{ -1.f,  0.f}) { mPlayingSprite.setZeroFrame(6); }
    if (mDirection == Vector2{  0.f, -1.f}) { mPlayingSprite.setZeroFrame(9); }

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
    mPlayingSprite.update();
}