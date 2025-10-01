#pragma once

#include <iomanip>
#include <vector>

#include <raylib.h>

#include "DebugUtils.hpp"
#include "Ghost.hpp"
#include "Pacman.hpp"

const float CHASE_TIME = 45.f;
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
        std::vector< std::shared_ptr<Ghost>> g,
        std::shared_ptr<Pacman> p,
        std::shared_ptr<Labyrinth> l)
    : mGhosts(g),
      mPacman(p),
      mLabyrinth(l),
      mGhostStartingPoint({ 11.f * TILE_SIZE, 9.f * TILE_SIZE })
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

        std::string debugGhost = "Poinky";
        // ghost collisions
        for (const auto& ghost : mGhosts) {
            Vector2 ghostTile = ghost->getTilePosition();

            if (ghost->getName() == debugGhost) {
                std::cout << debugGhost << "'s state: " << ghost->getState();
                // std::cout << " -- tile target (x, y): (" << 
                //    static_cast<int>(mChaseTarget.x / TILE_SIZE) << ", " <<
                //    static_cast<int>(mChaseTarget.y / TILE_SIZE) << ")";
                std::cout << " -- tile pos (x, y): (" << ghostTile.x << ", " << ghostTile.y << ")\n";
            } 

            if (pacmanTilePosition.x == ghostTile.x && pacmanTilePosition.y == ghostTile.y) {
                if (ghost->getState() == Ghost::State::FRIGHTENED) {
                    ghost->setState(Ghost::State::GOING_TO_PRISON);
                    ghost->setChaseTarget(ghost->mPrisonPosition);
                    ghost->updateSprite();
                }
                else {
                    onDeath();
                }
            }
            
            float ghostStartingTileX = static_cast<int>(mGhostStartingPoint.x / TILE_SIZE);
            float ghostStartingTileY = static_cast<int>(mGhostStartingPoint.y / TILE_SIZE);

            if (ghostTile.x == ghostStartingTileX && ghostTile.y == ghostStartingTileY && ghost->mState == Ghost::State::LEAVING_PRISON) {
                std::cout << "LEAVE\n";
                ghost->setState(Ghost::State::CHASE);
                ghost->setChaseTarget(mPacman->getPosition());
                ghost->resetDecisionTile();
            } 

            if (isGhostInPrison(ghost) && ghost->getState() == Ghost::State::GOING_TO_PRISON) {
                std::cout << "oh no!\n";
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
    
    inline bool isGhostInPrison(std::shared_ptr<Ghost> ghost) {
        int ghostTileX = static_cast<int>(ghost->mPosition.x / TILE_SIZE);
        int ghostTileY = static_cast<int>(ghost->mPosition.y / TILE_SIZE);
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

    std::shared_ptr<Pacman>             mPacman;
    std::shared_ptr<Labyrinth>          mLabyrinth;
    std::vector<std::shared_ptr<Ghost>> mGhosts;
};