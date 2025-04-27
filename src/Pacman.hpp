#pragma once

#include "Labyrinth.hpp"

#include <raylib.h>

class Pacman {
public:
    Pacman();
    void draw();

    float lerp(float a, float b, float f) { return a + f * (b - a); }
    void move(Vector2, const Labyrinth &);  // NOLINT

    Color   mColor;
    Color   mDebugTileColor;
    Vector2 mDirection;
    int     mRadius;
    Vector2 mPosition;
    float   mSpeed;
};
