#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

class Pacman
{
public:
  Pacman(std::shared_ptr<sf::RenderWindow>, float, float, sf::Vector2f);
  ~Pacman();

  void draw();
  //void move(sf::Vector2f, sf::Clock &, const Labyrinth &);
  void move(sf::Vector2f, sf::Time, const Labyrinth &);
  sf::Vector2f getPosition();

  // conversion methods so we don't have to write a getter for
  // the pacman shape when we use the collides(sf::Shape, sf::Shape)
  // method
  operator sf::Shape&() { return m_pacman; }
  operator const sf::Shape&() const { return m_pacman; }

private:
  float m_speed;
  float m_radius;
  sf::CircleShape m_pacman;
  sf::Time m_deltaTime;
  sf::Vector2f m_position;
  Direction m_direction;
  std::shared_ptr<sf::RenderWindow> m_pGameWindow;
};