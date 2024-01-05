#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_set>

#include "Ghosts.hpp"
#include "../helpers/Collisions.hpp"

Ghost::Ghost(std::shared_ptr<sf::RenderWindow> pGameWindow, float speed)
  : m_meanderOdds(66.6),
    m_speed(speed),
    m_ghostShape(sf::Vector2f(25.f, 25.f)),
    m_deltaTime(),
    m_movement(sf::Vector2f(1.f, 0.f)),
    m_position(sf::Vector2f(350.f, 125.f)),
    m_direction(RIGHT),
    m_pGameWindow(pGameWindow),
    m_state(MEANDER)
{
  m_seed = std::chrono::system_clock::now().time_since_epoch().count(),
  m_randGenerator = std::mt19937(m_seed);
  m_ghostShape.setFillColor(sf::Color(219, 48, 130));
  m_ghostShape.setPosition(m_position);
}

Ghost::~Ghost() {}

void Ghost::chase()
{

}

void Ghost::changeDirection(Direction newDirection)
{
  switch(newDirection)
  {
    case DOWN:
      m_movement = sf::Vector2f(0.f,  1.f);
      break;
    case UP:
      m_movement = sf::Vector2f(0.f, -1.f);
      break;
    case RIGHT:
      m_movement = sf::Vector2f(1.f,  0.f);
      break;
    case LEFT:
      m_movement = sf::Vector2f(-1.f, 0.f); 
      break;
  }
}

void Ghost::meander(sf::Clock &rGameMgrClock, const Labyrinth& labyrinth)
{
  // EXPLAIN:
  // first check to see if we have any available turns, before we start moving
  // let's do this by assuring ourselves that the ghost is in a single tile
  // and that it has turns available
  auto ghostSizeX = m_ghostShape.getGlobalBounds().width;
  auto ghostSizeY = m_ghostShape.getGlobalBounds().height;

  // EXPLAIN:
  // i want to make an unordered set of std::pair<int, int> objects to keep
  // track of how many unique coordinates we have for each corner.  this is
  // how i want to make sure we're in a single tile.  so i want a set.
  // but we need to be able to define a hash function for the objects in that
  // set, as std::pair<int, int> doesn't have one defined.
  struct PairHash {
      size_t operator()(const std::pair<int, int>& p) const {
        // Custom hash function combining hashes of first and second elements
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        //     std::hash<int>()  <-- this is a function, a ctor (i think it's a ctor)
        //     std::hash<int>()(p.first)  <-- this is also a function with an arg of p.first
      }
  };

  // make an unordered set of std::pair<int, int> coords for all corners of ghost
  std::unordered_set<std::pair<int, int>, PairHash> coords;
  coords.emplace(tileCoordsAtPosition(sf::Vector2f(m_position.x, m_position.y)));
  coords.emplace(tileCoordsAtPosition(sf::Vector2f(m_position.x + ghostSizeX, m_position.y)));
  coords.emplace(tileCoordsAtPosition(sf::Vector2f(m_position.x, m_position.y + ghostSizeY)));
  coords.emplace(tileCoordsAtPosition(sf::Vector2f(m_position.x + ghostSizeX, m_position.y + ghostSizeY)));

  // EXPLAIN:
  // now let's calculate some helpful values like our direction as a sf::Vector2f,
  // our direction as a Direction enum, and our available turns at the current position.
  auto newPosition = m_ghostShape.getPosition() + m_movement;

  auto x2 = newPosition.x;
  auto y2 = newPosition.y;
  auto x1 = m_ghostShape.getPosition().x;
  auto y1 = m_ghostShape.getPosition().y;

  auto calculatedDirection = sf::Vector2f(
    static_cast<float>(x2 - x1),
    static_cast<float>(y2 - y1)
  );

  auto currentDirection = directionVecToDirection(calculatedDirection);
  auto turns = availableTurns(m_ghostShape.getPosition(), calculatedDirection, labyrinth);

  // EXPLAIN:
  // check if length of set of tile coords for ghost's 4 corner sf::Vector2f position
  // vectors == 1.  e.g., all corners occupy a single tile.
  // for debug purposes, if we never move the ghost in the first place, we should be able
  // to position him (or her!) somewhere in the labyrinth and have it stay put while
  // we take the previous measurements and they'll come back as a set with a single
  // entry.
  /*
  if (coords.size() == 1 && turns.size() >= 2)
  {
    // EXPLAIN: let's roll the dice to see if we're going to turn
    bool doesGhostTurn = (m_randGenerator() % 100) <= m_meanderOdds;
    if (doesGhostTurn) {
      auto newDirection = m_randGenerator() % turns.size();
      changeDirection(turns.at(newDirection));
      m_ghostShape.move(m_movement);
    }
  }
  */

  bool wallCollision = wallCollides(
    newPosition,
    sf::Vector2f(24.f, 24.f),
    labyrinth
  );

  if (wallCollision)
  {
    Direction newDirection = static_cast<Direction>(m_randGenerator() % 4);
    changeDirection(newDirection);
    m_ghostShape.move(m_movement);
    return;
  } 

  // EXPLAIN:
  // if we've gotten here, we didn't take a turn nor did we collide with a wall.  
  // if we didn't take a turn, it's because we rolled the dice and kept going
  m_ghostShape.move(m_movement);
}

//void Ghost::scatter() {}

void Ghost::draw()
{
  m_pGameWindow->draw(m_ghostShape);
}
