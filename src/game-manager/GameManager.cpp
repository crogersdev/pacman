#include "GameManager.hpp"
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
  m_pacman.setPosition(m_initialPosition);

  m_collisionTiles.push_back(sf::RectangleShape(sf::Vector2f(m_tileSizeX, m_tileSizeY)));
  m_collisionTiles.back().setPosition(m_initialPosition);
  m_collisionTiles.back().setFillColor(sf::Color::Red);

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
      //       the ctor that move pacman
      pair.second();
    }
  }
}

void GameManager::movePacman(sf::Vector2f movement)
{
  const float radius = m_pacman.getRadius();
  sf::Vector2f newPosition = m_pacman.getPosition() + movement;

  wrapCoordinate(newPosition.x, -radius * 2, m_windowBounds.width);
  wrapCoordinate(newPosition.y, -radius * 2, m_windowBounds.height);

  auto upperLeftCollision  = tileCoordsAtPosition(sf::Vector2f(newPosition.x, newPosition.y));
  auto upperRightCollision = tileCoordsAtPosition(sf::Vector2f(newPosition.x + PACMAN_RADIUS*2, newPosition.y));
  auto lowerLeftCollision  = tileCoordsAtPosition(sf::Vector2f(newPosition.x, newPosition.y + PACMAN_RADIUS*2));
  auto lowerRightCollision = tileCoordsAtPosition(sf::Vector2f(newPosition.x + PACMAN_RADIUS*2, newPosition.y + PACMAN_RADIUS*2));

  m_collisionTiles.clear();

  auto foo = sf::RectangleShape(sf::Vector2f(50, 50));
  foo.setFillColor(sf::Color::Cyan);
  foo.setPosition(upperLeftCollision);
  m_collisionTiles.push_back(foo);

  auto bar = sf::RectangleShape(sf::Vector2f(50, 50));
  bar.setFillColor(sf::Color(144, 12, 200));
  bar.setPosition(upperRightCollision);
  m_collisionTiles.push_back(bar);

  auto baz = sf::RectangleShape(sf::Vector2f(50, 50));
  baz.setFillColor(sf::Color(128, 0, 22));
  baz.setPosition(lowerLeftCollision);
  m_collisionTiles.push_back(baz);

  auto puke = sf::RectangleShape(sf::Vector2f(50, 50));
  puke.setFillColor(sf::Color::Magenta);
  puke.setPosition(lowerRightCollision);
  m_collisionTiles.push_back(puke);

  // TRICKY: we avoid .move(movement) here because doing so would ignore
  //         the arithmetic we implemented to wrap pacman around the edges
  m_pacman.setPosition(newPosition);
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
