#pragma once

#include <vector>

#include <raylib.h>

#include "Ghost.hpp"
#include "Pacman.hpp"

const float CHASE_TIME = 12.f;
const float POWERUP_TIME = 4.f;
const float SCATTER_TIME = 3.f;

class GameManager {
public:
    enum class State {
        MENU            = 0,
        PLAYING         = 1,
        PLAYING_POWERUP = 2,
        GAME_OVER       = 3,
        GAME_WON        = 4,
    };

    inline GameManager(std::vector< std::shared_ptr<Ghost>> g, std::shared_ptr<Pacman> p)
    : mGhosts(g),
      mPacman(p)
    {};
    inline ~GameManager() {};

    inline int getScore() const { return mScore; }
    inline int getLives() const { return mPacmanLives; }

    inline void onDeath() {
        mPacmanLives--;

        if (mPacmanLives < 0) {
            mState = State::GAME_OVER;
        }
    };

    inline void onDotsEaten() {
        mScore += 10;
        mScoreExtraLife++;
        mDotsEaten++;
        std::cout << "dots eaten: " << mDotsEaten << "\n";
        std::cout << "score: " << mScore << "\n";

        if (mDotsEaten > 10) { 
            onGhostsStartChasing();
        }

        if (mScoreExtraLife > 100) {
            mPacmanLives++;
            mScoreExtraLife = 0;
        }
    };

    inline void onGhostsStartChasing() {
        for (auto &g : mGhosts) {
            g->setChaseTarget(mPacman->mPosition);
            g->mState = Ghost::State::CHASE;
            g->updateSprite();
        }
    }

    inline void onPowerUpEaten() {
        mState = State::PLAYING_POWERUP;
        mPowerUpTime = true;
        for (auto &g : mGhosts) {
            if (g->mState != Ghost::State::GOING_TO_PRISON) {
                g->setChaseTarget(g->mScatterCorner);
                g->mState = Ghost::State::FRIGHTENED;
                g->updateSprite();
            }
        }
    };

    inline void onPowerUpWearsOff() {
        mState = State::PLAYING;
        mPowerUpTime = false;
        onGhostsStartChasing();
    }

    inline void onStartGame() {
        mPacmanLives = 3;
        mScoreExtraLife = 0;
        mScore = 0;
    };

    inline void onToggleGhostMode() {
        for (auto &g : mGhosts) {
            if (g->mState == Ghost::State::CHASE) {
                g->mState = Ghost::State::SCATTER;
            } else if (g->mState == Ghost::State::SCATTER) {
                g->mState = Ghost::State::CHASE;
                g->setChaseTarget(mPacman->mPosition);
            }
        }
    }

    inline void updateTimers() {
        mTimerChase += GetFrameTime();
        if (mPowerUpTime) { mTimerPowerUp += GetFrameTime(); }
        mTimerScatter += GetFrameTime();

        if (mTimerChase >= CHASE_TIME) { 
            onToggleGhostMode();
            mTimerChase = 0.f;
        }
        if (mTimerPowerUp >= POWERUP_TIME) { 
            onPowerUpWearsOff();
            mTimerPowerUp = 0.f;
        }
        if (mTimerScatter >= SCATTER_TIME) { 
            onToggleGhostMode();
            mTimerScatter = 0.f;
        }
    }

private:
    // things to keep track of
    int    mDotsEaten = 0;
    int    mPowerUpsEaten = 0;
    bool   mPowerUpTime = false;
    int    mPacmanLives = 3;
    float  mPacmanSpeed = 50.f;
    bool   mPaused = false;
    int    mScore = 0;
    int    mScoreExtraLife = 0;
    State  mState = State::MENU;
    float  mTimerChase = 0.;
    float  mTimerPowerUp = 0.;
    float  mTimerScatter = 0.;

    std::shared_ptr<Pacman>             mPacman;
    std::vector<std::shared_ptr<Ghost>> mGhosts;
};