#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

#include "./Ghosts.hpp"
#include "../helpers/Collisions.hpp"

Ghost::Ghost(float speed, sf::Vector2f pos, sf::Color c, bool debugMode)
    : mColor(c),
      mDebugMode(debugMode),
      mMeanderOdds(66.6),
      mSpeedMultiplier(speed),
      mGhostShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)),
      mDirection(sf::Vector2f(1.f, 0.f)),
      mInitialPosition(pos),
      mTarget(),
      mPath(),
      mState(MEANDER) {
  mSeed = std::chrono::system_clock::now().time_since_epoch().count();
  mRandGenerator = std::mt19937(mSeed);
  mGhostShape.setFillColor(mColor);
  mGhostShape.setPosition(mInitialPosition);

#ifndef NDEBUG
  mDebugMode = true;
#endif
}

Ghost::~Ghost() {}

bool Ghost::checkAndSnapToTile() {

  auto isAlmostEqual = [](float a, float b, float epsilon =.001f) {
    return std::abs(a - b) < epsilon;
  };

  const float LEADING_EDGE_SNAP_THRESHOLD = 0.05f;   // use this for going UP and LEFT
  const float TRAILING_EDGE_SNAP_THRESHOLD = 0.95f;  // use this for going DOWN and RIGHT
  const float TILE_ALIGNMENT_THRESHOLD = 0.01f;

  sf::Vector2f tilePos = sf::Vector2f(
    mGhostShape.getPosition().x / TILE_SIZE,
    mGhostShape.getPosition().y / TILE_SIZE
  );

  if (isAlmostEqual(tilePos.x, std::round(tilePos.x)) &&
      isAlmostEqual(tilePos.y, std::round(tilePos.y))) {
      return true;
  }

  float fracX = tilePos.x - std::floor(tilePos.x);
  float fracY = tilePos.y - std::floor(tilePos.y);

  Direction dir = directionVecToDirection(mDirection);
  sf::Vector2f newPosition = mGhostShape.getPosition();
  bool shouldSnap = false;

  switch (dir) {
    case UP:
      shouldSnap = fracY <= LEADING_EDGE_SNAP_THRESHOLD && fracX <= TILE_ALIGNMENT_THRESHOLD;
      if (shouldSnap) newPosition.y = std::floor(tilePos.y) * TILE_SIZE;
      break;
    case DOWN:
      shouldSnap = fracY >= TRAILING_EDGE_SNAP_THRESHOLD && fracX <= TILE_ALIGNMENT_THRESHOLD;
      if (shouldSnap) newPosition.y = std::ceil(tilePos.y) * TILE_SIZE;
      break;
    case LEFT:
      shouldSnap = fracY <= TILE_ALIGNMENT_THRESHOLD && fracX <= LEADING_EDGE_SNAP_THRESHOLD;
      if (shouldSnap) newPosition.x = std::floor(tilePos.x) * TILE_SIZE;
      break;
    case RIGHT:
      shouldSnap = fracY <= TILE_ALIGNMENT_THRESHOLD && fracX >= TRAILING_EDGE_SNAP_THRESHOLD;
      if (shouldSnap) newPosition.x = std::ceil(tilePos.x) * TILE_SIZE;
      break;
  }

  if (shouldSnap) {
    mGhostShape.setPosition(newPosition);
    return true;
  }

  return false;
}

void Ghost::chase(const Labyrinth &rLabyrinth, sf::Vector2f target) {
  mTarget = target;
  std::priority_queue<TileScore, std::vector<TileScore>, OrderByScore> frontier;
  const int ghostOffset = rLabyrinth.getOffset(mGhostShape.getPosition());

  frontier.push(TileScore(ghostOffset, 0));

  std::map<int, std::optional<int>> cameFrom;
  std::map<int, int> costSoFar;

  cameFrom[ghostOffset] = NULL;
  costSoFar[ghostOffset] = 0;

  // EXPLAIN: This is a hack to get past the position Pacman
  //          will be in as he wraps his coordinates as
  //          he goes across that open chasm thing from one
  //          side of the labyrinth to the other.  his x
  //          position could be more or less than the bounds
  //          of the labyrinth so we just tie them to the max/
  //          min of the labyrinth
  if (floor(target.x / TILE_SIZE) <= 0) {
    target.x = LABYRINTH_COLS - 2;
  }
  if (floor(target.x / TILE_SIZE) >= LABYRINTH_COLS - 2) {
    target.x = 0;
  }

  // find path by defining cameFrom
  while (!frontier.empty()) {
    TileScore current = frontier.top();
    frontier.pop();

    if (current.positionOffset == rLabyrinth.getOffset(target)) {
      break;
    }

    auto neighbors = rLabyrinth.getNeighbors(current.positionOffset);
    for (auto next : neighbors) {
      int movementCost = rLabyrinth.movementCost(current.positionOffset, next);
      int newCost = costSoFar[current.positionOffset] + movementCost;

      if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
        costSoFar[next] = newCost;

        auto h = rLabyrinth.heuristic(next, rLabyrinth.getOffset(target));
        auto ht = rLabyrinth.heuristicThroughTunnel(next, rLabyrinth.getOffset(target));

        auto lowerHeuristic = std::min(h, ht);

        int priority = newCost + lowerHeuristic;
        frontier.push(TileScore(next, priority));
        cameFrom[next] = current.positionOffset;
      }
    }
  }

  if (checkAndSnapToTile() == false) {
    // Ghost doesn't change direction unless Ghost occupies a single tile
    auto movement = mDirection * mSpeedMultiplier;
    auto newPosition = mGhostShape.getPosition() + movement;
    wrapCoordinate(newPosition.x, -TILE_SIZE, rLabyrinth.mMaxLabyrinthWidth);
    wrapCoordinate(newPosition.y, -TILE_SIZE, rLabyrinth.mMaxLabyrinthHeight);
    mGhostShape.setPosition(newPosition);
    return;
  }

  // reconstruct path from cameFrom
  mPath.clear();  // we don't have to do this if we made it a map or something
  int current = rLabyrinth.getOffset(target);
  while (current != ghostOffset) {
    mPath.emplace_front(rLabyrinth.getSfVecFromOffset(current));
    auto iterator = cameFrom.find(current);
    if (iterator == cameFrom.end() || !iterator->second.has_value()) {
      std::cout << "path broken\n";
      break;
    }
    current = *iterator->second;
  }

  if (!mPath.empty()) {
    sf::Vector2f nextPosition = mPath.front();
    auto ghostPosition = mGhostShape.getPosition();
    mDirection = nextPosition - ghostPosition;

    // Normalize direction to unit vector
    mDirection.x = (mDirection.x != 0) ? std::copysign(1.f, mDirection.x) : 0.f;
    mDirection.y = (mDirection.y != 0) ? std::copysign(1.f, mDirection.y) : 0.f;

    // this is where we're going to detect if we're on the tunnel row
    // and then keep the direction to cross the tunnel properly even though
    // the direction vector determined by new position and current position
    // will point in the opposite direction
    if ((ghostPosition.y / TILE_SIZE) == TUNNEL_ROW && (nextPosition.y / TILE_SIZE) == TUNNEL_ROW) {
      if (ghostPosition.x <= 50 && nextPosition.x == 675) {
        mDirection = sf::Vector2f(-1.f, 0.f);
      } else if (ghostPosition.x >= 650 && nextPosition.x == 0) {
        mDirection = sf::Vector2f(1.f, 0.f);
      }
    }

    if (std::abs(mDirection.x) == 1.f && std::abs(mDirection.y) == 1.f) {
      std::cout << "caution rogue robots\n";
      // not sure where this bug is, and why the path gives us a diagonal
      // or if it's the location of the position being at a diagonal
      // in relation to the next position or something
      return;
    }

    auto movement = mDirection * mSpeedMultiplier;
    auto newPosition = mGhostShape.getPosition() + movement;
    wrapCoordinate(newPosition.x, -TILE_SIZE, rLabyrinth.mMaxLabyrinthWidth);
    wrapCoordinate(newPosition.y, -TILE_SIZE, rLabyrinth.mMaxLabyrinthHeight);
    mGhostShape.setPosition(newPosition);
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
  sf::Vector2f pos = mGhostShape.getPosition();
  pos.x += (TILE_SIZE / 2.f) - 1.f;
  pos.y += (TILE_SIZE / 2.f) - 1.f;

  return pos;
}

void Ghost::meander(const Labyrinth &rLabyrinth) {
  auto movement = mDirection * mSpeedMultiplier;
  auto newPosition = mGhostShape.getPosition() + movement;

  auto ghostSizeX = mGhostShape.getGlobalBounds().width;
  auto ghostSizeY = mGhostShape.getGlobalBounds().height;

  wrapCoordinate(newPosition.x, -ghostSizeX, rLabyrinth.mMaxLabyrinthWidth);
  wrapCoordinate(newPosition.y, -ghostSizeY, rLabyrinth.mMaxLabyrinthHeight);

  // EXPLAIN:
  // when we calculate the direction across the open 'tunnel' where
  // we pop on the other side of the labyrinth, we do it by wrapping the
  // coordinates.  this messes up the direction vector, so
  // our availableTurns and directionVecToDirection get messed up as well
  auto x2 = fmod(newPosition.x + rLabyrinth.mMaxLabyrinthWidth, rLabyrinth.mMaxLabyrinthWidth);
  auto y2 = fmod(newPosition.y + rLabyrinth.mMaxLabyrinthHeight, rLabyrinth.mMaxLabyrinthHeight);
  auto x1 = mGhostShape.getPosition().x;
  auto y1 = mGhostShape.getPosition().y;

  auto calculatedDirection = sf::Vector2f(
      static_cast<float>(x2 - x1),
      static_cast<float>(y2 - y1));

  auto turns = availableTurns(mGhostShape.getPosition(), calculatedDirection, rLabyrinth);

  if (checkAndSnapToTile() && turns.size() > 2) {
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
    sf::Vector2f(TILE_SIZE - 1.f, TILE_SIZE - 1.f),
    rLabyrinth);

  while (wallCollision) {
    auto newDirection = static_cast<Direction>(mRandGenerator() % 4);
    changeDirection(newDirection);
    movement = mDirection * mSpeedMultiplier;
    newPosition = mGhostShape.getPosition() + movement;

    wallCollision = wallCollides(
      newPosition,
      sf::Vector2f(TILE_SIZE - 1.f, TILE_SIZE - 1.f),
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
