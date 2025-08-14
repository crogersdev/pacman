#pragma once

#include <random>

#include "helpers/AnimatedSprite.hpp"
#include "helpers/Movement.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>

class Ghost {
public:
    enum class State {
        CHASE           = 0,
        FRIGHTENED      = 1,
        SCATTER         = 2,
        GOING_TO_PRISON = 3,
        MEANDER         = 4
    };

    friend std::ostream &operator<<(std::ostream &os, State s) {
        switch(s) {
        case State::CHASE:
            os << "CHASE";
            break;
        case State::FRIGHTENED:
            os << "FRIGHTENED";
            break;
        case State::SCATTER:
            os << "SCATTER";
            break;
        case State::GOING_TO_PRISON:
            os << "GOING_TO_PRISON";
            break;
        case State::MEANDER:
            os << "MEANDER";
            break;
        default:
            os << "duh";
        }
        return os;
    }

    Ghost(std::string, Vector2, Vector2);
    ~Ghost();

    void    act(const Labyrinth &);
    void    chase(const Labyrinth &);
    void    draw();
    bool    isCentered();
    Vector2 getTilePosition() const;
    void    meander(const Labyrinth &);
    void    setChaseTarget(const Vector2 &);
    void    updateSprite();

    float          mChaseSpeed;
    Vector2        mChaseTarget;
    Vector2        mDirection;
    float          mFrightenedSpeed;
    std::mt19937   mGen;
    AnimatedSprite mGhostSprite;
    std::string    mGhostTexture;
    Vector2        mPosition;
    float          mPrisonSpeed;
    Vector2        mPrisonPosition;
    Vector2        mScatterCorner;
    float          mSpeed;
    State          mState;

private:
    std::map<Direction, Vector2> getAvailableTurns(const Labyrinth&);
    int  computeTileDistance(Vector2 start, Vector2 end) { return static_cast<int>((std::abs(end.y - start.y) + std::abs(end.x - start.x )) / TILE_SIZE); }
    void updateSpriteFrameAndMove(); 

};
