#include "Labyrinth.hpp"
#include "Pacman.hpp"

Pacman::Pacman()
    : mPosition{25., 25.},
      mColor(GetColor(0xFFFF00FF)) {

}

void Pacman::draw() {
    DrawCircle(mPosition.x, mPosition.y, 12, mColor);
}

void Pacman::move() {
    
}