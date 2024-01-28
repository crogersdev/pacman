#include <iostream>
#include "Pacman.hpp"

#include "../helpers/Collisions.hpp"

Pacman::Pacman(
  float radius,
  float speed,
  sf::Vector2f initialPosition
  )
  : m_guys(4), 
    m_speed(speed),
	  m_radius(radius),
    m_pacman(sf::CircleShape(radius)),
    m_position(initialPosition),
    m_direction(Direction::DOWN)
{
  m_pacman.setFillColor(sf::Color::Yellow);
  m_pacman.setPosition(m_position);
}

Pacman::~Pacman() {}

void Pacman::draw(std::shared_ptr<sf::RenderWindow> p_Window)
{
  p_Window->draw(m_pacman);
}

void Pacman::move(sf::Vector2f direction, sf::Time t, const Labyrinth &r_labyrinth)
{
  auto movement = direction * (m_speed * t.asSeconds());
  m_direction = directionVecToDirection(direction);

  sf::Vector2f newPosition = m_pacman.getPosition() + movement;

  auto maxLabyrinthWidth = r_labyrinth.m_labyrinthCols * r_labyrinth.m_labyrinthTileSize;
  auto maxLabyrinthHeight = r_labyrinth.m_labyrinthRows * r_labyrinth.m_labyrinthTileSize;

  wrapCoordinate(newPosition.x, -m_radius * 2, maxLabyrinthWidth);
  wrapCoordinate(newPosition.y, -m_radius * 2, maxLabyrinthHeight);

  auto pacmanWidth = (m_radius * 2.f) - 1.f;
  bool wallCollision = wallCollides(
    newPosition,
    sf::Vector2f(pacmanWidth, pacmanWidth),
    r_labyrinth
  );

  // TRICKY: we avoid .move(movement) here because doing so would ignore
  //         the arithmetic we implemented to wrap pacman around the edges
  if (!wallCollision) 
    m_pacman.setPosition(newPosition);
}

sf::Vector2f Pacman::getPosition()
{
  return m_pacman.getPosition();
}
