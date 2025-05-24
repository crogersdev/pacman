#pragma once

#include "helpers/AnimatedSprite.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>

class Pacman {
public:
    Pacman();
    void  draw();
    float lerp(float a, float b, float f) { return a + f * (b - a); }
    void  move(Vector2, Vector2);
    void  moveOLD(Vector2, const Labyrinth &);  // NOLINT

    AnimatedSprite mPacmanSprite;
    Color          mColor;
    Color          mDebugTileColor;
    Vector2        mDirection;
    int            mRadius;
    Vector2        mPosition;  // this is registered in pixels
    float          mSpeed;
};
