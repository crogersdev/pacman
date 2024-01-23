#pragma once

#include <SFML/Graphics.hpp>

#include <functional>
#include <memory>

#include "../entities/Ghosts.hpp"
#include "../entities/Labyrinth.hpp"
#include "../entities/Pacman.hpp"

class GameManager
{
public:
  // NOTE: apparently single arg ctors _should_ be explicit.
  explicit GameManager(std::shared_ptr<sf::RenderWindow> pWindow);
  ~GameManager();

  std::map<sf::Keyboard::Key, std::function<void(sf::Time)>> m_keyActions;
  
  // TODO: should these guys be private?
  Pacman m_pacman;
  Ghost m_pinky;
  sf::RectangleShape m_wallTile;

  void drawLabyrinth();
  void handleInputs();
  void updateEntities();
  void updateWindow();

private:
  sf::Clock m_clock;
  sf::FloatRect m_windowBounds;
  sf::Font m_debugFont;
  sf::Text m_debugText;
  sf::Time m_deltaTime;
  std::shared_ptr<sf::RenderWindow> m_pWindow;
  Labyrinth m_labyrinth;

  float m_fps;

  const float m_tileSizeX = TILE_SIZE;
  const float m_tileSizeY = TILE_SIZE;

};