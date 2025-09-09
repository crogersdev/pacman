#pragma once

#include <memory>

#include "helpers/AnimatedSprite.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>

class Pacman {
public:
    Pacman();
    ~Pacman();

    void    draw();
    bool    isCentered();
    Vector2 getTilePosition() const;
    Vector2 getPosition() const { return mPosition; }
    float   lerp(float a, float b, float f) { return a + f * (b - a); }
    void    move(Vector2, std::shared_ptr<Labyrinth>);

    Color          mColor;
    Color          mDebugTileColor;
    Vector2        mDirection;
    AnimatedSprite mPacmanSprite;
    Vector2        mPosition;  // this is registered in pixels
    int            mRadius;
    float          mSpeed;
};
