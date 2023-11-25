#include "GameManager.hpp"
#include "../entities/Labyrinth.hpp"
#include <iostream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pWindow(std::move(pWindow)),
    m_labyrinth(Labyrinth())
{
  m_pWindow->setFramerateLimit(60);
  m_windowBounds = sf::FloatRect(0, 0, m_pWindow->getSize().x, m_pWindow->getSize().y);

  m_initialPosition = sf::Vector2f(m_tileSizeX, m_tileSizeY);
  m_pacman = sf::CircleShape(m_pacmanRadius);
  m_pacman.setFillColor(sf::Color::Yellow);
  m_pacman.setPosition(m_tileSizeX, m_tileSizeY);

  for (int i = 0; i < 4; ++i) {
    m_collisionTiles.push_back(sf::RectangleShape(sf::Vector2f(m_tileSizeX, m_tileSizeY)));
    m_collisionTiles.back().setPosition(m_initialPosition);
    m_collisionTiles.back().setFillColor(sf::Color::Red);
  }

  m_wallTile = sf::RectangleShape(sf::Vector2f(m_tileSizeX, m_tileSizeY));
  m_wallTile.setFillColor(sf::Color::Blue);

  m_keyActions = {
    {sf::Keyboard::Left,  [&]() { movePacman(sf::Vector2f(-m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
    {sf::Keyboard::Right, [&]() { movePacman(sf::Vector2f( m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
    {sf::Keyboard::Up,  [&]() { movePacman(sf::Vector2f(0, -m_movementSpeed * m_deltaTime.asSeconds())); }},
    {sf::Keyboard::Down,  [&]() { movePacman(sf::Vector2f(0,  m_movementSpeed * m_deltaTime.asSeconds())); }}
  };
}

GameManager::~GameManager() {}

void GameManager::handleInputs() {
  sf::Event event;

  while (m_pWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      m_pWindow->close();
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
      m_pWindow->close();
  }

  m_deltaTime = m_clock.restart();

  for (const auto& pair : m_keyActions) {
    if (sf::Keyboard::isKeyPressed(pair.first)) {
      // note: this invokes the lambdas defined in
      //   the ctor that move pacman
      pair.second();
    }
  }
}

void GameManager::movePacman(sf::Vector2f movement)
{
  const float radius = m_pacman.getRadius();
  sf::Vector2f newPosition = m_pacman.getPosition() + movement;

  auto wrapCoordinate = [](float &coord, float min, float max) {
    if (coord < min)
      coord = max;
    else if (coord > max)
      coord = min;
  };

  wrapCoordinate(newPosition.x, -radius * 2, m_windowBounds.width);
  wrapCoordinate(newPosition.y, -radius * 2, m_windowBounds.height);

  auto upperLeftCollision = sf::Vector2f(
    (int(newPosition.x)/int(m_tileSizeX))*m_tileSizeX, (int(newPosition.y)/int(m_tileSizeY))*m_tileSizeY
  );

  auto bottomLeftCollision = sf::Vector2f(
    (int(newPosition.x)/int(m_tileSizeX))*m_tileSizeX, (int(newPosition.y)/int(m_tileSizeY))*m_tileSizeY+m_tileSizeY
  );

  auto upperRightCollision = sf::Vector2f(
    ((int(newPosition.x)/int(m_tileSizeX))*m_tileSizeX)+m_tileSizeX, (int(newPosition.y)/int(m_tileSizeY))*m_tileSizeY
  );

  auto bottomRightCollision = sf::Vector2f(
    ((int(newPosition.x)/int(m_tileSizeX))*m_tileSizeX)+m_tileSizeX, (int(newPosition.y)/int(m_tileSizeY))*m_tileSizeY+m_tileSizeY
  );

  m_collisionTiles.at(0).setPosition(upperLeftCollision);
  m_collisionTiles.at(1).setPosition(bottomLeftCollision);
  m_collisionTiles.at(2).setPosition(upperRightCollision);
  m_collisionTiles.at(3).setPosition(bottomRightCollision);
 
  // TRICKY: we avoid .move(movement) here because doing so would ignore
  //   the arithmetic we implemented to wrap pacman around the edges
  m_pacman.setPosition(newPosition);
  std::cout<<"(" << newPosition.x << ", " << newPosition.y << ")\n";
}

void GameManager::updateWindow()
{
  m_pWindow->clear();

  for (int row = 0; row < m_labyrinth.m_labyrinthRows; ++row)
  {
    for (int col = 0; col < m_labyrinth.m_labyrinthCols; ++col)
    {
      if (m_labyrinth.at(col, row) == m_labyrinth.WALL) {
        m_wallTile.setPosition(sf::Vector2f(m_tileSizeX*col, m_tileSizeY*row));
        m_walls.push_back(m_wallTile);
        m_pWindow->draw(m_wallTile);
      }
    }
  }

  for (auto tile: m_collisionTiles) {
    m_pWindow->draw(tile);
  }

  m_pWindow->draw(m_pacman);
  m_pWindow->display();
}
