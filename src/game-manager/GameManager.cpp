#include "GameManager.hpp"
#include "../helpers/Collisions.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pacman((TILE_SIZE / 2) - 1, 200.f, sf::Vector2f(TILE_SIZE + 1.f, TILE_SIZE + 1.f)),
    // remember the order in the vector!
    //             sf::Vector2f(xval, yval)
    //             or
    //             sf::Vector2f(col,  row)
    m_pinky(  .9f, sf::Vector2f(16.f * TILE_SIZE, 15.f * TILE_SIZE), sf::Color(228, 160, 191)),
    m_inky(  1.3f, sf::Vector2f(11.f * TILE_SIZE, 15.f * TILE_SIZE), sf::Color(255, 29,  33)),
    m_blinky(1.5f, sf::Vector2f(16.f * TILE_SIZE, 13.f * TILE_SIZE), sf::Color(117, 254, 255)),
    m_clyde( 1.1f, sf::Vector2f(11.f * TILE_SIZE, 13.f * TILE_SIZE), sf::Color(255, 179, 71)),
    m_clock(),
    m_windowBounds(),
    m_gameHud(),
    m_debugHud(),
    m_deltaTime(),
    m_pGameWindow(pWindow),
    m_labyrinth(),
    m_fps(60.0),
    m_score(0),
    m_pelletValue(50),
    mMousePos(),
    mPaused(false),
    m_debugMode(false) {
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

void GameManager::handleInputs() {
  sf::Event event;

  while (m_pGameWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      m_pGameWindow->close();
    if (event.type == sf::Event::KeyPressed &&
          (event.key.code == sf::Keyboard::Escape ||
          (event.key.code == sf::Keyboard::C &&
            (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)))))
      m_pGameWindow->close();
    if (event.type)
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
      mPaused = !mPaused;
    if (event.type == sf::Event::MouseMoved) {
      mMousePos = sf::Mouse::getPosition(*m_pGameWindow);
    }
  }

  if (mPaused) return;

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
  sf::Vector2f pacmanPosition = m_pacman.getPosition();
  sf::Vector2f pacmanCenter = sf::Vector2f(
    pacmanPosition.x + (TILE_SIZE / 2), pacmanPosition.y + (TILE_SIZE / 2));

  m_pinky.chase(m_labyrinth, pacmanCenter);
  m_blinky.chase(m_labyrinth, pacmanCenter);
  m_inky.chase(m_labyrinth, pacmanCenter);
  m_clyde.chase(m_labyrinth, pacmanCenter);

  // m_pinky.meander(m_labyrinth);
  // m_blinky.meander(m_labyrinth);
  // m_clyde.meander(m_labyrinth);
  // m_inky.meander(m_labyrinth);

  if (entityCollides(m_pinky, m_pacman)) {
    m_pinky.resetPath(m_labyrinth);
  }

  auto whatDidPacmanEat = m_labyrinth.at(
    m_pacman.getPosition().x / TILE_SIZE,
    m_pacman.getPosition().y / TILE_SIZE);

  if (whatDidPacmanEat == Labyrinth::PELLET) {
    m_score += m_pelletValue;
    m_labyrinth.set(m_pacman.getPosition(), Labyrinth::EMPTY);
  }
}

void GameManager::updateWindow() {
  sf::Time elapsed = m_clock.restart();
  m_fps = 1.f / elapsed.asSeconds();

  m_pGameWindow->clear();
  std::ostringstream oss;
  oss << "Score: " << m_score << "\n";
  m_gameHud.score.setString(oss.str());
  m_pGameWindow->draw(m_gameHud.score);
  m_gameHud.drawGuys(m_pGameWindow);
  oss.clear();

  // Update debug information
  if (m_debugMode) {
    std::ostringstream debugOss;
    auto pacmanRow = floor(m_pacman.getPosition().y / TILE_SIZE);
    auto pacmanCol = floor(m_pacman.getPosition().x / TILE_SIZE);
    auto tileInfoEnum = m_labyrinth.at(pacmanCol, pacmanRow);
    auto tileInfo = m_labyrinth.m_tileLabelLut.at(tileInfoEnum);
    debugOss << "Pacman\n";
    debugOss << "    pos r,c: " << pacmanRow << ", " << pacmanCol << "\n";
    debugOss << "    offset:  " << m_labyrinth.getOffset(pacmanCol, pacmanRow) << "\n";
    auto pair = m_labyrinth.getPairFromOffset(m_labyrinth.getOffset(pacmanCol, pacmanRow));
    debugOss << "    pair:    " << pair.second << ", " << pair.first << "\n";
    auto o = m_labyrinth.getOffset(pair);
    debugOss << "    offset2: " << o << "\n";
    debugOss << "    tile:    " << tileInfo << "\n";

    auto pinkyRow = floor(m_pinky.getPosition().y / TILE_SIZE);
    auto pinkyCol = floor(m_pinky.getPosition().x / TILE_SIZE);
    auto pinkyOffset = m_labyrinth.getOffset(pinkyCol, pinkyRow);
    debugOss << "Pinky\n";
    debugOss << "    pos r,c: " << pinkyRow << ", " << pinkyCol << "\n";
    debugOss << "    offset : " << pinkyOffset << "\n";
    debugOss << "\nghost neighbors at offset " << pinkyOffset;
    debugOss << "\n        ";
    for (auto n : m_labyrinth.getNeighbors(pinkyOffset)) {
      debugOss << n << ", ";
    }
    debugOss << "\n";

    auto trRow = floor(m_pinky.getTarget().y / TILE_SIZE);
    auto trCol = floor(m_pinky.getTarget().x / TILE_SIZE);
    debugOss << "    chasing r, c: " << trRow << ", " << trCol << "\n";
    debugOss << "          offset: " << m_labyrinth.getOffset(trCol, trRow);

    auto path = m_pinky.getPath();
    debugOss << "\npath: \n";
    int count = 0;
    for (auto p : path) {
      auto foo = m_labyrinth.getOffset(p);
      debugOss << foo << ", ";
      count++;
      if (count == 8) {
        count = 0;
        debugOss << "\n";
      }
    }

    debugOss << "\n\n";
    debugOss << "mouse position r, c: (" << mMousePos.y / TILE_SIZE << ", " << mMousePos.x / TILE_SIZE << ")\n";
    debugOss << "      offset       :  " << m_labyrinth.getOffset(mMousePos.y, mMousePos.x) << "\n";
    debugOss << "      map lut info :  " << m_labyrinth.m_tileLabelLut.at(m_labyrinth.at(mMousePos.y, mMousePos.x));
    debugOss << "\n";

    m_debugHud.debugText.setString(debugOss.str());
    m_pGameWindow->draw(m_debugHud.debugText);
  }

  m_labyrinth.draw(m_pGameWindow);
  m_pacman.draw(m_pGameWindow);
  m_pinky.draw(m_pGameWindow);
  m_inky.draw(m_pGameWindow);
  m_blinky.draw(m_pGameWindow);
  m_clyde.draw(m_pGameWindow);
  m_pGameWindow->display();
}
