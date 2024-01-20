#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <random>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

class Ghost
{
public:
  Ghost(std::shared_ptr<sf::RenderWindow>, float);
  ~Ghost();

  float m_meanderOdds;

  void chase();
  void changeDirection(Direction);
  void draw();
  sf::Vector2f getPosition();
  void meander(sf::Clock &, const Labyrinth &);
  //void scatter();

  // conversion methods so we don't have to write a getter for
  // the ghost shape when we use the collides(sf::Shape, sf::Shape)
  operator sf::Shape&() { return m_ghostShape; }
  operator const sf::Shape&() const { return m_ghostShape; }

private:
  enum State {
    SCATTER    = 1,
    MEANDER    = 2,
    CHASE      = 3,
    FRIGHTENED = 4, 
    WAIT       = 5 
  };

  unsigned m_seed;
  std::mt19937 m_randGenerator;

  float m_speed;
  //sf::Sprite m_ghost;
  sf::RectangleShape m_ghostShape;
  //const sf::Texture m_spriteSheet;
  sf::Time m_deltaTime;
  sf::Vector2f m_movement;
  sf::Vector2f m_position;
  Direction m_direction;
  std::shared_ptr<sf::RenderWindow> m_pGameWindow;
  State m_state;
};