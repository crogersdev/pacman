#pragma once

#include <raylib.h>
#include <raymath.h>

#include "../Labyrinth.hpp"
#include "../Pacman.hpp"

void CheckCollisions(Pacman pacman, Labyrinth labyrinth) {
    int windowRow = std::floor(pacman.mPosition.x);
    int windowCol = std::floor(pacman.mPosition.y);

    for (int row = windowRow - 2; row < windowRow + 2; ++row) {
        for (int col = windowCol - 2; col < windowCol + 2; ++col) {
        }
    }

    int pacX = (int) pacman.mPosition.x / LABYRINTH_COLS;
    int pacY = (int) pacman.mPosition.y / LABYRINTH_ROWS;

    std::cout << labyrinth.at(pacX, pacY);

}
