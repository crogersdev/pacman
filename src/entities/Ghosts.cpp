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

Ghost::Ghost(float s, sf::Vector2f p, sf::Color c, const Labyrinth &rl)
    :   mChaseSpeed(s)
      , mColor(c)
      , mDirection(sf::Vector2f(1.f, 0.f))
      , mFrightenedSpeed(.3f)
      , mGhostShape(sf::Vector2f(TILE_SIZE, TILE_SIZE))
      , mInitialPosition(p)
      , mMeanderOdds(66.6f)
      , mMeanderSpeed(.8f)
      , mRLabyrinth(rl)
      , mState(Ghost::State::MEANDER) {
  mDebugMode = false;
  mGhostShape.setFillColor(mColor);
  mGhostShape.setPosition(mInitialPosition);
  mSeed = std::chrono::system_clock::now().time_since_epoch().count();
  mRandGenerator = std::mt19937(mSeed);
}

Ghost::~Ghost() {}

void Ghost::act() {
  switch (mState) {
    case State::CHASE:
      chase();
      break;
    case State::MEANDER:
      meander();
      break;
    case State::SCATTER:
      // setTarget( some corner )
      chase();
    case State::FRIGHTENED:
      scatter();
      break;
    default:
      break;
  }
}

void Ghost::changeDirection(Direction newDirection) {
  switch (newDirection) {
  case Direction::DOWN:
    mDirection = sf::Vector2f(0.f, 1.f);
    break;
  case Direction::UP:
    mDirection = sf::Vector2f(0.f, -1.f);
    break;
  case Direction::RIGHT:
    mDirection = sf::Vector2f(1.f, 0.f);
    break;
  case Direction::LEFT:
    mDirection = sf::Vector2f(-1.f, 0.f);
    break;
  }
}

bool Ghost::checkAndSnapToTile() {
  const float SNAP_THRESHOLD = 4.0f; // Adjust this value as needed

  sf::Vector2f currentPos = mGhostShape.getPosition();
  sf::Vector2f tilePos(currentPos.x / TILE_SIZE, currentPos.y / TILE_SIZE);

  // If we're already perfectly aligned, return true
  if (tilePos.x == std::round(tilePos.x) &&
    tilePos.y == std::round(tilePos.y)) {
    return true;
  }

  // Calculate rounded position
  sf::Vector2f roundedPos(
    std::round(tilePos.x) * TILE_SIZE,
    std::round(tilePos.y) * TILE_SIZE);

  // Calculate distance to nearest tile center
  float distanceX = std::abs(currentPos.x - roundedPos.x);
  float distanceY = std::abs(currentPos.y - roundedPos.y);

  // If within threshold, snap and return true
  if (distanceX <= SNAP_THRESHOLD && distanceY <= SNAP_THRESHOLD) {
    mGhostShape.setPosition(roundedPos);
    std::cout << "Snapped to new position: " << roundedPos.y << ", " << roundedPos.x << "\n";
    return true;
  }

  // Not aligned and not close enough to snap
  return false;
}

// bool Ghost::checkAndSnapToTile() {
//     sf::Vector2f currentPos = mGhostShape.getPosition();
//     sf::Vector2f tilePos(currentPos.x / TILE_SIZE, currentPos.y / TILE_SIZE);

//     // If we're already perfectly aligned, return early
//     if (tilePos.x == std::round(tilePos.x) &&
//         tilePos.y == std::round(tilePos.y)) {
//         return false;
//     }

//     // Round to nearest tile
//     sf::Vector2f newPos(
//         std::round(tilePos.x) * TILE_SIZE,
//         std::round(tilePos.y) * TILE_SIZE);

//     mGhostShape.setPosition(newPos);
//     std::cout << "setting a new position0: " << newPos.y << ", " << newPos.x << "\n";

//     return true;  // Snap occurred
// }

void Ghost::draw(std::shared_ptr<sf::RenderWindow> pGameWindow) {
  pGameWindow->draw(mGhostShape);
}

bool Ghost::hasLeftCurrentTile() {
    sf::Vector2f pos = mGhostShape.getPosition();
    sf::Vector2f tilePos(pos.x / TILE_SIZE, pos.y / TILE_SIZE);

    // Get fractional part of position (how far into current tile)
    float fracX = tilePos.x - std::floor(tilePos.x);
    float fracY = tilePos.y - std::floor(tilePos.y);

    // Check if we've moved past tile midpoint based on direction
    switch (directionVecToDirection(mDirection)) {
        case Direction::UP:
          if (fracY < 0.5f) {
            std::cout << "we have left our current tile!\n";
            return true;
          }
        case Direction::DOWN:
          if (fracY > 0.5f) {
            std::cout << "we have left our current tile!\n";
            return true;
          }
        case Direction::LEFT:
          if (fracX < 0.5f) {
            std::cout << "we have left our current tile!\n";
            return true;
          }
        case Direction::RIGHT:
          if (fracX > 0.5f) {
            std::cout << "we have left our current tile!\n";
            return true;
          }
    }
    return false;
}

sf::Vector2f Ghost::getPosition() {
  return mGhostShape.getPosition();
}

void Ghost::resetPath() {
  mPath.clear();
}

void Ghost::chase() {
  auto target = mTarget;
  std::priority_queue<TileScore, std::vector<TileScore>, OrderByScore> frontier;
  const int ghostOffset = mRLabyrinth.getOffset(mGhostShape.getPosition());

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

    if (current.positionOffset == mRLabyrinth.getOffset(target)) {
      break;
    }

    auto neighbors = mRLabyrinth.getNeighbors(current.positionOffset);
    for (auto next : neighbors) {
      int movementCost = mRLabyrinth.movementCost(current.positionOffset, next);
      int newCost = costSoFar[current.positionOffset] + movementCost;

      if (costSoFar.find(next) == costSoFar.end() || newCost < costSoFar[next]) {
        costSoFar[next] = newCost;

        auto h = mRLabyrinth.heuristic(next, mRLabyrinth.getOffset(target));
        auto ht = mRLabyrinth.heuristicThroughTunnel(next, mRLabyrinth.getOffset(target));

        auto lowerHeuristic = std::min(h, ht);

        int priority = newCost + lowerHeuristic;
        frontier.push(TileScore(next, priority));
        cameFrom[next] = current.positionOffset;
      }
    }
  }

  // reconstruct path from cameFrom
  mPath.clear();  // we don't have to do this if we made it a map or something
  int current = mRLabyrinth.getOffset(target);
  while (current != ghostOffset) {
    mPath.emplace_front(mRLabyrinth.getSfVecFromOffset(current));
    auto iterator = cameFrom.find(current);
    if (iterator == cameFrom.end() || !iterator->second.has_value()) {
      std::cout << "path broken\n";
      break;
    }
    current = *iterator->second;
  }

  // consider changing to "if in tile completely and not at final destination"
  // that way you can get rid of 'has left current tile'

  bool ready = ghostOffset == mRLabyrinth.getOffset(mPath.front());
  if (ready)
    std::cout << "i'm ready, son\n";

  if (ready && checkAndSnapToTile()) {
    // Ghost doesn't change direction unless Ghost occupies a single tile
    auto movement = mDirection * mChaseSpeed;
    auto newPosition = mGhostShape.getPosition() + movement;
    wrapCoordinate(newPosition.x, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthWidth);
    wrapCoordinate(newPosition.y, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthHeight);
    mGhostShape.setPosition(newPosition);
    std::cout << "returning\n";
    return;
  }

  if (!mPath.empty()) {
    sf::Vector2f nextPosition = mPath.front();
    auto ghostPosition = mGhostShape.getPosition();
    mDirection = nextPosition - ghostPosition;

    // Normalize direction to unit vector
    mDirection.x = (mDirection.x != 0) ? std::copysign(1.f, mDirection.x) : 0.f;
    mDirection.y = (mDirection.y != 0) ? std::copysign(1.f, mDirection.y) : 0.f;

    std::cout << "direction: (x: "
              << mDirection.x
              << ", y: "
              << mDirection.y
              << "\t"
              << directionVecToDirection(mDirection)
              << "\n";

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

    auto movement = mDirection * mChaseSpeed;
    auto newPosition = mGhostShape.getPosition() + movement;
    wrapCoordinate(newPosition.x, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthWidth);
    wrapCoordinate(newPosition.y, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthHeight);
    mGhostShape.setPosition(newPosition);
  }
}
 
/*
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

  if (!occupiesSingleTile()) {
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
    if ((ghostPosition.y / TILE_SIZE) == 14 && (nextPosition.y / TILE_SIZE) == 14) {
      if (ghostPosition.x <= 50 && nextPosition.x == 675) {
        mDirection = sf::Vector2f(-1.f, 0.f);
      } else if (ghostPosition.x >= 650 && nextPosition.x == 0) {
        mDirection = sf::Vector2f(1.f, 0.f);
      }
    }

    if (std::abs(mDirection.x) == 1.f && std::abs(mDirection.y) == 1.f) {
      mDirection.y = 0.f;
      std::cout << "caution rogue robots\n";
    }

    auto movement = mDirection * mSpeedMultiplier;
    auto newPosition = mGhostShape.getPosition() + movement;
    wrapCoordinate(newPosition.x, -TILE_SIZE, rLabyrinth.mMaxLabyrinthWidth);
    wrapCoordinate(newPosition.y, -TILE_SIZE, rLabyrinth.mMaxLabyrinthHeight);
    mGhostShape.setPosition(newPosition);
  }
}
  */

void Ghost::meander() {
  auto movement = mDirection * mMeanderSpeed;
  auto newPosition = mGhostShape.getPosition() + movement;

  auto ghostSizeX = mGhostShape.getGlobalBounds().size.x;
  auto ghostSizeY = mGhostShape.getGlobalBounds().size.y;

  wrapCoordinate(newPosition.x, -ghostSizeX, mRLabyrinth.mMaxLabyrinthWidth);
  wrapCoordinate(newPosition.y, -ghostSizeY, mRLabyrinth.mMaxLabyrinthHeight);

  // EXPLAIN:
  // when we calculate the direction across the open 'tunnel' where
  // we pop on the other side of the labyrinth, we do it by wrapping the
  // coordinates.  this messes up the direction vector, so
  // our availableTurns and directionVecToDirection get messed up as well
  auto x2 = fmod(newPosition.x + mRLabyrinth.mMaxLabyrinthWidth, mRLabyrinth.mMaxLabyrinthWidth);
  auto y2 = fmod(newPosition.y + mRLabyrinth.mMaxLabyrinthHeight, mRLabyrinth.mMaxLabyrinthHeight);
  auto x1 = mGhostShape.getPosition().x;
  auto y1 = mGhostShape.getPosition().y;

  auto calculatedDirection = sf::Vector2f(
      static_cast<float>(x2 - x1),
      static_cast<float>(y2 - y1));

  auto turns = availableTurns(mGhostShape.getPosition(), calculatedDirection, mRLabyrinth);

  if (hasLeftCurrentTile() && !checkAndSnapToTile() && turns.size() > 1) {
    bool doesGhostTurn = (mRandGenerator() % 100) <= mMeanderOdds;
    if (doesGhostTurn) {
      unsigned int newDirection;
      newDirection = mRandGenerator() % turns.size();
      changeDirection(turns.at(newDirection));
      mGhostShape.setPosition(newPosition);
      return;
    }
  }

  bool wallCollision =
    wallCollides(newPosition,
                 sf::Vector2f(TILE_SIZE - 1.f, TILE_SIZE - 1.f),
                 mRLabyrinth)
    ||
    mRLabyrinth.at(static_cast<int> (newPosition.x / TILE_SIZE),
                   static_cast<int> (newPosition.y / TILE_SIZE))
    == Labyrinth::Tile::GATE;

  while (wallCollision) {
    auto newDirection = static_cast<Direction>(mRandGenerator() % 4);
    changeDirection(newDirection);
    movement = mDirection * mMeanderSpeed;
    newPosition = mGhostShape.getPosition() + movement;

    wallCollision = wallCollides(newPosition,
                                 sf::Vector2f(TILE_SIZE - 1.f, TILE_SIZE - 1.f),
                                 mRLabyrinth)
    ||
    mRLabyrinth.at(static_cast<int> (newPosition.x / TILE_SIZE),
                   static_cast<int> (newPosition.y / TILE_SIZE))
    == Labyrinth::Tile::GATE;

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

void Ghost::scatter() {
  mTarget = mFrightenedTarget;
  mChaseSpeed = mFrightenedSpeed;
  chase();
}
