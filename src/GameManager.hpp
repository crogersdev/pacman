#pragma once

#include <raylib.h>

class GameManager {
public:
    enum class State {
        MENU            = 0,
        PLAYING         = 1,
        PLAYING_POWERUP = 2,
        GAME_OVER       = 3,
        GAME_WON        = 4,
    };

    GameManager();
    ~GameManager();

    void onDeath() {
        mPacmanLives--;

        if (mPacmanLives < 0) {
            mState = State::GAME_OVER;
        }
    };

    void onDotsEaten() {
        mScore += 10;
        mScoreExtraLife++;
        mDotsEaten++;

        if (mScoreExtraLife > 100) {
            mPacmanLives++;
            mScoreExtraLife = 0;
        }
    };

    void onPowerUpEaten() {
        mState = State::PLAYING_POWERUP;
    };

    void onStartGame() {
        mPacmanLives = 3;
        mScoreExtraLife = 0;
        mScore = 0;
    };

private:
    // things to keep track of
    int    mDotsEaten = 0;
    int    mPowerUpsEaten = 0;
    int    mPacmanLives = 3;
    float  mPacmanSpeed = 50.f;
    bool   mPaused = false;
    int    mScore = 0;
    int    mScoreExtraLife = 0;
    State  mState = State::MENU;
};