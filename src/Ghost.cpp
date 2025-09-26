#include "Ghost.hpp"

#include <iomanip>
#include <sstream>

#include <raymath.h>

Ghost::Ghost(std::string name, std::string texture, Vector2 initTilePos, Vector2 scatterTilePos) 
    :
      mChaseSpeed(40.f),
      mChaseTarget{},
      mDirection{ 1.f, 0.f },
      mDistanceToTarget(),
      mFrightenedSpeed(30.f),
      mGen(std::random_device{}()),
      mGhostSprite(texture, 26, 26, 2, 4),
      mGhostTexture(texture),
      mLastDecisionTile(initTilePos),
      mName(name),
      mPrisonSpeed(55.f),
      mPrisonPosition{ 12.f * TILE_SIZE - TILE_SIZE / 2.f, 12.f * TILE_SIZE - TILE_SIZE / 2.f },
      mPosition{ initTilePos.x * TILE_SIZE - TILE_SIZE / 2.f, initTilePos.y * TILE_SIZE - TILE_SIZE / 2.f },
      mScatterCornerPosition{ scatterTilePos.x * TILE_SIZE - TILE_SIZE / 2.f, scatterTilePos.y * TILE_SIZE - TILE_SIZE / 2.f },
      mSpeed(40.f),
      mState(State::SCATTER),
      mTurns{} {
}

Ghost::~Ghost() {}

void Ghost::act(std::shared_ptr<Labyrinth> labyrinth) {
    // i hate how the ints and floats don't mix this sucks
    float tilePositionX = static_cast<int>(mPosition.x / TILE_SIZE);
    float tilePositionY = static_cast<int>(mPosition.y / TILE_SIZE);

    if (isCentered() && (tilePositionX != mLastDecisionTile.x || tilePositionY != mLastDecisionTile.y)) {
        mLastDecisionTile = Vector2{ tilePositionX, tilePositionY };
        
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

void Ghost::chase(std::shared_ptr<Labyrinth> labyrinth) {
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
    int tilePositionX = static_cast<int>(mPosition.x / TILE_SIZE);
    int tilePositionY = static_cast<int>(mPosition.y / TILE_SIZE);

    for (const auto& t: availableTurns ) {
        Vector2 potentialPosition = { mPosition.x + t.second.x * TILE_SIZE, mPosition.y + t.second.y * TILE_SIZE };

        if (tilePositionY == TUNNEL_ROW || static_cast<int>(target.y / TILE_SIZE) == TUNNEL_ROW) {
            Vector2 leftTunnelExit = Vector2{ 0.f, TUNNEL_ROW * TILE_SIZE };
            int leftTunnelDistance = computeTileDistance(potentialPosition, leftTunnelExit);
            leftTunnelDistance += computeTileDistance(leftTunnelExit, target);

            Vector2 rightTunnelExit = Vector2{ LABYRINTH_COLS * TILE_SIZE, TUNNEL_ROW * TILE_SIZE };
            int rightTunnelDistance = computeTileDistance(potentialPosition, rightTunnelExit);
            rightTunnelDistance += computeTileDistance(rightTunnelExit, target);

            distance = std::min(leftTunnelDistance, rightTunnelDistance);

        } else {
            distance = computeTileDistance(potentialPosition, target);
            std::cout << "tile pos (x, y): (" <<
                static_cast<int>(mPosition.x / TILE_SIZE) << ", " <<
                static_cast<int>(mPosition.y / TILE_SIZE) << ")";
            
            std::cout << "\tdistance from " <<
                static_cast<int>(potentialPosition.x / TILE_SIZE) << ", " <<
                static_cast<int>(potentialPosition.y / TILE_SIZE) << " to " <<
                static_cast<int>(target.x / TILE_SIZE) << ", " << 
                static_cast<int>(target.y / TILE_SIZE) << " is " << 
                std::round(distance) << "\n";
        }

        mDistanceToTarget.push_back(
            std::make_pair(
                Vector2{ potentialPosition.x + (TILE_SIZE * t.second.x), potentialPosition.y + (TILE_SIZE * t.second.y) },
                static_cast<int>(std::round(distance))
            )
        );

        if (std::round(distance) < bestDistance) {
            bestDistance = distance;
            turn = t.second;
        }
    }
    // std::cout << "picking turn direction <" << turn.x << ", " << turn.y << ">\n";
    mDirection = turn;
}

void Ghost::draw() {
    mGhostSprite.draw(mPosition);
    
    /*
    for (const auto& turn : mTurns) {
        DrawRectangle(turn.first, turn.second, 5, 12, Color{255, 128, 128, 255});
    }
    
    DrawRectangle(mPosition.x+(mDirection.x * 26)-13, mPosition.y+(mDirection.y * 26)-13, 26, 26, Color{0, 128, 64, 212});
    */
    DrawRectangle((mPosition.x / 26)-13, (mPosition.y / 26)-13, 26, 26, Color{0, 128, 64, 212});

    std::stringstream ss;
    for (const auto distance : mDistanceToTarget) {
        ss << std::fixed << std::setprecision(2) << distance.second;
        DrawText(ss.str().c_str(), distance.first.x, distance.first.y, 15, GOLD);
        ss.str("");
        ss.clear();
    }

    std::string debugGhost = "Poinky";
    if (mName == debugGhost) {
        std::cout << mName << "'s state: " << mState;
        std::cout << " -- tile target (x, y): (" << 
           static_cast<int>(mChaseTarget.x / TILE_SIZE) << ", " <<
           static_cast<int>(mChaseTarget.y / TILE_SIZE) << ")";
        std::cout << " -- tile pos (x, y): (" <<
            static_cast<int>(mPosition.x / TILE_SIZE) << ", " <<
            static_cast<int>(mPosition.y / TILE_SIZE) << ")";
        std::cout << " -- headed <" << mDirection.x << ", " << mDirection.y << ">\n";
    }
}

bool Ghost::isCentered() {
    const float ALIGNMENT_THRESHOLD = 1.5f; // for 60fps
    // const float ALIGNMENT_THRESHOLD = 6.f; // for 10fps
    const float TILE_CENTER_OFFSET = TILE_SIZE / 2.f;

    float distFromTileCenterX = fabs(
        (mPosition.x - TILE_CENTER_OFFSET) -
        (static_cast<int>(mPosition.x / TILE_SIZE) * TILE_SIZE));
    float distFromTileCenterY = fabs(
        (mPosition.y - TILE_CENTER_OFFSET) -
        (static_cast<int>(mPosition.y / TILE_SIZE) * TILE_SIZE));

    return (distFromTileCenterX < ALIGNMENT_THRESHOLD && 
            distFromTileCenterY < ALIGNMENT_THRESHOLD);
}

std::map<Direction, Vector2> Ghost::getAvailableTurns(std::shared_ptr<Labyrinth> labyrinth) {
    int currentTileX = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentTileY = static_cast<int>(mPosition.y / TILE_SIZE);

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

Vector2 Ghost::getTilePosition() const {
    return Vector2{ 
        std::floor(mPosition.x / TILE_SIZE),
        std::floor(mPosition.y / TILE_SIZE)
    };
}

void Ghost::meander(std::shared_ptr<Labyrinth> labyrinth) {
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
        // std::cout << "changing direction to " << static_cast<int>(it->first) << "\n";
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

    Vector2 newPosition = {
        mPosition.x + (mDirection.x * mSpeed * GetFrameTime()),
        mPosition.y + (mDirection.y * mSpeed * GetFrameTime())
    };

    /*
    std::cout << mName << " position (x, y)\t(" <<
        static_cast<int>(mPosition.x / TILE_SIZE) << ", " <<
        static_cast<int>(mPosition.y / TILE_SIZE) << ")\n";
    */

    int newTileX = static_cast<int>(newPosition.x / TILE_SIZE);
    int newTileY = static_cast<int>(newPosition.y / TILE_SIZE);

    if (newTileY == TUNNEL_ROW) {
        if (newTileX < 0) {
            newPosition.x = (LABYRINTH_COLS * TILE_SIZE);
        }
        if (newTileX > LABYRINTH_COLS) {
            newPosition.x = 0;
        }
    }

    mPosition = newPosition;
    mGhostSprite.update();
}