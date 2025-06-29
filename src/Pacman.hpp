#pragma once

#include "helpers/AnimatedSprite.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>

class Pacman {
public:
    Pacman();
    ~Pacman();

    void  draw();
    float lerp(float a, float b, float f) { return a + f * (b - a); }
    bool  isCentered();
    void  move(Vector2, Vector2);

    Color          mColor;
    Color          mDebugTileColor;
    Vector2        mDirection;
    AnimatedSprite mPacmanSprite;
    Vector2        mPosition;  // this is registered in pixels
    int            mRadius;
    float          mSpeed;
};
