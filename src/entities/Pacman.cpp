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

  // EXPLAIN: the min is the negative value of pacman's size so that
  //          you can see him disappear as he goes left without having him
  //          blink out the second his top left corner hits the border
  wrapCoordinate(newPosition.x, -mRadius * 2, rLabyrinth.mMaxLabyrinthWidth);
  wrapCoordinate(newPosition.y, -mRadius * 2, rLabyrinth.mMaxLabyrinthHeight);

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
