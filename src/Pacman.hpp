#pragma once

#include <memory>

#include "helpers/AnimatedSprite.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>

using std::shared_ptr;

class Pacman {
public:
    enum class State {
        PLAYING = 0,
        DYING   = 1
    };

    Pacman();
    ~Pacman();

    void    checkMomentumCollision(int, int, shared_ptr<Labyrinth>);
    void    draw();
    bool    finishedDying();
    Vector2 getPosition() const { return mPosition; }
    State   getState() const { return mState; }
    Vector2 handleDirectionInput(Vector2, Vector2, shared_ptr<Labyrinth>);
    float   lerp(float a, float b, float f) { return a + f * (b - a); }
    void    move(Vector2, shared_ptr<Labyrinth>);
    void    resetPosition() { mPosition = mInitialPosition; }
    void    setState(const State s) { mState = s; }
    void    updateSpriteFrameAndMove();

    std::pair<int, int> getTilePosition() const;

    Color          mColor;
    Color          mDebugTileColor;
    Vector2        mDirection;
    AnimatedSprite mDyingSprite;
    Vector2        mInitialPosition;
    AnimatedSprite mPlayingSprite;
    Vector2        mPosition;  // this is registered in pixels
    int            mRadius;
    float          mSpeed;
    State          mState;
};
