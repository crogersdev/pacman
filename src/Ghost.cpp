#include <random>

#include "Ghost.hpp"
#include "helpers/Movement.hpp"

#include <raymath.h>

void chase() {
    
} 

Ghost::Ghost(std::string tp, Vector2 pos) 
    : mDirection{1.f, 0.f},
      mPosition(pos),
      mGhostSprite(tp, 26, 26, 2, 4),
      mSpeed(.5f),
      mState(State::MEANDER) {

}

Ghost::~Ghost() {}

void Ghost::act(const Labyrinth &rLabyrinth) {
    switch (mState) {
    case State::MEANDER:
        meander(rLabyrinth);
        break;
    default:
        break;
    }

}

void Ghost::draw() {
    mGhostSprite.draw(mPosition);
}

void Ghost::meander(const Labyrinth &rLabyrinth) {
    if (mDirection == Vector2{  0.f, -1.f }) { mGhostSprite.setZeroFrame(0); }
    if (mDirection == Vector2{  0.f,  1.f }) { mGhostSprite.setZeroFrame(6); }
    if (mDirection == Vector2{ -1.f,  0.f }) { mGhostSprite.setZeroFrame(4); }
    if (mDirection == Vector2{  1.f,  0.f }) { mGhostSprite.setZeroFrame(2); }

    int intendedTileX = static_cast<int>(mPosition.x / TILE_SIZE + mDirection.x);
    int intendedTileY = static_cast<int>(mPosition.y / TILE_SIZE + mDirection.y);

    Vector2 intendedPosition = { intendedTileX * TILE_SIZE, intendedTileY * TILE_SIZE };
    Rectangle intendedRect = { intendedPosition.x, intendedPosition.y, TILE_SIZE, TILE_SIZE };
    Rectangle ghostRect = { 
        mPosition.x - (TILE_SIZE / 2),
        mPosition.y - (TILE_SIZE / 2),
        TILE_SIZE,
        TILE_SIZE
    };

    int tileX = static_cast<int>(mPosition.x / TILE_SIZE);
    int tileY = static_cast<int>(mPosition.y / TILE_SIZE);
    Rectangle r;
    auto availableTurns(directionLut);
    // what was i doing here
    // availableTurns.erase()
    for (int i = 0; i < 4; ++i) {
        Direction d = static_cast<Direction>(i);
        auto potentialTileCol = directionLut.at(d).x + tileX;
        auto potentialTileRow = directionLut.at(d).y + tileY;
        Labyrinth::Tile tile = rLabyrinth.at(potentialTileRow, potentialTileCol);
        switch (tile) {
        case Labyrinth::Tile::WALL:
        case Labyrinth::Tile::GATE:
            availableTurns.erase(d);
            break;
        default:
            potentialTileRow *= TILE_SIZE;
            potentialTileCol *= TILE_SIZE;
            r = { potentialTileCol, potentialTileRow, TILE_SIZE, TILE_SIZE };
            // DrawRectangle(potentialTileCol, potentialTileRow, TILE_SIZE, TILE_SIZE, MAROON);
            // DrawRectangle(mPosition.x - (TILE_SIZE / 2), mPosition.y - (TILE_SIZE / 2), TILE_SIZE, TILE_SIZE, MAGENTA);
            if (CheckCollisionRecs(r, ghostRect)) availableTurns.erase(d);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);

    if (availableTurns.size() > 2) {
        auto it = availableTurns.begin();
        std::advance(it, dis(gen) % availableTurns.size());
        mDirection = it->second;
        std::cout << "changing direction to " << static_cast<int>(it->first) << "\n";
        float dt = GetFrameTime();
        mPosition = Vector2Add(mPosition, Vector2Scale(mDirection, mSpeed));
        return;
    }

    Labyrinth::Tile tile = rLabyrinth.at(intendedPosition);
    switch (tile) {
    case Labyrinth::Tile::WALL:
    case Labyrinth::Tile::GATE:
        if (CheckCollisionRecs(intendedRect, ghostRect)) {
            // DrawRectangle(intendedRect.x, intendedRect.y, 26, 26, ORANGE);
            // DrawRectangle(ghostRect.x, ghostRect.y, 26, 26, GREEN);
            mDirection = directionLut.at(static_cast<Direction>(dis(gen)));
            return;
        }
        break;
    default:  // unnecessary, but it turns off a warning.
        break;
    }

    // if available turn pick at random to change direction and set as new direction
    // move in new direction
    float dt = GetFrameTime();
    mPosition = Vector2Add(mPosition, Vector2Scale(mDirection, mSpeed));

    // std::cout << "direction: [" << mDirection.x << ", " << mDirection.y << "]  position: (" << mPosition.x << ", " << mPosition.y << ")\n";

    mGhostSprite.update();
}
