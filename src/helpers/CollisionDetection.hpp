#pragma once

#include <raylib.h>
#include <raymath.h>

#include "../Labyrinth.hpp"
#include "../Pacman.hpp"

inline void CheckCollisions(Pacman pacman, Labyrinth labyrinth) {
    int windowRow = std::floor(pacman.mPosition.x);
    int windowCol = std::floor(pacman.mPosition.y);


    for (int row = windowRow - 2; row < windowRow + 2; ++windowRow) {
        for (int col = windowCol - 2; col < windowCol + 2; ++windowCol) {
        }
    }

    int pacX = std::floor(pacman.mPosition.x);
    int pacY = std::floor(pacman.mPosition.y);

    if (labyrinth.at(pacX, pacY) == Labyrinth::Tile::WALL) {
        std::cout << "nooooooooooooooooooooo\n";
    }
}
