#pragma once

#include <memory>
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
        MEANDER         = 3,
        IN_PRISON       = 4,
        LEAVING_PRISON  = 5,
        GOING_TO_PRISON = 6
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
        case State::MEANDER:
            os << "MEANDER";
            break;
        case State::IN_PRISON:
            os << "IN_PRISON";
            break;
        case State::LEAVING_PRISON:
            os << "LEAVING_PRISON";
            break;
        case State::GOING_TO_PRISON:
            os << "GOING_TO_PRISON";
            break;
        default:
            os << "duh";
        }
        return os;
    }

    Ghost(std::string, std::string, Vector2, Vector2);
    ~Ghost();

    void         act(std::shared_ptr<Labyrinth>);
    void         chase(std::shared_ptr<Labyrinth>);
    void         draw();
    bool         isCentered();
    Ghost::State getState() const { return mState; }
    std::string  getName() const { return mName; }
    Vector2      getTilePosition() const;
    void         meander(std::shared_ptr<Labyrinth>);
    void         setChaseTarget(const Vector2 &t) { mChaseTarget = t; }
    void         setName(const std::string s) { mName = s; }
    void         setState(const State s) { mState = s; }
    void         updateSprite();

    float          mChaseSpeed;
    Vector2        mChaseTarget;
    Vector2        mDirection;
    float          mFrightenedSpeed;
    std::mt19937   mGen;
    AnimatedSprite mGhostSprite;
    std::string    mGhostTexture;
    Vector2        mLastDecisionTile;
    std::string    mName;
    Vector2        mPosition;
    float          mPrisonSpeed;
    Vector2        mPrisonPosition;
    Vector2        mScatterCornerPosition;
    float          mSpeed;
    State          mState;

    std::vector<std::pair<int, int>> mTurns;

private:
    std::map<Direction, Vector2> getAvailableTurns(std::shared_ptr<Labyrinth>);
    int  computeTileDistance(Vector2 start, Vector2 end) { return static_cast<int>((std::abs(end.y - start.y) + std::abs(end.x - start.x )) / TILE_SIZE); }
    void updateSpriteFrameAndMove(); 
};
