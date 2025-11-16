#include <algorithm>
#include <iomanip>
#include <sstream>

#include "Ghost.hpp"
#include "DebugUtils.hpp"
#include "helpers/Movement.hpp"

Ghost::Ghost(std::string name, std::string texture, std::pair<int, int> initTilePos, Vector2 scatterTilePos) 
    : mChaseSpeed(40.f)
    , mChaseTarget{}
    , mDirection{ 1.f, 0.f }
    , mDistanceToTarget()
    , mFrightenedSpeed(30.f)
    , mGen(std::random_device{}())
    , mGhostSprite(texture, 26, 26, 2, 4)
    , mGhostTexture(texture)
    , mInitialPosition{ initTilePos.first * TILE_SIZE - TILE_SIZE / 2.f, initTilePos.second * TILE_SIZE - TILE_SIZE / 2.f }
    , mLastDecisionTile{-1, -1}
    , mName(name)
    , mPrisonSpeed(55.f)
    , mPrisonPosition{ 12.f * TILE_SIZE, 12.f * TILE_SIZE }
    , mPosition{ initTilePos.first * TILE_SIZE - TILE_SIZE / 2.f, initTilePos.second * TILE_SIZE - TILE_SIZE / 2.f }
    , mScatterCornerPosition{ scatterTilePos.x * TILE_SIZE - TILE_SIZE / 2.f, scatterTilePos.y * TILE_SIZE - TILE_SIZE / 2.f }
    , mSpeed(40.f)
    , mState(State::SCATTER)
    , mTurns{} {}

Ghost::~Ghost() {}

void Ghost::act(shared_ptr<Labyrinth> labyrinth) {
    // i hate how the ints and floats don't mix this sucks
    int tilePositionX = getTilePosition().first;
    int tilePositionY = getTilePosition().second;

    if (isCentered(mPosition, TILE_SIZE) && (tilePositionX != mLastDecisionTile.first || tilePositionY != mLastDecisionTile.second)) {
        mLastDecisionTile = std::make_pair(tilePositionX, tilePositionY);
        
        switch (mState) {
        case State::CHASE:
            mSpeed = mChaseSpeed;
            chase(labyrinth);
            break;
        case State::SCATTER:
            mSpeed = mChaseSpeed;
            chase(labyrinth);
            break;
        case State::LEAVING_PRISON:
            mSpeed = mChaseSpeed;
            chase(labyrinth);
            break;
        case State::FRIGHTENED:
            mSpeed = mFrightenedSpeed;
            meander(labyrinth);
            break;
        case State::GOING_TO_PRISON:
            mSpeed = mPrisonSpeed;
            chase(labyrinth);
            break; 
        case State::MEANDER:
        case State::IN_PRISON:
            meander(labyrinth);
            break;
        default:
            break;
        }
    }
    
    updateSpriteFrameAndMove();
}

void Ghost::chase(shared_ptr<Labyrinth> labyrinth) {
    mDistanceToTarget.clear();

    Vector2 target = mChaseTarget;
    if (mState == State::SCATTER) {
        target = mScatterCornerPosition;
    } else if (mState == State::GOING_TO_PRISON) {
        target = mPrisonPosition;
    }

    auto availableTurns = getAvailableTurns(labyrinth);
    Vector2 turn = Vector2{ 0.f , 0.f };
    float bestDistance = 1000.f, distance;
    int tilePositionX = getTilePosition().first;
    int tilePositionY = getTilePosition().second;

    std::vector<Vector2> bestDistanceDirections;

    for (const auto& t: availableTurns ) {
        Vector2 potentialPosition = Vector2Add(mPosition, Vector2Scale(t.second, TILE_SIZE));

        if (tilePositionY == TUNNEL_ROW || static_cast<int>(target.y / TILE_SIZE) == TUNNEL_ROW) {
            Vector2 leftTunnelExit = Vector2{ 0.f, TUNNEL_ROW * TILE_SIZE };
            float leftTunnelDistance = Vector2Distance(potentialPosition, leftTunnelExit);
            leftTunnelDistance += Vector2Distance(leftTunnelExit, target);

            Vector2 rightTunnelExit = Vector2{ (LABYRINTH_COLS - 1) * TILE_SIZE, TUNNEL_ROW * TILE_SIZE };
            float rightTunnelDistance = Vector2Distance(potentialPosition, rightTunnelExit);
            rightTunnelDistance += Vector2Distance(rightTunnelExit, target);

            float tunnelDistance = std::min(leftTunnelDistance, rightTunnelDistance);
            distance = std::min(tunnelDistance, Vector2Distance(potentialPosition, target));
        } else {
            distance = Vector2Distance(potentialPosition, target);
        }

        mDistanceToTarget.push_back(std::make_pair(potentialPosition, distance));

        if (distance < bestDistance) {
            bestDistance = distance;
            bestDistanceDirections.clear();
            turn = t.second;
        }
        if (distance <= bestDistance) {
            bestDistanceDirections.push_back(t.second);
        }
    }

    if (bestDistanceDirections.size() > 0) {
        Vector2 directionPriorities[] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };
        for (const auto& priority : directionPriorities) {
            if (std::find(bestDistanceDirections.begin(), bestDistanceDirections.end(), priority) != bestDistanceDirections.end()) {
                turn = priority;
                break;
            }
        }
    }

    mDirection = turn;
}

void Ghost::draw() {
    mGhostSprite.draw(mPosition);
}

void Ghost::drawDebugDistances() {
    if (mDistanceToTarget.empty())
        return;

    Vector2 directions[] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};  // up, down, left, right
    Vector2 offsets[] = {{0, -20}, {0, 20}, {-25, 0}, {25, 0}}; // above, below, left, right

    for (int i = 0; i < 4; i++) {
        Vector2 testPos = Vector2Add(mPosition, Vector2Scale(directions[i], TILE_SIZE));

        // Find matching distance from your mDistanceToTarget vector
        float dist = -1;
        for (const auto &pair : mDistanceToTarget) {
            if (Vector2Distance(pair.first, testPos) < 5.0f)
            { 
                dist = pair.second;
                break;
            }
        }

       if (dist >= 0) {
            Vector2 textPos = Vector2Add(mPosition, offsets[i]);
            DrawText(TextFormat("%.1f", dist), textPos.x, textPos.y, 10, YELLOW);
       }
    }
    
    if (mState == State::GOING_TO_PRISON || mState == State::CHASE) {
        Color targetColor = Fade(WHITE, 0.5f);
        if (mName == "Inky")   targetColor = Fade(BLUE,   0.5f);
        if (mName == "Pinky")  targetColor = Fade(PINK,   0.5f);
        if (mName == "Blinky") targetColor = Fade(RED,    0.5f);
        if (mName == "Clyde")  targetColor = Fade(ORANGE, 0.5f);

        DrawRectangle(mChaseTarget.x, mChaseTarget.y, 26, 26, targetColor);
        DrawLine(mPosition.x, mPosition.y, mChaseTarget.x + 12, mChaseTarget.y + 12, targetColor);
    }
}

std::map<Direction, Vector2> Ghost::getAvailableTurns(shared_ptr<Labyrinth> labyrinth) {
    int currentTileX = getTilePosition().first;
    int currentTileY = getTilePosition().second;

    auto availableTurns(directionLut);

    for (int i = 0; i < 4; ++i) {
        Direction d = static_cast<Direction>(i);

        if (directionLut.at(d).x == mDirection.x && directionLut.at(d).y == mDirection.y) {
            availableTurns.erase(static_cast<Direction>(i ^ 1));
        }

        Vector2 intendedTile = {
            currentTileX + directionLut.at(d).x,
            currentTileY + directionLut.at(d).y
        };

        Labyrinth::Tile tile = labyrinth->at(intendedTile.y, intendedTile.x);
        switch (tile) {
        case Labyrinth::Tile::WALL:
            availableTurns.erase(d);
            break;
        case Labyrinth::Tile::GATE:
            if (mState != State::LEAVING_PRISON && mState != State::GOING_TO_PRISON) {
                availableTurns.erase(d);
            }
            break;
        default:
            break;
        }
    }

    mTurns.clear();
    for (const auto& turn : availableTurns) {
        auto x = mPosition.x + (turn.second.x * 20);
        auto y = mPosition.y + (turn.second.y * 20);
        mTurns.push_back(std::make_pair<int, int>(x, y));
    }

    return availableTurns;
}

std::pair<int, int> Ghost::getTilePosition() const {
    int x = static_cast<int>(mPosition.x / TILE_SIZE);
    int y = static_cast<int>(mPosition.y / TILE_SIZE);
    return std::make_pair(x, y);
}

void Ghost::meander(shared_ptr<Labyrinth> labyrinth) {
    auto availableTurns = getAvailableTurns(labyrinth);

    if (availableTurns.size() == 1) {
        mDirection = availableTurns.begin()->second;

    } else if (availableTurns.size() == 2) {
        bool currentDirectionAvailable = false;
        for (const auto& turn : availableTurns) {
            if (turn.second.x == mDirection.x && turn.second.y == mDirection.y) {
                currentDirectionAvailable = true;
                break;
            }
        }
        if (!currentDirectionAvailable) {
            std::uniform_int_distribution<> dis(0, 1);
            auto it = availableTurns.begin();
            std::advance(it, dis(mGen));
            mDirection = it->second;
        }

    } else if (availableTurns.size() >= 3) {
        std::uniform_int_distribution<> dis(0, 3);
        auto it = availableTurns.begin();
        std::advance(it, dis(mGen) % availableTurns.size());
        mDirection = it->second;
    }
}

void Ghost::updateSprite() {
    if (mState == Ghost::State::FRIGHTENED) {
        mGhostSprite.setTextureFile("res/frightened.png");
    } else if (mState == Ghost::State::GOING_TO_PRISON) {
        mGhostSprite.setTextureFile("res/going-to-prison.png");
    } else {
        mGhostSprite.setTextureFile(mGhostTexture); 
    }
}

void Ghost::updateSpriteFrameAndMove() {
    if (mState != Ghost::State::FRIGHTENED) {
        if (mDirection == Vector2{  0.f, -1.f }) { mGhostSprite.setZeroFrame(0); }
        if (mDirection == Vector2{  0.f,  1.f }) { mGhostSprite.setZeroFrame(6); }
        if (mDirection == Vector2{ -1.f,  0.f }) { mGhostSprite.setZeroFrame(4); }
        if (mDirection == Vector2{  1.f,  0.f }) { mGhostSprite.setZeroFrame(2); }
    }

    Vector2 newPosition = Vector2Add(mPosition, Vector2Scale(mDirection, mSpeed * GetFrameTime()));

    int newTileX = static_cast<int>(newPosition.x / TILE_SIZE);
    int newTileY = static_cast<int>(newPosition.y / TILE_SIZE);

    if (newTileY == TUNNEL_ROW) {
        if (newTileX < 0) {
            newPosition.x = LABYRINTH_COLS * TILE_SIZE;
        }
        if (newTileX > LABYRINTH_COLS - 1) {
            newPosition.x = 0;
        }
    }

    mPosition = newPosition;
    mGhostSprite.update();
}