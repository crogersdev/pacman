#include "Ghost.hpp"

#include <raymath.h>

Ghost::Ghost(std::string name, std::string texture, Vector2 initTilePos, Vector2 scatterTilePos) 
    :
      mChaseSpeed(40.f),
      mChaseTarget({}),
      mDirection{1.f, 0.f},
      mFrightenedSpeed(30.f),
      mGen(std::random_device{}()),
      mGhostSprite(texture, 26, 26, 2, 4),
      mGhostTexture(texture),
      mName(name),
      mPrisonSpeed(55.f),
      mPrisonPosition(Vector2{ 12 * TILE_SIZE - TILE_SIZE / 2, 12 * TILE_SIZE - TILE_SIZE / 2}),
      mPosition({ initTilePos.x * TILE_SIZE - TILE_SIZE / 2.f, initTilePos.y * TILE_SIZE - TILE_SIZE / 2.f }),
      mScatterCornerPosition({ scatterTilePos.x * TILE_SIZE - TILE_SIZE / 2.f, scatterTilePos.y * TILE_SIZE - TILE_SIZE / 2 }),
      mSpeed(40.f),
      mState(State::SCATTER) {
}

Ghost::~Ghost() {}

void Ghost::act(std::shared_ptr<Labyrinth> labyrinth) {
    
    if (mName == "Blinky") {
        std::cout << "Blinky's state: " << mState << "\n";
        std::cout << "Blinky's tile target (x, y): (" << 
            mChaseTarget.x / TILE_SIZE << ", " <<
            mChaseTarget.y / TILE_SIZE << ")\n";
    }
    

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

void Ghost::chase(std::shared_ptr<Labyrinth> labyrinth) {
    Vector2 target = mChaseTarget;
    if (mState == State::SCATTER) {
        target = mScatterCornerPosition;
    } else if (mState == State::GOING_TO_PRISON) {
        target = mPrisonPosition;
    }

    auto availableTurns = getAvailableTurns(labyrinth);

    if (isCentered()) {
        Vector2 turn = { 0.f , 0.f };
        int bestDistance = 1000;

        for (const auto& t: availableTurns ) {
            Vector2 potentialPosition = { mPosition.x + t.second.x * TILE_SIZE, mPosition.y + t.second.y * TILE_SIZE };
            int d = computeTileDistance(potentialPosition, target);
            if (d < bestDistance) {
                bestDistance = d;
                turn = t.second;
            }
        }

        mDirection = turn;
    }

    updateSpriteFrameAndMove();
}

void Ghost::draw() {
    mGhostSprite.draw(mPosition);
}

bool Ghost::isCentered() {
    const float ALIGNMENT_THRESHOLD = 1.5f;
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
    return availableTurns;
}

Vector2 Ghost::getTilePosition() const {
    return Vector2{ 
        std::round(mPosition.x / TILE_SIZE),
        std::round(mPosition.y / TILE_SIZE)
    };
}

void Ghost::meander(std::shared_ptr<Labyrinth> labyrinth) {
    if (isCentered()) {
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

    updateSpriteFrameAndMove();
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