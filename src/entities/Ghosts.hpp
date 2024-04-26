#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <random>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

class Ghost
{
public:
  // NOTE: apparently single arg ctors _should_ be explicit.
  explicit Ghost(float);
  ~Ghost();

  float m_meanderOdds;

  void chase();
  void changeDirection(Direction);
  void draw(std::shared_ptr<sf::RenderWindow>);
  sf::Vector2f getPosition();
  void meander(const Labyrinth &);
  void scatter();

  // conversion methods so we don't have to write a getter for
  // the ghost shape when we use the collides(sf::Shape, sf::Shape)
  // method
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
  sf::Vector2f m_movement;
  sf::Vector2f m_position;
  Direction m_direction;
  std::shared_ptr<sf::RenderWindow> m_pGameWindow;
  State m_state;
};