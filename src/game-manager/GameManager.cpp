#include "GameManager.hpp"
#include "../helpers/Collisions.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pacman((TILE_SIZE / 2) - 1, 200.f, sf::Vector2f(TILE_SIZE + 1.f, TILE_SIZE + 1.f)),
    m_pinky(400.f),
    m_clock(),
    m_windowBounds(),
    m_hud(),
    m_deltaTime(),
    m_pGameWindow(pWindow),
    m_labyrinth(),
    m_fps(60.0),
    m_score(0),
    m_pelletValue(50),
    m_debugMode(false)

{
  #ifndef NDEBUG
    m_debugMode = true;
  #else
    m_debugMode = false;
  #endif

  m_pGameWindow->setFramerateLimit(m_fps);
  m_windowBounds = sf::FloatRect(0, 0, m_pGameWindow->getSize().x, m_pGameWindow->getSize().y);

  auto pacmanPosition = sf::Vector2f(TILE_SIZE + 1, TILE_SIZE + 1);

  m_labyrinth.set(pacmanPosition, Labyrinth::PACMAN);

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

  for (const auto& pair : m_keyActions) {
    if (sf::Keyboard::isKeyPressed(pair.first)) {
      // note: this invokes the lambdas defined in
      //       the ctor that move pacman
      pair.second(m_deltaTime);
    }
  }
}

void GameManager::updateEntities() {
  //  m_pinky.meander(m_labyrinth);
  m_pinky.chase(m_labyrinth, m_pacman.getPosition());

  if (entityCollides(m_pinky, m_pacman))
    std::cout << "YOU AND I COLLIDE\n";
  
  auto row = floor(static_cast<int>(m_pacman.getPosition().y + (TILE_SIZE / 2.f)) / TILE_SIZE);
  auto col = floor(static_cast<int>(m_pacman.getPosition().x + (TILE_SIZE / 2.f)) / TILE_SIZE);
  
  auto whatDidPacmanEat = m_labyrinth.at(col, row);
  if (whatDidPacmanEat == Labyrinth::PELLET) {
    m_score += m_pelletValue;
    m_labyrinth.set(row, col, Labyrinth::EMPTY);
  }
}

void GameManager::updateWindow() {
  sf::Time elapsed = m_clock.restart();
  m_fps = 1.f / elapsed.asSeconds();

  m_pGameWindow->clear();
  std::ostringstream oss;
  oss << "Score: " << m_score << "\n";
  m_hud.score.setString(oss.str());
  m_pGameWindow->draw(m_hud.score);
  m_hud.drawGuys(m_pGameWindow);
  oss.clear();

  // Update debug information
  if (m_debugMode) {
    oss << "FPS: " << m_fps << "\n";
    auto row = std::floor(static_cast<int>(m_pacman.getPosition().y) / TILE_SIZE);
    auto col = std::floor(static_cast<int>(m_pacman.getPosition().x) / TILE_SIZE);
    oss << "Row: " << row << "  Col: " << col << "\n";
    auto bar = m_labyrinth.at(col, row);
    auto foo = m_labyrinth.m_tileLabelLut.at(bar);
    oss << "Map LUT at " << row << ", " << col << ": " << foo << "\n";
    m_pGameWindow->draw(m_hud.debugText);
    m_hud.debugText.setString(oss.str());
  }

  m_labyrinth.draw(m_pGameWindow);
  m_pacman.draw(m_pGameWindow);
  m_pinky.draw(m_pGameWindow);
  m_pGameWindow->display();
}
