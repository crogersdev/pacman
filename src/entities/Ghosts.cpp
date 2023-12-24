#include <iostream>
#include "Ghosts.hpp"
#include "../helpers/Collisions.hpp"

Ghost::Ghost(std::shared_ptr<sf::RenderWindow> pGameWindow, float speed)
  : m_speed(speed),
    m_ghostShape(sf::Vector2f(25.f, 25.f)),
    m_deltaTime(),
    m_movement(sf::Vector2f(1, 0)),
    m_position(sf::Vector2f(350.f, 125.f)),
    m_pGameWindow(pGameWindow),
    m_state(MEANDER)
{
  m_ghostShape.setFillColor(sf::Color(219, 48, 130));
  m_ghostShape.setPosition(m_position);
}

Ghost::~Ghost() {}

//void Ghost::chase() {}

void Ghost::meander(sf::Clock &rGameMgrClock, const Labyrinth& labyrinth)
{
  auto newPosition = m_ghostShape.getPosition() + m_movement;
  auto coords = tileCoordsAtPosition(sf::Vector2f(newPosition.x + 20.f, newPosition.y));

  bool wallCollision = wallCollides(
    newPosition,
    sf::Vector2f(24.f, 24.f),
    labyrinth
  );

  //if (labyrinth.at(coords.first, coords.second) == labyrinth.WALL) {
  if (wallCollision)
  {
    m_movement = sf::Vector2f(0, 1);
    return;
  }

  m_ghostShape.move(m_movement);
}

//void Ghost::scatter() {}

void Ghost::draw()
{
  m_pGameWindow->draw(m_ghostShape);
}
