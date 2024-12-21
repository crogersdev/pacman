#include "GameManager.hpp"
#include "../helpers/Collisions.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : mPacman((TILE_SIZE / 2) - 1, 200.f, sf::Vector2f(TILE_SIZE + 1.f, TILE_SIZE + 1.f)),
    // remember the order in the vector!
    //             sf::Vector2f(xval, yval)
    //             or
    //             sf::Vector2f(col,  row)
    mInky(  1.3f, sf::Vector2f(11.f * TILE_SIZE, 15.f * TILE_SIZE), sf::Color(255, 29,  33)),
    mBlinky(1.5f, sf::Vector2f(16.f * TILE_SIZE, 13.f * TILE_SIZE), sf::Color(117, 254, 255)),
    mPinky(  .9f, sf::Vector2f(16.f * TILE_SIZE, 15.f * TILE_SIZE), sf::Color(228, 160, 191)),
    mClyde( 1.1f, sf::Vector2f(11.f * TILE_SIZE, 13.f * TILE_SIZE), sf::Color(255, 179, 71)),
    mPaused(false),
    mClock(),
    mStateClock(),
    mWindowBounds(),
    mMousePos(),
    mGameHud(),
    mDebugHud(),
    mpGameWindow(pWindow),
    mLabyrinth(),
    mFps(60.0),
    mScore(0),
    mPelletValue(50),
    mDebugMode(false) {
  #ifndef NDEBUG
    mDebugMode = true;
  #else
    mDebugMode = false;
  #endif

  mpGameWindow->setFramerateLimit(mFps);
  mWindowBounds = sf::FloatRect(0, 0, mpGameWindow->getSize().x, mpGameWindow->getSize().y);

  auto pacmanPosition = sf::Vector2f(TILE_SIZE + 1, TILE_SIZE + 1);
  auto mPaused = false;

  mLabyrinth.set(pacmanPosition, Labyrinth::Tile::PACMAN);

  mKeyActions = {
    {sf::Keyboard::Left,  [&](sf::Time dt) { mPacman.move(sf::Vector2f(-1.f, 0.f), dt, mLabyrinth); }},
    {sf::Keyboard::Right, [&](sf::Time dt) { mPacman.move(sf::Vector2f( 1.f, 0.f), dt, mLabyrinth); }},
    {sf::Keyboard::Up,    [&](sf::Time dt) { mPacman.move(sf::Vector2f(0.f, -1.f), dt, mLabyrinth); }},
    {sf::Keyboard::Down,  [&](sf::Time dt) { mPacman.move(sf::Vector2f(0.f,  1.f), dt, mLabyrinth); }}
  };
}

GameManager::~GameManager() {}

void GameManager::handleInputs() {
  sf::Event event;

  while (mpGameWindow->pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      mpGameWindow->close();
    if (event.type == sf::Event::KeyPressed &&
          (event.key.code == sf::Keyboard::Escape ||
          (event.key.code == sf::Keyboard::C &&
            (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)))))
      mpGameWindow->close();
    if (event.type)
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
      mPaused = !mPaused;
    if (event.type == sf::Event::MouseMoved) {
      mMousePos = sf::Mouse::getPosition(*mpGameWindow);
    }
  }

  if (mPaused) return;

  auto dt = mClock.restart();

  for (const auto& pair : mKeyActions) {
    if (sf::Keyboard::isKeyPressed(pair.first)) {
      // note: this invokes the lambdas defined in
      //       the ctor that move pacman
      pair.second(dt);
    }
  }
}

void GameManager::updateEntities() {
  sf::Vector2f pacmanPosition = mPacman.getPosition();
  sf::Vector2f pacmanCenter = sf::Vector2f(
    pacmanPosition.x + (TILE_SIZE / 2), pacmanPosition.y + (TILE_SIZE / 2));

  mPinky.chase(mLabyrinth, pacmanCenter);
  mBlinky.chase(mLabyrinth, pacmanCenter);
  mInky.chase(mLabyrinth, pacmanCenter);
  mClyde.chase(mLabyrinth, pacmanCenter);

  // mPinky.meander(mLabyrinth);
  // mBlinky.meander(mLabyrinth);
  // mClyde.meander(mLabyrinth);
  // mInky.meander(mLabyrinth);

  if (entityCollides(mPinky, mPacman)) {
    mPinky.resetPath(mLabyrinth);
  }

  auto whatDidPacmanEat = mLabyrinth.at(
    mPacman.getPosition().x / TILE_SIZE,
    mPacman.getPosition().y / TILE_SIZE);

  if (whatDidPacmanEat == Labyrinth::Tile::PELLET) {
    mScore += mPelletValue;
    mLabyrinth.set(mPacman.getPosition(), Labyrinth::Tile::EMPTY);
  }
}

void GameManager::updateWindow() {
  sf::Time elapsed = mClock.restart();
  mFps = 1.f / elapsed.asSeconds();

  mpGameWindow->clear();
  std::ostringstream oss;
  oss << "Score: " << mScore << "\n";
  mGameHud.score.setString(oss.str());
  mpGameWindow->draw(mGameHud.score);
  mGameHud.drawGuys(mpGameWindow);
  oss.clear();

  // Update debug information
  if (mDebugMode) {
    std::ostringstream debugOss;
    auto pacmanRow = floor(mPacman.getPosition().y / TILE_SIZE);
    auto pacmanCol = floor(mPacman.getPosition().x / TILE_SIZE);
    auto tileInfoEnum = mLabyrinth.at(pacmanCol, pacmanRow);
    auto tileInfo = mLabyrinth.mTileLabelLut.at(tileInfoEnum);
    debugOss << "Pacman\n";
    debugOss << "    pos r,c: " << pacmanRow << ", " << pacmanCol << "\n";
    debugOss << "    offset:  " << mLabyrinth.getOffset(pacmanCol, pacmanRow) << "\n";
    auto pair = mLabyrinth.getPairFromOffset(mLabyrinth.getOffset(pacmanCol, pacmanRow));
    debugOss << "    pair:    " << pair.second << ", " << pair.first << "\n";
    auto o = mLabyrinth.getOffset(pair);
    debugOss << "    offset2: " << o << "\n";
    debugOss << "    tile:    " << tileInfo << "\n";

    auto pinkyRow = floor(mPinky.getPosition().y / TILE_SIZE);
    auto pinkyCol = floor(mPinky.getPosition().x / TILE_SIZE);
    auto pinkyOffset = mLabyrinth.getOffset(pinkyCol, pinkyRow);
    debugOss << "Pinky\n";
    debugOss << "    pos r,c: " << pinkyRow << ", " << pinkyCol << "\n";
    debugOss << "    offset : " << pinkyOffset << "\n";
    debugOss << "\nghost neighbors at offset " << pinkyOffset;
    debugOss << "\n        ";
    for (auto n : mLabyrinth.getNeighbors(pinkyOffset)) {
      debugOss << n << ", ";
    }
    debugOss << "\n";

    auto trRow = floor(mPinky.getTarget().y / TILE_SIZE);
    auto trCol = floor(mPinky.getTarget().x / TILE_SIZE);
    debugOss << "    chasing r, c: " << trRow << ", " << trCol << "\n";
    debugOss << "          offset: " << mLabyrinth.getOffset(trCol, trRow);

    auto path = mPinky.getPath();
    debugOss << "\npath: \n";
    int count = 0;
    for (auto p : path) {
      auto foo = mLabyrinth.getOffset(p);
      debugOss << foo << ", ";
      count++;
      if (count == 8) {
        count = 0;
        debugOss << "\n";
      }
    }

    debugOss << "\n\n\n";
    debugOss << "state clock: " << mStateClock.getElapsedTime().asSeconds() << "\n";

    debugOss << "\n\n";
    debugOss << "mouse position r, c: (" << mMousePos.y / TILE_SIZE << ", " << mMousePos.x / TILE_SIZE << ")\n";
    debugOss << "      offset       :  " << mLabyrinth.getOffset(mMousePos.y, mMousePos.x) << "\n";
    debugOss << "      map lut info :  " << mLabyrinth.mTileLabelLut.at(mLabyrinth.at(mMousePos.y, mMousePos.x));
    debugOss << "\n";

    mDebugHud.debugText.setString(debugOss.str());
    mpGameWindow->draw(mDebugHud.debugText);
  }

  mLabyrinth.draw(mpGameWindow);
  mPacman.draw(mpGameWindow);
  mPinky.draw(mpGameWindow);
  mInky.draw(mpGameWindow);
  mBlinky.draw(mpGameWindow);
  mClyde.draw(mpGameWindow);
  mpGameWindow->display();
}
