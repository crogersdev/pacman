#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>

enum Direction {
  DOWN  = 0,
  UP    = 1,
  RIGHT = 2,
  LEFT  = 3,
  NOPE  = 4
};

void wrapCoordinate(float &, float min, float);
std::pair<int, int> tileCoordsAtPosition(sf::Vector2f);
Direction directionVecToDirection(sf::Vector2f);