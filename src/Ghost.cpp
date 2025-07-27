#include "Ghost.hpp"

#include <raymath.h>

void chase() {
    
} 

Ghost::Ghost(std::string tp, Vector2 pos) 
    : mDirection{1.f, 0.f},
      mPosition(pos),
      mGhostSprite(tp, 26, 26, 2, 4),
      mSpeed(.5f),
      mState(State::MEANDER) {

    std::random_device rd;
    std::mt19937 mGen(rd());
}

Ghost::~Ghost() {}

void Ghost::act(const Labyrinth &rLabyrinth, const Vector2 targetPosition) {
    switch (mState) {
    case State::MEANDER:
        meander(rLabyrinth);
        break;
    case State::CHASE:
        chase(rLabyrinth, targetPosition);
        break; 
    default:
        break;
    }
}

void Ghost::chase(const Labyrinth &rLabyrinth, const Vector2 pacmanPosition) {
    auto availableTurns = getAvailableTurns(rLabyrinth);

    if (isCentered()) {
        Vector2 turn = { 0.f , 0.f };
        int bestDistance = 1000;

        for (const auto& t: availableTurns ) {
            Vector2 potentialPosition = { mPosition.x + t.second.x * TILE_SIZE, mPosition.y + t.second.y * TILE_SIZE };
            int d = computeTileDistance(potentialPosition, pacmanPosition);
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

std::map<Direction, Vector2> Ghost::getAvailableTurns(const Labyrinth &rLabyrinth) {
    int currentTileX = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentTileY = static_cast<int>(mPosition.y / TILE_SIZE);

    auto availableTurns(directionLut);
    for (int i = 0; i < 4; ++i) {
        Direction d = static_cast<Direction>(i);
        Vector2 intendedTile = {
            currentTileX + directionLut.at(d).x,
            currentTileY + directionLut.at(d).y
        };
        Labyrinth::Tile tile = rLabyrinth.at(intendedTile.y, intendedTile.x);
        switch (tile) {
        case Labyrinth::Tile::WALL:
        case Labyrinth::Tile::GATE:
            availableTurns.erase(d);
            break;
        default:
        }
    }
    return availableTurns;
}

void Ghost::meander(const Labyrinth &rLabyrinth) {

    if (isCentered()) {
        auto availableTurns = getAvailableTurns(rLabyrinth);

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

void Ghost::updateSpriteFrameAndMove() {
    if (mDirection == Vector2{  0.f, -1.f }) { mGhostSprite.setZeroFrame(0); }
    if (mDirection == Vector2{  0.f,  1.f }) { mGhostSprite.setZeroFrame(6); }
    if (mDirection == Vector2{ -1.f,  0.f }) { mGhostSprite.setZeroFrame(4); }
    if (mDirection == Vector2{  1.f,  0.f }) { mGhostSprite.setZeroFrame(2); }

    float dt = GetFrameTime();
    mPosition = Vector2Add(mPosition, Vector2Scale(mDirection, mSpeed));
    mGhostSprite.update();

}