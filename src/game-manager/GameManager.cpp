#include "GameManager.hpp"
#include "../helpers/Collisions.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pinky(pWindow, 400.f),
    m_clock(),
    m_deltaTime(),
    m_pWindow(pWindow),
    m_labyrinth(Labyrinth()),
    m_fps(60.0)
{
  m_pWindow->setFramerateLimit(60);
  m_windowBounds = sf::FloatRect(0, 0, m_pWindow->getSize().x, m_pWindow->getSize().y);

  m_initialPosition = sf::Vector2f(m_tileSizeX + 1, m_tileSizeY + 1);
  m_pacman = sf::CircleShape(m_pacmanRadius);
  m_pacman.setFillColor(sf::Color::Yellow);
  m_pacman.setPosition(m_initialPosition);

  m_labyrinth.set(m_initialPosition, Labyrinth::PACMAN);

  m_debugFont.loadFromFile("./res/PublicPixel.ttf");
  m_debugText.setFont(m_debugFont);
  m_debugText.setCharacterSize(20);
  m_debugText.setFillColor(sf::Color::White);
  m_debugText.setPosition(10.f, TILE_SIZE * LABYRINTH_ROWS);

  m_wallTile = sf::RectangleShape(sf::Vector2f(m_tileSizeX, m_tileSizeY));
  m_wallTile.setFillColor(sf::Color::Blue);

  m_keyActions = {
    {sf::Keyboard::Left,  [&]() { movePacman(sf::Vector2f(-m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
    {sf::Keyboard::Right, [&]() { movePacman(sf::Vector2f( m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
    {sf::Keyboard::Up,    [&]() { movePacman(sf::Vector2f(0, -m_movementSpeed * m_deltaTime.asSeconds())); }},
    {sf::Keyboard::Down,  [&]() { movePacman(sf::Vector2f(0,  m_movementSpeed * m_deltaTime.asSeconds())); }}
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
      pair.second();
    }
  }
}

void GameManager::updateEntities()
{
  m_pinky.meander(m_clock, m_labyrinth);
}

void GameManager::movePacman(sf::Vector2f movement)
{
  const float radius = m_pacman.getRadius();
  sf::Vector2f newPosition = m_pacman.getPosition() + movement;
  
  wrapCoordinate(newPosition.x, -radius * 2, m_windowBounds.width);
  wrapCoordinate(newPosition.y, -radius * 2, m_windowBounds.height);

  auto pacmanWidth = (m_pacman.getRadius() * 2.f) - 1.f;
  bool wallCollision = wallCollides(
    newPosition,
    sf::Vector2f(pacmanWidth, pacmanWidth),
    m_labyrinth
  );

  // TRICKY: we avoid .move(movement) here because doing so would ignore
  //         the arithmetic we implemented to wrap pacman around the edges
  if (!wallCollision) 
    m_pacman.setPosition(newPosition);
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

  m_pWindow->draw(m_pacman);
  m_pinky.draw();
  m_pWindow->display();
}
