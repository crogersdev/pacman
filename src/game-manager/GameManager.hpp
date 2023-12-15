#pragma once

#include <SFML/Graphics.hpp>

#include <functional>
#include <memory>
#include <unordered_map>

#include "../entities/Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

const float TILE_SIZE = 50.f;
const float PACMAN_RADIUS = (TILE_SIZE / 2) - 1;

class GameManager
{
private:
  sf::Clock m_clock;
  sf::FloatRect m_windowBounds;
  sf::Font m_debugFont;
  sf::Text m_debugText;
  sf::Time m_deltaTime;
  sf::Vector2f m_initialPosition;
  std::shared_ptr<sf::RenderWindow> m_pWindow;
  std::vector<sf::RectangleShape> m_walls;
  Labyrinth m_labyrinth;

  float m_fps;

  const float m_movementSpeed = 200.f;
  const float m_pacmanRadius = PACMAN_RADIUS;
  const float m_tileSizeX = TILE_SIZE;
  const float m_tileSizeY = TILE_SIZE;

public:
  explicit GameManager(std::shared_ptr<sf::RenderWindow> pWindow);
  ~GameManager();

  std::unordered_map<sf::Keyboard::Key, std::function<void()>> m_keyActions;
  sf::CircleShape m_pacman;
  sf::RectangleShape m_wallTile;

  void drawLabyrinth();
  void handleInputs();
  void movePacman(sf::Vector2f);
  void updateWindow();
};