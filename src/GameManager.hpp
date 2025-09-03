#pragma once

#include <vector>

#include <raylib.h>

#include "Ghost.hpp"
#include "Pacman.hpp"

const float CHASE_TIME = 12.f;
const float POWERUP_TIME = 15.f;
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

    inline GameManager(
        std::vector< std::shared_ptr<Ghost>> g,
        std::shared_ptr<Pacman> p,
        std::shared_ptr<Labyrinth> l)
    : mGhosts(g),
      mPacman(p),
      mLabyrinth(l)

    {};
    inline ~GameManager() {};

    inline void checkCollisions() {
        Vector2 pacmanTilePosition = mPacman->getTilePosition();
        int pacmanTileX = static_cast<int>(pacmanTilePosition.x);
        int pacmanTileY = static_cast<int>(pacmanTilePosition.y);

        // pellet collisions
        if (mLabyrinth->mPellets.find({ pacmanTileX, pacmanTileY }) != mLabyrinth->mPellets.end()) {
            mLabyrinth->mPellets.erase({ pacmanTileX, pacmanTileY });
            onDotsEaten();
            std::cout << "nomnomnom\n";
        }

        // powerup collisions
        if (mLabyrinth->mPowerups.find({ pacmanTileX, pacmanTileY }) != mLabyrinth->mPowerups.end()) {
            mLabyrinth->mPowerups.erase({ pacmanTileX, pacmanTileY });
            onPowerUpEaten();
        }

        // ghost collisions
        for (const auto& ghost : mGhosts) {
            Vector2 ghostTile = ghost->getTilePosition();

            if (pacmanTilePosition.x == ghostTile.x && pacmanTilePosition.y == ghostTile.y) {
                if (ghost->getState() == Ghost::State::FRIGHTENED) {
                    ghost->setState(Ghost::State::GOING_TO_PRISON);
                }
                else {
                    onDeath();
                }
            }
            
            if (ghostTile.x == 11 && ghostTile.y == 8 && ghost->mState == Ghost::State::LEAVING_PRISON) {
                std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
                ghost->setState(Ghost::State::CHASE);
                ghost->setChaseTarget(mPacman->mPosition);
            } 

            if (ghostTile.x >= 1 && ghostTile.x <= 10 &&
                ghostTile.y >= 1 && ghostTile.y <= 10 &&
                ghost->getState() == Ghost::State::GOING_TO_PRISON) {
            } 
        }
    };

    inline void drawStuff() {
        mPacman->draw();
        mLabyrinth->draw();
        for (const auto& ghost : mGhosts) { ghost->draw(); }
    }

    inline int getScore() const { return mScore; }
    inline int getLives() const { return mPacmanLives; }

    inline void moveStuff() {
        Vector2 intendedDirection = { 0.f, 0.f };
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            intendedDirection = { 0.f, 1.f };
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            intendedDirection = { 0.f, -1.f };
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            intendedDirection = { 1.f, 0.f };
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            intendedDirection = { -1.f, 0.f };
        }

        mPacman->move(intendedDirection, mLabyrinth);
        for (const auto& ghost : mGhosts) { ghost->act(mLabyrinth); }
    }

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
            for (auto &ghost : mGhosts) {
                if (ghost->getState() == Ghost::State::IN_PRISON) {
                    ghost->setChaseTarget({ 11.f * TILE_SIZE, 8.f * TILE_SIZE });
                    ghost->setState(Ghost::State::LEAVING_PRISON);
                }
            }
        }

        if (mScoreExtraLife > 100) {
            mPacmanLives++;
            mScoreExtraLife = 0;
        }
    };

    inline void onGhostsStartChasing() {
        for (auto &ghost : mGhosts) {
            ghost->setChaseTarget(mPacman->mPosition);
            ghost->setState(Ghost::State::CHASE);
            ghost->updateSprite();
        }
    }

    inline void onPowerUpEaten() {
        mState = State::PLAYING_POWERUP;
        mPowerUpTime = true;
        for (auto &ghost : mGhosts) {
            if (ghost->getState() != Ghost::State::GOING_TO_PRISON) {
                ghost->setChaseTarget(ghost->mScatterCorner);
                ghost->setState(Ghost::State::FRIGHTENED);
                ghost->updateSprite();
            }
        }
    };

    inline void onPowerUpWearsOff() {
        mState = State::PLAYING;
        mPowerUpTime = false;
        onGhostsStartChasing();
    }

    inline void onToggleGhostMode() {
        for (auto &ghost : mGhosts) {
            if (ghost->getState() == Ghost::State::CHASE) {
                ghost->setState(Ghost::State::SCATTER);
            } else if (ghost->getState() == Ghost::State::SCATTER) {
                ghost->setChaseTarget(mPacman->mPosition);
                ghost->setState(Ghost::State::CHASE);
            }
        }
    }

    inline void startGame() {
        mPacmanLives = 3;
        mScoreExtraLife = 0;
        mScore = 0;
        for (auto &ghost : mGhosts) {
            if (ghost->getName() == "Blinky") { 
                ghost->setChaseTarget({ 11.f * TILE_SIZE, 8.f * TILE_SIZE });
                ghost->setState(Ghost::State::LEAVING_PRISON);
            }
        }
    };

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
    std::shared_ptr<Labyrinth>          mLabyrinth;
    std::vector<std::shared_ptr<Ghost>> mGhosts;
};