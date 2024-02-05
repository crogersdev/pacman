#include "GameManager.hpp"
#include "../helpers/Collisions.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pacman((TILE_SIZE / 2) - 1, 200.f, sf::Vector2f(TILE_SIZE + 1.f, TILE_SIZE + 1.f)),
    m_pinky(400.f),
    m_clock(),
    m_deltaTime(),
    m_pGameWindow(pWindow),
    m_labyrinth(),
    m_fps(60.0)
{
  m_pGameWindow->setFramerateLimit(60);
  m_windowBounds = sf::FloatRect(0, 0, m_pGameWindow->getSize().x, m_pGameWindow->getSize().y);

  auto pacmanPosition = sf::Vector2f(m_tileSizeX + 1, m_tileSizeY + 1);

  m_labyrinth.set(pacmanPosition, Labyrinth::PACMAN);

  m_debugFont.loadFromFile("./res/PublicPixel.ttf");
  m_debugText.setFont(m_debugFont);
  m_debugText.setCharacterSize(20);
  m_debugText.setFillColor(sf::Color::White);
  m_debugText.setPosition(10.f, TILE_SIZE * LABYRINTH_ROWS);

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

  while (m_pGameWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      m_pGameWindow->close();
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
      m_pGameWindow->close();
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
  
  auto row = floor(static_cast<int>(m_pacman.getPosition().y + (TILE_SIZE / 2.f)) / m_tileSizeY);
  auto col = floor(static_cast<int>(m_pacman.getPosition().x + (TILE_SIZE / 2.f)) / m_tileSizeX);
  auto whatDidPacmanEat = m_labyrinth.at(col, row);
  if (whatDidPacmanEat == Labyrinth::PELLET)
  {
    m_labyrinth.set(row, col, Labyrinth::EMPTY);
  }
}

void GameManager::updateWindow()
{
  sf::Time elapsed = m_clock.restart();
  m_fps = 1.f / elapsed.asSeconds();

  // Update debug information
  std::ostringstream oss;
  oss << "FPS: " << m_fps << "\n";
  auto row = floor(static_cast<int>(m_pacman.getPosition().y) / m_tileSizeY);
  auto col = floor(static_cast<int>(m_pacman.getPosition().x) / m_tileSizeX);
  oss << "Row: " << row << "  Col: " << col << "\n";
  auto bar = m_labyrinth.at(col, row);
  auto foo = m_labyrinth.m_tileLabelLut.at(bar);
  oss << "Map LUT at " << row << ", " << col << ": " << foo << "\n";
  m_debugText.setString(oss.str());

  m_pGameWindow->clear();
  m_pGameWindow->draw(m_debugText);
  m_labyrinth.draw(m_pGameWindow);
  m_pacman.draw(m_pGameWindow);
  m_pinky.draw(m_pGameWindow);
  m_pGameWindow->display();
}
