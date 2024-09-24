#include <iostream>

#include "./Pacman.hpp"
#include "../helpers/Collisions.hpp"

Pacman::Pacman(
  float radius,
  float speed,
  sf::Vector2f initialPosition)
  : mGuys(4),
    mSpeed(speed),
    mRadius(radius),
    mPacman(sf::CircleShape(radius)),
    mPosition(initialPosition),
    mDirection(Direction::DOWN) {
  mPacman.setFillColor(sf::Color::Yellow);
  mPacman.setPosition(mPosition);
}

Pacman::~Pacman() {}

void Pacman::draw(std::shared_ptr<sf::RenderWindow> p_Window) {
  p_Window->draw(mPacman);
}

void Pacman::move(sf::Vector2f direction, sf::Time t, const Labyrinth &rLabyrinth) {
  auto movement = direction * (mSpeed * t.asSeconds());
  mDirection = directionVecToDirection(direction);

  sf::Vector2f newPosition = mPacman.getPosition() + movement;

  auto maxLabyrinthWidth = rLabyrinth.m_labyrinthCols * (rLabyrinth.m_labyrinthTileSize - 1);
  auto maxLabyrinthHeight = rLabyrinth.m_labyrinthRows * (rLabyrinth.m_labyrinthTileSize - 1);

  // EXPLAIN: the min is the negative value of pacman's size so that
  //          you can see him disappear as he goes left without having him
  //          blink out the second his top left corner hits the border
  wrapCoordinate(newPosition.x, -mRadius * 2, maxLabyrinthWidth);
  wrapCoordinate(newPosition.y, -mRadius * 2, maxLabyrinthHeight);

  auto pacmanWidth = (mRadius * 2.f) - 1.f;
  bool wallCollision = wallCollides(
    newPosition,
    sf::Vector2f(pacmanWidth, pacmanWidth),
    rLabyrinth);

  // TRICKY: we avoid .move(movement) here because doing so would ignore
  //         the arithmetic we implemented to wrap pacman around the edges
  if (!wallCollision) {
    mPacman.setPosition(newPosition);
  }
}

sf::Vector2f Pacman::getPosition() const {
  return mPacman.getPosition();
}

float Pacman::getRadius() const {
  return mRadius;
}
