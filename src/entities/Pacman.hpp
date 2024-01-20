#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

class Pacman
{
public:
  Pacman();
  ~Pacman();

  void move(sf::Vector2f, const Labyrinth &);

private:
  float m_speed;
  float m_radius;
  sf::CircleShape m_pacman;
  sf::Time m_deltaTime;
  sf::Vector2f m_movement;
  sf::Vector2f m_position;
  Direction m_direction;

  std::shared_ptr<sf::RenderWindow> m_pGameWindow;

};