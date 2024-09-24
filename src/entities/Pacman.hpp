#pragma once

#include "./Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

#include <memory>

#include <SFML/Graphics.hpp>

class Pacman
{
public:
  Pacman(float, float, sf::Vector2f);
  ~Pacman();

  void draw(std::shared_ptr<sf::RenderWindow>);
  void move(sf::Vector2f, sf::Time, const Labyrinth &);
  sf::Vector2f getPosition() const;
  float getRadius() const;

  // conversion methods so we don't have to write a getter for
  // the pacman shape when we use the collides(sf::Shape, sf::Shape)
  // method
  operator sf::Shape&() { return mPacman; }
  operator const sf::Shape&() const { return mPacman; }

private:
  int mGuys;
  float mSpeed;
  float mRadius;
  sf::CircleShape mPacman;
  sf::Time mDeltaTime;
  sf::Vector2f mPosition;
  Direction mDirection;
  std::shared_ptr<sf::RenderWindow> mPGameWindow;
};