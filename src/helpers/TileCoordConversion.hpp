#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>

enum class Direction {
  DOWN  = 0,
  UP    = 1,
  RIGHT = 2,
  LEFT  = 3,
  NOPE  = 4
};

inline std::ostream &operator<<(std::ostream& os, Direction d) {
  switch (d) {
  case Direction::DOWN:
    os << "DOWN";
    break;
  case Direction::UP:
    os << "UP";
    break;
  case Direction::LEFT:
    os << "LEFT";
    break;
  case Direction::RIGHT:
    os << "RIGHT";
    break;
  case Direction::NOPE:
    os << "NOPE";
    break;
  default:
    os << "NOT A DIRECTION";
  }
  return os;
}

void wrapCoordinate(float &, float min, float);
std::pair<int, int> tileCoordsAtPosition(sf::Vector2f);
sf::Vector2f normalize(const sf::Vector2f &);
Direction directionVecToDirection(sf::Vector2f);