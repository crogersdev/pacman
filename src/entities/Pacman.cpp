#include "Pacman.hpp"

#include "../helpers/Collisions.hpp"

Pacman::Pacman()
  : m_pacman(sf::CircleShape()),
	  m_radius(25.f)
{

}

Pacman::~Pacman() {}

void Pacman::move(sf::Vector2f movement, const Labyrinth &r_labyrinth)
{
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