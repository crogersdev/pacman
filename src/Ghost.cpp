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
    // random number get new direction
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 3);

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

    Labyrinth::Tile tile = rLabyrinth.at(intendedPosition);
    switch (tile) {
    case Labyrinth::Tile::WALL:
    case Labyrinth::Tile::GATE:
        if (CheckCollisionRecs(intendedRect, ghostRect)) {
            // DrawRectangle(intendedRect.x, intendedRect.y, 26, 26, ORANGE);
            // DrawRectangle(ghostRect.x, ghostRect.y, 26, 26, GREEN);
            mDirection = directionLut[dis(gen)];
            return;
        }
        break;
    default:
        break;
    }

    // if available turn pick at random to change direction and set as new direction
    // move in new direction
    float dt = GetFrameTime();
    mPosition = Vector2Add(mPosition, Vector2Scale(mDirection, mSpeed));

    // std::cout << "direction: [" << mDirection.x << ", " << mDirection.y << "]  position: (" << mPosition.x << ", " << mPosition.y << ")\n";

    mGhostSprite.update();
}
