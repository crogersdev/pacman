#include "TileCoordConversion.hpp"
#include "../game-manager/GameManager.hpp"

void wrapCoordinate(float &coord, float min, float max)
{
  if (coord < min)
    coord = max;
  else if (coord > max)
    coord = min;
}

std::pair<int, int> tileCoordsAtPosition(sf::Vector2f pos)
{
  return std::make_pair<int, int>(
    floor(pos.x / TILE_SIZE),
    floor(pos.y / TILE_SIZE));
}

Direction directionVecToDirection(sf::Vector2f direction)
{
  // NOTE: Why aren't we using a LUT with a std::map?
  //       I'll tell you.
  //       std::map requires operators like <, >, ==, etc
  //       to be defined for the objects they are templated
  //       for.  In the case of sf::Vector2f, as the key type
  //       in the std::map, libsfml has not implemented 
  //       the < operator, so we were getting compiler
  //       errors with the following code:
  //       std::map<sf::Vector2f, Direction> directionLut = {
  //         {sf::Vector2f(0.f, 1.f), Direction::DOWN},
  //         {sf::Vector2f(0.f, -1.f), Direction::UP},
  //         {sf::Vector2f(1.f, 0.f), Direction::RIGHT},
  //         {sf::Vector2f(-1.f, 0.f), Direction::LEFT}};
  //       As a result, in order to use a std::map for this
  //       converter function, we'd have to overload the 
  //       comparator signs lke <, > or maybe others.
  //       This if/else feels easier.
  if (direction == sf::Vector2f(0.f, 1.f))
    return Direction::DOWN;
  else if (direction == sf::Vector2f(0.f, -1.f))
    return Direction::UP;
  else if (direction == sf::Vector2f(1.f, 0.f))
    return Direction::RIGHT;
  else if (direction == sf::Vector2f(-1.f, 0.f))
    return Direction::LEFT;
  else
    return Direction::NOPE;
}