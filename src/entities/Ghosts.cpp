#include <chrono>
#include <iostream>
#include <vector>

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
  auto newPosition = m_ghostShape.getPosition() + m_movement;
  auto coords = tileCoordsAtPosition(sf::Vector2f(newPosition.x + 20.f, newPosition.y));
  //std::cout << "ghost coords: " << coords.first << ", " << coords.second << "\n";

  auto x2 = newPosition.x;
  auto y2 = newPosition.y;
  auto x1 = m_ghostShape.getPosition().x;
  auto y1 = m_ghostShape.getPosition().y;

  //std::cout << "sqrt((" << x2 << " - " << x1 << ")^2 "

  //auto calculatedDirection = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
  auto calculatedDirection = sf::Vector2f(
    static_cast<float>(x2 - x1),
    static_cast<float>(y2 - y1)
  );

  //std::cout << "calculated direction: " << calculatedDirection.x << ", " << calculatedDirection.y << "\n";

  auto currentDirection = directionVecToDirection(calculatedDirection);
  auto turns = availableTurns(newPosition, calculatedDirection, labyrinth);

  bool wallCollision = wallCollides(
    newPosition,
    sf::Vector2f(24.f, 24.f),
    labyrinth
  );

  if (wallCollision)
  {
    Direction newDirection = static_cast<Direction>(m_randGenerator() % turns.size());
    changeDirection(newDirection);
    m_ghostShape.move(m_movement);
    return;
  } 

/*
  if (turns.size() > 1)
  {
    bool doesGhostTurn = (m_randGenerator() % 100) <= m_meanderOdds;
    if (doesGhostTurn)
      changeDirection(static_cast<Direction>(m_randGenerator() % turns.size()));
  }
  */
  m_ghostShape.move(m_movement);
}

//void Ghost::scatter() {}

void Ghost::draw()
{
  m_pGameWindow->draw(m_ghostShape);
}
