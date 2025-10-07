#pragma once

#include <memory>
#include <random>

#include "helpers/AnimatedSprite.hpp"
#include "helpers/Movement.hpp"
#include "Labyrinth.hpp"

#include <raylib.h>
#include <raymath.h>

using std::shared_ptr;

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

    Ghost(std::string, std::string, Vector2, Vector2);
    ~Ghost();

    void         act(shared_ptr<Labyrinth>);
    void         chase(shared_ptr<Labyrinth>);
    void         draw();
    bool         isCentered();
    Ghost::State getState() const { return mState; }
    std::string  getName() const { return mName; }
    Vector2      getTilePosition() const;
    void         meander(shared_ptr<Labyrinth>);
    void         resetDecisionTile() { mLastDecisionTile = Vector2{ -1.f, -1.f }; }
    void         setChaseTarget(const Vector2 &t) { mChaseTarget = t; }
    void         setName(const std::string s) { mName = s; }
    void         setSpeed(const float s) { mSpeed = s; }
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

    std::vector<std::pair<int, int>>     mTurns;
    std::vector<std::pair<Vector2, int>> mDistanceToTarget;

private:
    std::map<Direction, Vector2> getAvailableTurns(shared_ptr<Labyrinth>);
    void updateSpriteFrameAndMove(); 
};

