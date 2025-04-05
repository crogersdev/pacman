#pragma once

#include "Labyrinth.hpp"

#include <raylib.h>

class Pacman {
public:
    Pacman();
    void draw();
    void move(const Labyrinth &);

// private:
    Color   mColor;
    Vector2 mDirection;
    Vector2 mPosition;
    float   mSpeed;
};