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
      , mPath()
      , mRLabyrinth(rl)
      , mState(Ghost::State::WAIT) {
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
      this->chase();
      break;
    case State::MEANDER:
      this->meander();
      break;
    case State::FRIGHTENED:
      this->scatter();
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
  auto isAlmostEqual = [](float a, float b, float epsilon = .001f) {
    return std::abs(a - b) < epsilon;
  };

  const float LEADING_EDGE_SNAP_THRESHOLD = 0.05f;   // use this for going UP and LEFT
  const float TRAILING_EDGE_SNAP_THRESHOLD = 0.95f;  // use this for going DOWN and RIGHT
  const float TILE_ALIGNMENT_THRESHOLD = 0.01f;

  sf::Vector2f tilePos = sf::Vector2f(
    mGhostShape.getPosition().x / TILE_SIZE,
    mGhostShape.getPosition().y / TILE_SIZE);

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
    case Direction::UP:
      shouldSnap = fracY <= LEADING_EDGE_SNAP_THRESHOLD && fracX <= TILE_ALIGNMENT_THRESHOLD;
      if (shouldSnap) newPosition.y = std::floor(tilePos.y) * TILE_SIZE;
      break;
    case Direction::DOWN:
      shouldSnap = fracY >= TRAILING_EDGE_SNAP_THRESHOLD && fracX <= TILE_ALIGNMENT_THRESHOLD;
      if (shouldSnap) newPosition.y = std::ceil(tilePos.y) * TILE_SIZE;
      break;
    case Direction::LEFT:
      shouldSnap = fracY <= TILE_ALIGNMENT_THRESHOLD && fracX <= LEADING_EDGE_SNAP_THRESHOLD;
      if (shouldSnap) newPosition.x = std::floor(tilePos.x) * TILE_SIZE;
      break;
    case Direction::RIGHT:
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

void Ghost::draw(std::shared_ptr<sf::RenderWindow> pGameWindow) {
  pGameWindow->draw(mGhostShape);
}

sf::Vector2f Ghost::getPosition() {
  sf::Vector2f pos = mGhostShape.getPosition();
  pos.x += (TILE_SIZE / 2.f) - 1.f;
  pos.y += (TILE_SIZE / 2.f) - 1.f;

  return pos;
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

  if (checkAndSnapToTile() == false) {
    // Ghost doesn't change direction unless Ghost occupies a single tile
    auto movement = mDirection * mChaseSpeed;
    auto newPosition = mGhostShape.getPosition() + movement;
    wrapCoordinate(newPosition.x, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthWidth);
    wrapCoordinate(newPosition.y, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthHeight);
    mGhostShape.setPosition(newPosition);
    return;
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

    auto movement = mDirection * mChaseSpeed;
    auto newPosition = mGhostShape.getPosition() + movement;
    wrapCoordinate(newPosition.x, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthWidth);
    wrapCoordinate(newPosition.y, -TILE_SIZE, mRLabyrinth.mMaxLabyrinthHeight);
    mGhostShape.setPosition(newPosition);
  }
}

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
    mRLabyrinth);

  while (wallCollision) {
    auto newDirection = static_cast<Direction>(mRandGenerator() % 4);
    changeDirection(newDirection);
    movement = mDirection * mMeanderSpeed;
    newPosition = mGhostShape.getPosition() + movement;

    wallCollision = wallCollides(
      newPosition,
      sf::Vector2f(TILE_SIZE - 1.f, TILE_SIZE - 1.f),
      mRLabyrinth);

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
  this->chase();
}
