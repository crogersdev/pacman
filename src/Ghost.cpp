#include <iomanip>
#include <sstream>

#include "Ghost.hpp"
#include "DebugUtils.hpp"

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
            turn = t.second;
        } else if (distance == bestDistance) {
            Vector2 directionToTarget = Vector2{ target.x - mPosition.x, target.y - mPosition.y };

            // this should help us pick the direction most likely to 
            // get us closer to our target if we've got two equal distances
            // to choose from.  higher is better.
            float potentialDotProd = Vector2DotProduct(t.second, directionToTarget);
            float currentDotProd = Vector2DotProduct(turn, directionToTarget);
            if (potentialDotProd > currentDotProd) {
                turn = t.second;
            } 
        }
    }
    mDirection = turn;
}

void Ghost::draw() {
    mGhostSprite.draw(mPosition);
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
            newPosition.x = (LABYRINTH_COLS * TILE_SIZE);
        }
        if (newTileX > LABYRINTH_COLS) {
            newPosition.x = 0;
        }
    }

    mPosition = newPosition;
    mGhostSprite.update();
}