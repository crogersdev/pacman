#pragma once

#include <raylib.h>

class Pacman {
public:
    Pacman();
    void draw();
    void move();

private:
    Color   mColor;
    Vector2 mPosition;
};