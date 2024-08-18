#include <chrono>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

#include "./Ghosts.hpp"
#include "../helpers/Collisions.hpp"

Ghost::Ghost(float speed, bool debugMode)
    : mDebugMode(debugMode),
      mMeanderOdds(66.6),
      mSpeed(speed),
      mGhostShape(sf::Vector2f(25.f, 25.f)),
      mDirection(sf::Vector2f(1.f, 0.f)),
      // mInitialPosition(sf::Vector2f(12.f * TILE_SIZE, 8.f * TILE_SIZE)),
      mInitialPosition(sf::Vector2f(9.f * TILE_SIZE, 6.f * TILE_SIZE)),
      mPath(),
      mState(MEANDER) {
  mSeed = std::chrono::system_clock::now().time_since_epoch().count();
  mRandGenerator = std::mt19937(mSeed);
  mGhostShape.setFillColor(sf::Color(219, 48, 130));
  mGhostShape.setPosition(mInitialPosition);

#ifndef NDEBUG
  mDebugMode = true;
#endif
}

Ghost::~Ghost() {}

bool Ghost::occupiesSingleTile() {
  auto isTileX = mGhostShape.getPosition().x / TILE_SIZE;
  auto isTileY = mGhostShape.getPosition().y / TILE_SIZE;
  // i just like parentheses
  return (floor(isTileX) == isTileX && floor(isTileY) == isTileY);
}

void Ghost::chase(const Labyrinth &rLabyrinth, sf::Vector2f target) {
  std::priority_queue<TileScore, std::vector<TileScore>, OrderByScore> frontier;
  const int offset = rLabyrinth.getOffset(mGhostShape.getPosition());

  frontier.push(TileScore(offset, 0));

  std::map<int, std::optional<int>> cameFrom;
  std::map<int, int> costSoFar;

  cameFrom[offset] = NULL;
  costSoFar[offset] = 0;

  // find path by defining cameFrom
  while (!frontier.empty()) {
    TileScore current = frontier.top();
    frontier.pop();

    if (current.positionOffset == rLabyrinth.getOffset(target))
      break;

    auto neighbors = rLabyrinth.getNeighbors(current.positionOffset);
    for (auto next : neighbors) {
      int newCost = costSoFar[current.positionOffset] + rLabyrinth.heuristic(current.positionOffset, next);

      if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
        costSoFar[next] = newCost;
        int priority = newCost + rLabyrinth.heuristic(next, rLabyrinth.getOffset(target));
        frontier.push(TileScore(next, priority));
        cameFrom[next] = current.positionOffset;
      }
    }
  }

  // reconstruct path from cameFrom
  mPath.clear();  // we don't have to do this if we made it a map or something
  int current = rLabyrinth.getOffset(target);
  while (current != offset) {
    mPath.emplace_front(rLabyrinth.getSfVecFromOffset(current));
    auto iterator = cameFrom.find(current);
    if (iterator == cameFrom.end() || !iterator->second.has_value()) {
      break;
    }
    current = *iterator->second;
  }

  if (!occupiesSingleTile()) {
    // Ghost doesn't change direction unless Ghost occupies a single tile
    mGhostShape.setPosition(mGhostShape.getPosition() + mDirection);
    return;
  }

  if (!mPath.empty()) {
    sf::Vector2f nextPosition = mPath.front();
    auto ghostPosition = mGhostShape.getPosition();
    mDirection = nextPosition - ghostPosition;

    // Normalize direction to unit vector
    mDirection.x = (mDirection.x != 0) ? std::copysign(1.f, mDirection.x) : 0.f;
    mDirection.y = (mDirection.y != 0) ? std::copysign(1.f, mDirection.y) : 0.f;

    mGhostShape.setPosition(mGhostShape.getPosition() + mDirection);
  }
}

void Ghost::changeDirection(Direction newDirection) {
  switch (newDirection) {
  case DOWN:
    mDirection = sf::Vector2f(0.f, 1.f);
    break;
  case UP:
    mDirection = sf::Vector2f(0.f, -1.f);
    break;
  case RIGHT:
    mDirection = sf::Vector2f(1.f, 0.f);
    break;
  case LEFT:
    mDirection = sf::Vector2f(-1.f, 0.f);
    break;
  }
}

void Ghost::draw(std::shared_ptr<sf::RenderWindow> pGameWindow) {
  pGameWindow->draw(mGhostShape);
}

sf::Vector2f Ghost::getPosition() {
  return mGhostShape.getPosition();
}

void Ghost::meander(const Labyrinth &rLabyrinth) {
  auto newPosition = mGhostShape.getPosition() + mDirection;

  // EXPLAIN:
  // first check to see if we have any available turns, before we start moving
  // let's do this by assuring ourselves that the ghost is in a single tile
  // and that it has turns available
  auto ghostSizeX = mGhostShape.getGlobalBounds().width;
  auto ghostSizeY = mGhostShape.getGlobalBounds().height;

  // EXPLAIN:
  // now let's calculate some helpful values like our direction as a sf::Vector2f,
  // our direction as a Direction enum, and our available turns at the current position.
  auto maxLabyrinthWidth = rLabyrinth.m_labyrinthCols * rLabyrinth.m_labyrinthTileSize;
  auto maxLabyrinthHeight = rLabyrinth.m_labyrinthRows * rLabyrinth.m_labyrinthTileSize;
  wrapCoordinate(newPosition.x, -ghostSizeX, maxLabyrinthWidth);
  wrapCoordinate(newPosition.y, -ghostSizeY, maxLabyrinthHeight);

  // EXPLAIN:
  // when we calculate the direction across the open 'tunnel' where
  // we pop on the other side of the labyrinth, we do it by wrapping the
  // coordinates.  this messes up the direction vector, so
  // our availableTurns and directionVecToDirection get messed up as well
  auto x2 = fmod(newPosition.x + maxLabyrinthWidth, maxLabyrinthWidth);
  auto y2 = fmod(newPosition.y + maxLabyrinthHeight, maxLabyrinthHeight);
  auto x1 = mGhostShape.getPosition().x;
  auto y1 = mGhostShape.getPosition().y;

  auto calculatedDirection = sf::Vector2f(
      static_cast<float>(x2 - x1),
      static_cast<float>(y2 - y1));

  auto turns = availableTurns(mGhostShape.getPosition(), calculatedDirection, rLabyrinth);

  if (occupiesSingleTile() && turns.size() > 2) {
    bool doesGhostTurn = (mRandGenerator() % 100) <= mMeanderOdds;
    if (doesGhostTurn) {
      unsigned int newDirection;
      newDirection = mRandGenerator() % turns.size();
      changeDirection(turns.at(newDirection));
      mGhostShape.setPosition(newPosition);
      return;
    }
  }

  bool wallCollision = wallCollides(
      newPosition,
      sf::Vector2f(24.f, 24.f),
      rLabyrinth);

  while (wallCollision) {
    auto newDirection = static_cast<Direction>(mRandGenerator() % 4);
    changeDirection(newDirection);
    newPosition = mGhostShape.getPosition() + mDirection;
    wallCollision = wallCollides(
        newPosition,
        sf::Vector2f(24.f, 24.f),
        rLabyrinth);

    if (!wallCollision) {
      mGhostShape.setPosition(newPosition);
      return;
    }
  }

  // EXPLAIN:
  // if we've gotten here, we didn't take a turn nor did we collide with a wall.
  // if we didn't take a turn, it's because we rolled the dice and kept going
  mGhostShape.setPosition(newPosition);
}

void Ghost::scatter() {}

void Ghost::drawPath(Labyrinth &rLabyrinth) {
  if (mDebugMode) {
    for (const auto &p : mPath) {
      rLabyrinth.set(p, Labyrinth::Tile::PATH);
    }
  }
}

void Ghost::resetPath(Labyrinth &rLabyrinth) {
  if (mDebugMode) {
    for (const auto &p : mPath) {
      rLabyrinth.set(p, Labyrinth::Tile::EMPTY);
    }
  }
  mPath.clear();
}
