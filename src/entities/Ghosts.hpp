#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

class Ghost
{
public:
  Ghost(std::shared_ptr<sf::RenderWindow>, float);
  ~Ghost();

  //void chase();
  void meander(sf::Clock &, const Labyrinth &);
  //void scatter();
  void draw();

private:
  enum State {
    SCATTER    = 1,
    MEANDER    = 2,
    CHASE      = 3,
    FRIGHTENED = 4, 
    WAIT       = 5 
  };

  float m_speed;
  //sf::Sprite m_ghost;
  sf::RectangleShape m_ghostShape;
  //const sf::Texture m_spriteSheet;
  sf::Time m_deltaTime;
  sf::Vector2f m_movement;
  sf::Vector2f m_position;
  std::shared_ptr<sf::RenderWindow> m_pGameWindow;
  State m_state;

  // i want a direction so i can decide
  // which sprite to render

  
};