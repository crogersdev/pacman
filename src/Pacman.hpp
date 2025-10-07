#pragma once

#include <memory>

#include "helpers/AnimatedSprite.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>

using std::shared_ptr;

class Pacman {
public:
    Pacman();
    ~Pacman();

    void    checkMomentumCollision(int, int, shared_ptr<Labyrinth>);
    void    draw();
    bool    isCentered();
    Vector2 getTilePosition() const;
    Vector2 getPosition() const { return mPosition; }
    Vector2 handleDirectionInput(Vector2, Vector2, shared_ptr<Labyrinth>);
    float   lerp(float a, float b, float f) { return a + f * (b - a); }
    void    move(Vector2, shared_ptr<Labyrinth>);
    void    updateSpriteFrameAndMove();

    Color          mColor;
    Color          mDebugTileColor;
    Vector2        mDirection;
    AnimatedSprite mPacmanSprite;
    Vector2        mPosition;  // this is registered in pixels
    int            mRadius;
    float          mSpeed;
};
