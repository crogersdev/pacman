#pragma once

#include "Labyrinth.hpp"

#include <raylib.h>

class Pacman {
public:
    Pacman();
    void draw();
    
    void move(const Labyrinth &);

    Color   mColor;
    Vector2 mDirection;
    int     mRadius;
    Vector2 mPosition;
    float   mSpeed;
};