#include <iostream>

#include "Collisions.hpp"
#include "../entities/Labyrinth.hpp"

std::vector<Direction> availableTurns(sf::Vector2f currentPosition,
                         sf::Vector2f currentDirection,
                         const Labyrinth &rLabyrinth)
{
  auto coords = tileCoordsAtPosition(currentPosition);
  auto dir = directionVecToDirection(currentDirection);

  std::vector<Direction> turns;
  
  if (rLabyrinth.at(coords.first, coords.second - 1) != Labyrinth::WALL && dir != Direction::DOWN)
    turns.push_back(Direction::UP);
  if (rLabyrinth.at(coords.first, coords.second + 1) != Labyrinth::WALL && dir != Direction::UP)
    turns.push_back(Direction::DOWN);
  if (rLabyrinth.at(coords.first + 1, coords.second) != Labyrinth::WALL && dir != Direction::RIGHT)
    turns.push_back(Direction::RIGHT);
  if (rLabyrinth.at(coords.first - 1, coords.second) != Labyrinth::WALL && dir != Direction::LEFT)
    turns.push_back(Direction::LEFT);
  
  return turns;
}

bool entityCollides(sf::RectangleShape entity_a, sf::RectangleShape entity_b)
{
  if (entity_a.getGlobalBounds().intersects(entity_b.getGlobalBounds()))
    return true;
  return false;
}

bool wallCollides(sf::Vector2f entity, sf::Vector2f dimensions, const Labyrinth &rLabyrinth)
{
  std::vector<std::pair<int, int>> collisionCorners;

  collisionCorners.push_back(tileCoordsAtPosition(sf::Vector2f(entity.x, entity.y)));
  collisionCorners.push_back(tileCoordsAtPosition(sf::Vector2f(entity.x + dimensions.x, entity.y)));
  collisionCorners.push_back(tileCoordsAtPosition(sf::Vector2f(entity.x, entity.y + dimensions.y)));
  collisionCorners.push_back(tileCoordsAtPosition(sf::Vector2f(entity.x + dimensions.x, entity.y + dimensions.y)));

  bool wallCollision = std::any_of(
    collisionCorners.begin(),
    collisionCorners.end(),
    [&](auto coords) {
      if (rLabyrinth.at(coords.first, coords.second) == rLabyrinth.WALL)
        return true;
      else
        return false;
  });

  return wallCollision;
}