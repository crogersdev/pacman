#pragma once

#include "Labyrinth.hpp"

#include <raylib.h>

class Pacman {
public:
    Pacman();
    void draw();
    
    inline float lerp(float a, float b, float f) { return a + f * (b - a); }
    void move(const Labyrinth &);

    Color   mColor;
    Vector2 mDirection;
    int     mRadius;
    Vector2 mPosition;
    float   mSpeed;
};