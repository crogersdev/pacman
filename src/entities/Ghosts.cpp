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
    m_position(sf::Vector2f(650.f, 350.f)),
    //m_position(sf::Vector2f(550.f, 550.f)),
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

void Ghost::chase() {}

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

void Ghost::draw()
{
  m_pGameWindow->draw(m_ghostShape);
}

sf::Vector2f Ghost::getPosition()
{ 
  return m_ghostShape.getPosition();
}

void Ghost::meander(sf::Clock &rGameMgrClock, const Labyrinth &r_labyrinth)
{
  // EXPLAIN:
  // let's check for a collision
  auto newPosition = m_ghostShape.getPosition() + m_movement;

  // EXPLAIN:
  // first check to see if we have any available turns, before we start moving
  // let's do this by assuring ourselves that the ghost is in a single tile
  // and that it has turns available
  auto ghostSizeX = m_ghostShape.getGlobalBounds().width;
  auto ghostSizeY = m_ghostShape.getGlobalBounds().height;

  auto isTileX = m_ghostShape.getPosition().x / 25.f;
  auto isTileY = m_ghostShape.getPosition().y / 25.f;
  bool ghostOccupiesSingleTile = (floor(isTileX) == isTileX && floor(isTileY) == isTileY);

  // EXPLAIN:
  // now let's calculate some helpful values like our direction as a sf::Vector2f,
  // our direction as a Direction enum, and our available turns at the current position.
  auto maxLabyrinthWidth = r_labyrinth.m_labyrinthCols * r_labyrinth.m_labyrinthTileSize;
  auto maxLabyrinthHeight = r_labyrinth.m_labyrinthRows * r_labyrinth.m_labyrinthTileSize;
  wrapCoordinate(newPosition.x, -ghostSizeX, maxLabyrinthWidth);
  wrapCoordinate(newPosition.y, -ghostSizeY, maxLabyrinthHeight);

  // EXPLAIN:
  // when we calculate the direction across the open 'tunnel' where
  // we pop on the other side of the labyrinth, we do it by wrapping the
  // coordinates.  this messes up the direction vector, so 
  // our availableTurns and directionVecToDirection get messed up as well
  //auto x2 = fmod(newPosition.x + maxLabyrinthWidth, maxLabyrinthWidth);
  //auto y2 = fmod(newPosition.y + maxLabyrinthHeight, maxLabyrinthHeight);
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

  if (ghostOccupiesSingleTile && turns.size() > 2)
   {
    unsigned int newDirection;
    bool doesGhostTurn = (m_randGenerator() % 100) <= m_meanderOdds;
    if (doesGhostTurn)
    {
      newDirection = m_randGenerator() % turns.size();

      /*
      // EXPLAIN:
      // this is how we make sure we're going in an opposite direction
      auto foo = turns.at(newDirection);
      if (foo == 0 && currentDirection == 1 ||
          foo == 1 && currentDirection == 0 ||
          foo == 2 && currentDirection == 3 ||
          foo == 3 && currentDirection == 2) {
        std::cout << "opposite direction, baby!\n";
      }
      */

      changeDirection(turns.at(newDirection));
      m_ghostShape.setPosition(newPosition);
      return;
    }
  }

  bool wallCollision = wallCollides(
    newPosition,
    sf::Vector2f(24.f, 24.f),
    labyrinth
  );

  while (wallCollision)
  {
    auto newDirection = static_cast<Direction>(m_randGenerator() % 4);
    
    /*
    // EXPLAIN:
    // this is how we make sure we're going in an opposite direction
    auto foo = newDirection;
    if (foo == 0 && currentDirection == 1 ||
        foo == 1 && currentDirection == 0 ||
        foo == 2 && currentDirection == 3 ||
        foo == 3 && currentDirection == 2) { }
    */

    changeDirection(newDirection);
    auto newPosition = m_ghostShape.getPosition() + m_movement;
    bool wallCollision = wallCollides(
      newPosition,
      sf::Vector2f(24.f, 24.f),
      labyrinth
    );

    if (!wallCollision)
    {
      m_ghostShape.setPosition(newPosition);
      return;
    }
  }

  // EXPLAIN:
  // if we've gotten here, we didn't take a turn nor did we collide with a wall.  
  // if we didn't take a turn, it's because we rolled the dice and kept going
  m_ghostShape.setPosition(newPosition);
}

//void Ghost::scatter() {}

