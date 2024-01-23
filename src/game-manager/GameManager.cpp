#include "GameManager.hpp"
#include "../helpers/Collisions.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pinky(pWindow, 400.f),
    m_pacman(pWindow, (TILE_SIZE / 2) - 1, 200.f, sf::Vector2f(TILE_SIZE + 1.f, TILE_SIZE + 1.f)),
    m_clock(),
    m_deltaTime(),
    m_pWindow(pWindow),
    m_labyrinth(),
    m_fps(60.0)
{
  m_pWindow->setFramerateLimit(60);
  m_windowBounds = sf::FloatRect(0, 0, m_pWindow->getSize().x, m_pWindow->getSize().y);

  auto pacmanPosition = sf::Vector2f(m_tileSizeX + 1, m_tileSizeY + 1);

  m_labyrinth.set(pacmanPosition, Labyrinth::PACMAN);

  m_debugFont.loadFromFile("./res/PublicPixel.ttf");
  m_debugText.setFont(m_debugFont);
  m_debugText.setCharacterSize(20);
  m_debugText.setFillColor(sf::Color::White);
  m_debugText.setPosition(10.f, TILE_SIZE * LABYRINTH_ROWS);

  m_wallTile = sf::RectangleShape(sf::Vector2f(m_tileSizeX, m_tileSizeY));
  m_wallTile.setFillColor(sf::Color::Blue);

  m_keyActions = {
    {sf::Keyboard::Left,  [&](sf::Time dt) { m_pacman.move(sf::Vector2f(-1.f, 0.f), dt, m_labyrinth); }},
    {sf::Keyboard::Right, [&](sf::Time dt) { m_pacman.move(sf::Vector2f( 1.f, 0.f), dt, m_labyrinth); }},
    {sf::Keyboard::Up,    [&](sf::Time dt) { m_pacman.move(sf::Vector2f(0.f, -1.f), dt, m_labyrinth); }},
    {sf::Keyboard::Down,  [&](sf::Time dt) { m_pacman.move(sf::Vector2f(0.f,  1.f), dt, m_labyrinth); }}
  };
}

GameManager::~GameManager() {}

void GameManager::handleInputs()
{
  sf::Event event;

  while (m_pWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      m_pWindow->close();
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
      m_pWindow->close();
  }

  m_deltaTime = m_clock.restart();

  for (const auto& pair : m_keyActions)
  {
    if (sf::Keyboard::isKeyPressed(pair.first))
    {
      // note: this invokes the lambdas defined in
      //       the ctor that move pacman
      pair.second(m_deltaTime);
    }
  }
}

void GameManager::updateEntities()
{
  m_pinky.meander(m_labyrinth);

  if (entityCollides(m_pinky, m_pacman))
    std::cout << "YOU AND I COLLIDE\n";
}

void GameManager::updateWindow()
{
  m_pWindow->clear();

  for (int row = 0; row < m_labyrinth.m_labyrinthRows; ++row)
  {
    for (int col = 0; col < m_labyrinth.m_labyrinthCols; ++col)
    {
      auto tile = m_labyrinth.at(col, row);
      switch(tile) {
        case m_labyrinth.WALL:
          m_wallTile.setPosition(sf::Vector2f(m_tileSizeX*col, m_tileSizeY*row));
          m_pWindow->draw(m_wallTile);
        case m_labyrinth.GATE:
        case m_labyrinth.PELLET:
        case m_labyrinth.POWERUP:
          break;
        default:
          break;
      }
    }
  }

  sf::Time elapsed = m_clock.restart();
  m_fps = 1.f / elapsed.asSeconds();

  // Update debug information
  std::ostringstream oss;
  oss << "FPS: " << m_fps << "\n";
  auto row = floor(static_cast<int>(m_pacman.getPosition().y) / m_tileSizeY);
  auto col = floor(static_cast<int>(m_pacman.getPosition().x) / m_tileSizeX);
  oss << "Row: " << row << "  Col: " << col << "\n";
  oss << "Map LUT at " << row << ", " << col << ": " << m_labyrinth.at(row, col) << "\n";
  m_debugText.setString(oss.str());

  m_pWindow->draw(m_debugText);

  m_pacman.draw();
  m_pinky.draw();
  m_pWindow->display();
}
