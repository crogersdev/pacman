#include "Collisions.hpp"
#include "../helpers/TileCoordConversion.hpp"
#include "../entities/Labyrinth.hpp"

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

bool entityCollides(sf::RectangleShape entity_a, sf::RectangleShape entity_b)
{
  if (entity_a.getGlobalBounds().intersects(entity_b.getGlobalBounds()))
    return true;
  return false;
}