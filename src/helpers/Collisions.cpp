#include <iostream>

#include "./Collisions.hpp"
#include "../entities/Labyrinth.hpp"

std::vector<Direction> availableTurns(
  sf::Vector2f currentPosition,
  sf::Vector2f currentDirection,
  const Labyrinth &rLabyrinth)
{
  auto coords = tileCoordsAtPosition(currentPosition);

  std::vector<Direction> turns;

  if (rLabyrinth.at(coords.first, coords.second - 1) != Labyrinth::Tile::WALL &&
      rLabyrinth.at(coords.first, coords.second - 1) != Labyrinth::Tile::GATE)
    turns.push_back(Direction::UP);
  if (rLabyrinth.at(coords.first, coords.second + 1) != Labyrinth::Tile::WALL &&
      rLabyrinth.at(coords.first, coords.second + 1) != Labyrinth::Tile::GATE)
    turns.push_back(Direction::DOWN);
  if (rLabyrinth.at(coords.first + 1, coords.second) != Labyrinth::Tile::WALL &&
      rLabyrinth.at(coords.first + 1, coords.second) != Labyrinth::Tile::GATE)
    turns.push_back(Direction::RIGHT);
  if (rLabyrinth.at(coords.first - 1, coords.second) != Labyrinth::Tile::WALL &&
      rLabyrinth.at(coords.first - 1, coords.second) != Labyrinth::Tile::GATE)
    turns.push_back(Direction::LEFT);

  return turns;
}

bool entityCollides(const sf::Shape &r_entity_a, const sf::Shape &r_entity_b)
{
  if (r_entity_a.getGlobalBounds().intersects(r_entity_b.getGlobalBounds()))
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
      if (rLabyrinth.at(coords.first, coords.second) == rLabyrinth.Tile::WALL)
        return true;
      else
        return false;
  });

  return wallCollision;
}