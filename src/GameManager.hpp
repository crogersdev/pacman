#pragma once

#include <iomanip>
#include <vector>

#include <raylib.h>

#include "DebugUtils.hpp"
#include "Ghost.hpp"
#include "Pacman.hpp"

using std::shared_ptr;

const float CHASE_TIME = 10.f;
const float POWERUP_TIME = 15.f;
const float SCATTER_TIME = 5.f;
const float MAX_PRISON_TIME = 8.f;

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
        std::vector< shared_ptr<Ghost>> g,
        shared_ptr<Pacman> p,
        shared_ptr<Labyrinth> l)
    : mGhosts(g),
      mPacman(p),
      mLabyrinth(l),
      mGhostStartingPoint({ 11.f * TILE_SIZE, 9.f * TILE_SIZE })
    {};
    inline ~GameManager() {};

    inline void checkCollisions() {
        
        auto pacmanTile = mPacman->getTilePosition();

        std::cout << "pacman tile x, y: " << pacmanTile.first << ", " << pacmanTile.second << "\n";

        // pellet collisions
        if (mLabyrinth->mPellets.find({ pacmanTile.first, pacmanTile.second }) != mLabyrinth->mPellets.end()) {
            mLabyrinth->mPellets.erase({ pacmanTile.first, pacmanTile.second });
            onDotsEaten();
        }

        // powerup collisions
        if (mLabyrinth->mPowerups.find({ pacmanTile.first, pacmanTile.second }) != mLabyrinth->mPowerups.end()) {
            mLabyrinth->mPowerups.erase({ pacmanTile.first, pacmanTile.second });
            onPowerUpEaten();
        }

        // ghost collisions
        for (const auto& ghost : mGhosts) {
            auto ghostTile = ghost->getTilePosition();

            std::cout << ghost->getName() << " tile x, y: " << ghostTile.first << ", " << ghostTile.second << "\n";

            if (pacmanTile.first == ghostTile.first && pacmanTile.second == ghostTile.second) {
                std::cout << "there was a collision between a ghost and pacman\n";
                if (ghost->getState() == Ghost::State::FRIGHTENED) {
                    ghost->setState(Ghost::State::GOING_TO_PRISON);
                    ghost->setChaseTarget(ghost->mPrisonPosition);
                    ghost->updateSprite();
                }
                else {
                    std::cout << "pacman just died\n";
                    onDeath();
                }
            }
            
            int ghostStartingTileX = static_cast<int>(std::round(mGhostStartingPoint.x/ TILE_SIZE));
            int ghostStartingTileY = static_cast<int>(std::round(mGhostStartingPoint.y / TILE_SIZE));

            if (ghostTile.first == ghostStartingTileX && ghostTile.second == ghostStartingTileY && ghost->mState == Ghost::State::LEAVING_PRISON) {
                ghost->setState(Ghost::State::CHASE);
                ghost->setChaseTarget(mPacman->getPosition());
                ghost->resetDecisionTile();
            } 

            if (isGhostInPrison(ghost) && ghost->getState() == Ghost::State::GOING_TO_PRISON) {
                ghost->setState(Ghost::State::LEAVING_PRISON);
                ghost->resetDecisionTile();
                ghost->updateSprite();
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
    
    inline bool isGhostInPrison(shared_ptr<Ghost> ghost) {
        int ghostTileX = static_cast<int>(std::round(ghost->mPosition.x / TILE_SIZE));
        int ghostTileY = static_cast<int>(std::round(ghost->mPosition.y / TILE_SIZE));
        return ghostTileX >= 8 && ghostTileX <= 14 && ghostTileY >= 9 && ghostTileY <= 13;
    }

    inline void moveStuff() {
        Vector2 intendedDirection = Vector2{ 0.f, 0.f };
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            intendedDirection = Vector2{ 0.f, 1.f };
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            intendedDirection = Vector2{ 0.f, -1.f };
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            intendedDirection = Vector2{ 1.f, 0.f };
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            intendedDirection = Vector2{ -1.f, 0.f };
        }

        mPacman->move(intendedDirection, mLabyrinth);
        for (const auto& ghost : mGhosts) {
            if (ghost->getState() == Ghost::State::CHASE) { ghost->setChaseTarget(mPacman->getPosition()); }
            ghost->act(mLabyrinth);
        }
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

        for (auto &ghost : mGhosts) {
            auto gs = ghost->getState();
            auto gn = ghost->getName();

            if (mDotsEaten > 30 && gs == Ghost::State::IN_PRISON && gn == "Blinky" ) {
                ghost->setChaseTarget(mGhostStartingPoint);
                ghost->setState(Ghost::State::LEAVING_PRISON);
            }
            if (mDotsEaten > 60 && gs == Ghost::State::IN_PRISON && gn == "Clyde") {
                ghost->setChaseTarget(mGhostStartingPoint);
                ghost->setState(Ghost::State::LEAVING_PRISON);
            }
        }

        if (mScoreExtraLife > 100) {
            mPacmanLives++;
            mScoreExtraLife = 0;
        }
    };

    inline void onGhostsStartChasing() {
        for (auto &ghost : mGhosts) {
            if (ghost->getState() == Ghost::State::FRIGHTENED) {
                ghost->setChaseTarget(mPacman->getPosition());
                ghost->setState(Ghost::State::CHASE);
                ghost->updateSprite();
            }
        }
    }

    inline void onPowerUpEaten() {
        mState = State::PLAYING_POWERUP;
        mPowerUpTime = true;
        for (auto &ghost : mGhosts) {
            if (ghost->getState() != Ghost::State::GOING_TO_PRISON && 
                ghost->getState() != Ghost::State::IN_PRISON) {
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

    inline void startGame() {
        mPacmanLives = 3;
        mScoreExtraLife = 0;
        mScore = 0;

        for (auto &ghost : mGhosts) {
            if (ghost->getName() == "Blinky") {
                ghost->setChaseTarget(mPacman->getPosition());
                ghost->setState(Ghost::State::CHASE);
            } else if (ghost->getName() == "Pinky") { 
                ghost->setChaseTarget(mGhostStartingPoint);
                ghost->setState(Ghost::State::LEAVING_PRISON);
            } else {
                ghost->setState(Ghost::State::IN_PRISON);
            }
        }
    };

    inline void updateTimers() {
        if (mPowerUpTime) { mTimerPowerUp += GetFrameTime(); }

        if (mTimerPowerUp >= POWERUP_TIME) { 
            onPowerUpWearsOff();
            mTimerPowerUp = 0.f;
        }
        
        mTimerChaseMode += GetFrameTime();
        mTimerLeavePrison += GetFrameTime();

        for (auto &ghost : mGhosts) {
            if (ghost->getState() == Ghost::State::CHASE && !isGhostInPrison(ghost)) {
                if (mTimerChaseMode >= CHASE_TIME) {
                    // std::cout << "chase timer: " << std::fixed << std::setprecision(2) << mTimerChaseMode << std::endl;
                    // std::cout << "toggling " << ghost->getName() << " to chasing\n";
                    ghost->setChaseTarget(ghost->mScatterCornerPosition);
                    ghost->setState(Ghost::State::SCATTER);
                    mTimerChaseMode = 0.f;
                }
            }
            if (ghost->getState() == Ghost::State::SCATTER && !isGhostInPrison(ghost)) {
                if (mTimerChaseMode >= SCATTER_TIME) {
                    // std::cout << "chase timer: " << std::fixed << std::setprecision(2) << mTimerChaseMode << std::endl;
                    // std::cout << "toggling " << ghost->getName() << " to scatter\n";
                    ghost->setChaseTarget(mPacman->getPosition());
                    ghost->setState(Ghost::State::CHASE);
                    mTimerChaseMode = 0.f;
                }
            }
            if (ghost->getState() == Ghost::State::IN_PRISON) {
                if (ghost->getName() == "Inky" && mTimerLeavePrison >= MAX_PRISON_TIME) {
                    ghost->setChaseTarget(mGhostStartingPoint);
                    ghost->setState(Ghost::State::LEAVING_PRISON);
                }
                if (ghost->getName() == "Clyde" && mTimerLeavePrison >= MAX_PRISON_TIME + 2.f) {
                    ghost->setChaseTarget(mGhostStartingPoint);
                    ghost->setState(Ghost::State::LEAVING_PRISON);
                }

            }
        }
    }

private:
    // things to keep track of
    int     mDotsEaten = 0;
    Vector2 mGhostStartingPoint;
    int     mPowerUpsEaten = 0;
    bool    mPowerUpTime = false;
    int     mPacmanLives = 3;
    float   mPacmanSpeed = 50.f;
    bool    mPaused = false;
    int     mScore = 0;
    int     mScoreExtraLife = 0;
    State   mState = State::MENU;
    float   mTimerChaseMode = 0.f;
    float   mTimerPowerUp = 0.f;
    float   mTimerLeavePrison = 0.f;

    shared_ptr<Pacman>             mPacman;
    shared_ptr<Labyrinth>          mLabyrinth;
    std::vector<shared_ptr<Ghost>> mGhosts;
};