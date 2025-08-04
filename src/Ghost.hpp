#pragma once

#include <random>

#include "helpers/AnimatedSprite.hpp"
#include "helpers/Movement.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>

class Ghost {
public:
    enum class State {
        CHASE      = 0,
        FRIGHTENED = 1,
        MEANDER    = 2,
        SCATTER    = 3,
    };

    Ghost(std::string, Vector2);
    ~Ghost();

    void    act(const Labyrinth &, const Vector2);
    void    chase(const Labyrinth &, const Vector2);
    void    draw();
    bool    isCentered();
    Vector2 getTilePosition() const;
    void    meander(const Labyrinth &);

    Vector2        mDirection;
    std::mt19937   mGen;
    AnimatedSprite mGhostSprite;
    Vector2        mPosition;
    float          mSpeed;
    State          mState;

private:
    std::map<Direction, Vector2> getAvailableTurns(const Labyrinth&);
    int  computeTileDistance(Vector2 start, Vector2 end) { return static_cast<int>((std::abs(end.y - start.y) + std::abs(end.x - start.x )) / TILE_SIZE); }
    void updateSpriteFrameAndMove(); 

};
