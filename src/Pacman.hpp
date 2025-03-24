#pragma once

#include <raylib.h>

class Pacman {
public:
    Pacman();
    void draw();
    void move();

// private:
    Color   mColor;
    float   mSpeed;
    Vector2 mPosition;
    Vector2 mVelocity;
};