#pragma once

#include <iostream>

#include "Ghost.hpp"
#include "Labyrinth.hpp"

std::ostream &operator<<(std::ostream &os, Ghost::State s);
std::ostream &operator<<(std::ostream& os, Labyrinth::Tile t);