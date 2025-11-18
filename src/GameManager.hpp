#pragma once

#include <iomanip>
#include <sstream>
#include <vector>

#include <raylib.h>

#include "DebugUtils.hpp"
#include "Ghost.hpp"
#include "Pacman.hpp"

using std::shared_ptr;

const float CHASE_TIME = 1000.f;
const float POWERUP_TIME = 15.f;
const float SCATTER_TIME = 55.f;
const float MAX_PRISON_TIME = 8.f;

struct MenuEntity {
    bool draw;
    std::string name;
    Vector2 position;
    AnimatedSprite* sprite;
};

struct AnimatedEntity : public MenuEntity {
    float t, speed;
    Vector2 initialPosition;

    AnimatedEntity(bool d, std::string n, Vector2 p, AnimatedSprite* s, float t, float sp, Vector2 ip):
    MenuEntity{d, n, p, s}, t(t), speed(sp), initialPosition(ip) {}
};

class GameManager {
public:
    enum class State {
        MENU            = 0,
        PLAYING         = 1,
        PLAYING_POWERUP = 2,
        PACMAN_DYING    = 3,
        PAUSED          = 4,
        GAME_OVER       = 5,
        GAME_WON        = 6,
        GAME_START      = 7
    };

    inline GameManager(std::vector< shared_ptr<Ghost>> g, shared_ptr<Pacman> p, shared_ptr<Labyrinth> l)
        : mGhosts(g)
        , mHudFont(LoadFont("res/Bitty.ttf"))
        , mHudFont1(LoadFont("res/PublicPixel.ttf"))
        , mHudFont2(LoadFont("res/zector.regular.ttf"))
        , mMenuPacman("res/pacman.png", 26, 26, 3, 10)
        , mMenuInky("res/inky.png", 26, 26, 2, 4)
        , mMenuBlinky("res/blinky.png", 26, 26, 2, 4)
        , mMenuPinky("res/pinky.png", 26, 26, 2, 4)
        , mMenuClyde("res/clyde.png", 26, 26, 2, 4)
        , mMenuPellet("res/pellet.png", 26, 26, 2, 10)
        , mMenuPowerUp("res/powerup.png", 26, 26, 2, 12)
        , mPacman(p)
        , mPacmanGuy()
        , mLabyrinth(l)
        , mGhostStartingPoint({ 11.f * TILE_SIZE, 8.f * TILE_SIZE }) {

        mPacmanGuy = LoadTexture("res/pacman.png");
    };

    inline ~GameManager() {};

    inline void checkCollisions() {
        auto pacmanTile = mPacman->getTilePosition();

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

            // pacman eats ghost
            if (pacmanTile.first == ghostTile.first && pacmanTile.second == ghostTile.second) {
                if (ghost->getState() == Ghost::State::CHASE ||
                    ghost->getState() == Ghost::State::MEANDER ||
                    ghost->getState() == Ghost::State::SCATTER) {
                        onDeath();
                    }
                if (ghost->getState() == Ghost::State::FRIGHTENED) {
                    ghost->setState(Ghost::State::GOING_TO_PRISON);
                    ghost->setChaseTarget(mGhostStartingPoint);
                    ghost->updateSprite();
                    mScore += 100;
                }
            }
            
            int ghostStartingTileX = static_cast<int>(mGhostStartingPoint.x / TILE_SIZE);
            int ghostStartingTileY = static_cast<int>(mGhostStartingPoint.y / TILE_SIZE);

            // ghost emerges from prison
            if (ghostTile.first == ghostStartingTileX && ghostTile.second == ghostStartingTileY) {
                if (ghost->getState() == Ghost::State::LEAVING_PRISON) {
                    ghost->setChaseTarget(mPacman->getPosition());
                    ghost->setState(Ghost::State::CHASE);
                } 
                if (ghost->getState() == Ghost::State::GOING_TO_PRISON) {
                    ghost->setChaseTarget(ghost->mPrisonPosition);
                }
                ghost->resetDecisionTile();
            }

            // ghost entered prison after death, now needs to leave
            if (isGhostInPrison(ghost) && ghost->getState() == Ghost::State::GOING_TO_PRISON) {
                ghost->setChaseTarget(mGhostStartingPoint);
                ghost->setState(Ghost::State::LEAVING_PRISON);
                ghost->resetDecisionTile();
                ghost->updateSprite();
            } 
        }
    };

    inline void drawStuff() {
        mLabyrinth->draw();
        for (const auto& ghost : mGhosts) { 
            ghost->draw();
            // ghost->drawDebugDistances();
        }
        mPacman->draw();

        std::stringstream ss;

        auto font = mHudFont2;

        ss << "Score: " << mScore;
        Vector2 scoreHudPos = { 0, TILE_SIZE * LABYRINTH_ROWS + 10.f };
        DrawTextEx(font, ss.str().c_str(), scoreHudPos, 32, 2, WHITE);
        ss.str("");
        ss.clear();

        float pacmanGuysHudPosX = TILE_SIZE * LABYRINTH_COLS - (mPacmanLives * TILE_SIZE);
        float pacmanGuysHudPosY = TILE_SIZE * LABYRINTH_ROWS + 10.f;

        ss << "Lives: ";
        DrawTextEx(font, ss.str().c_str(), Vector2{ pacmanGuysHudPosX - 100, pacmanGuysHudPosY }, 32, 2, WHITE);

        Rectangle pacmanGuyRect = { 0, 0, TILE_SIZE, TILE_SIZE };
        for (int guy = 0; guy < mPacmanLives; guy++) {
            DrawTextureRec(mPacmanGuy, pacmanGuyRect, Vector2{ pacmanGuysHudPosX + (guy * TILE_SIZE), pacmanGuysHudPosY}, WHITE);
        }
    }

    inline int getScore() const { return mScore; }
    inline int getLives() const { return mPacmanLives; }

    inline void initEntities() {
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
 
    inline bool isGhostInPrison(shared_ptr<Ghost> ghost) {
        auto ghostTile = ghost->getTilePosition();
        return ghostTile.first >= 8 && ghostTile.first <= 14 && ghostTile.second >= 11 && ghostTile.second <= 13;
    }

    inline void menuModal(std::vector<AnimatedEntity> splashScreenEntities, std::vector<MenuEntity> splashScreenFixtures) {
        BeginBlendMode(BLEND_ALPHA);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 0, 0, 0, 133 });

        int x = GetScreenWidth() / 10;
        int y = GetScreenHeight() / 3;
        int modalWidth = GetScreenWidth() - 2*x;
        int modalHeight = y;

        DrawRectangle(x, y, modalWidth, modalHeight, BLACK);
        DrawRectangleLines(x, y, modalWidth, modalHeight, Color{ 0, 150, 255, 255 });

        // if we try to left/right center justify we'd have to get each character in hud message,
        // multiply by number of characters (don't forget spacing), subtract from total width,
        // divide by 2, and use that final value as padding
        std::string menuHudMsg = "Pacman!";
        int fontSize = 32;
        int fontSpacing = 2;
        auto margin = (modalWidth - (menuHudMsg.size() * (fontSize + fontSpacing))) / 2;

        Vector2 msgPosition = { static_cast<float>(x + margin), static_cast<float>(y + (modalHeight / 12)) };
        DrawTextEx(mHudFont1, menuHudMsg.c_str(), msgPosition, fontSize, fontSpacing, GetColor(0x5CBBFFFF));

        menuHudMsg = "Press any key to start...";
        fontSize = 16;
        fontSpacing = 1;
        margin = (modalWidth - (menuHudMsg.size() * (fontSize + fontSpacing))) / 2;

        msgPosition = { static_cast<float>(x + margin), msgPosition.y + 32 + 12 };
        DrawTextEx(mHudFont1, menuHudMsg.c_str(), msgPosition, fontSize, fontSpacing, GetColor(0x5CBBFFFF));

        BeginScissorMode(x, y, modalWidth, modalHeight);

        for (auto& entity : splashScreenEntities) {
            entity.sprite->draw(entity.position);
        }
        for (auto& entity : splashScreenFixtures) {
            entity.sprite->draw(entity.position);
        }

        EndScissorMode();

        for (auto& entity : splashScreenEntities) {
            if (entity.name != "pacman") {
                entity.sprite->setZeroFrame(2);
            }
            entity.sprite->update();
        }

        EndBlendMode();
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
        mState = State::PACMAN_DYING;
        mPacmanLives--;
        mPacman->setState(Pacman::State::DYING);

        if (mPacmanLives < 0) {
            mState = State::GAME_OVER;
        }
    };

    inline void onDotsEaten() {
        mScore += 10;
        mScoreExtraLife++;
        mDotsEaten++;

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
                ghost->getState() != Ghost::State::IN_PRISON &&
                ghost->getState() != Ghost::State::LEAVING_PRISON) {
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

    inline void resetGame() {
        mPacman->setState(Pacman::State::PLAYING);
        mPacman->resetPosition();

        for (auto &ghost : mGhosts) {
            ghost->setState(Ghost::State::IN_PRISON);
            if (ghost->getName() == "Blinky") { ghost->setState(Ghost::State::CHASE); }
            if (ghost->getName() == "Pinky")  { ghost->setState(Ghost::State::LEAVING_PRISON); }
            ghost->resetPosition();
            ghost->resetDecisionTile();
            ghost->updateSprite();
        }
    }

    inline void runGame() {

        std::vector<MenuEntity> splashScreenFixtures = {
            { true, "pellet0", Vector2{ 400.f, 370.f }, &mMenuPellet },
            { true, "pellet1", Vector2{ 400.f, 370.f }, &mMenuPellet },
            { true, "pellet2", Vector2{ 400.f, 370.f }, &mMenuPellet },
            { true, "powerup", Vector2{ 400.f, 370.f }, &mMenuPowerUp }
        };
        
        std::vector<AnimatedEntity> splashScreenAnimations = {
            { true, "pacman", Vector2{ 0.f, 0.f }, &mMenuPacman, 0.f, .1f,   Vector2{ 0.f, 0.f } },
            { true, "blinky", Vector2{ 0.f, 0.f }, &mMenuBlinky, 0.f, .082f, Vector2{ -2.f * 26.f, 0.f } },
            { true, "inky",   Vector2{ 0.f, 0.f }, &mMenuInky,   0.f, .076f, Vector2{ -3.f * 26.f, 0.f } },
            { true, "pinky",  Vector2{ 0.f, 0.f }, &mMenuPinky,  0.f, .09f,  Vector2{ -4.f * 26.f, 0.f } },
            { true, "clyde",  Vector2{ 0.f, 0.f }, &mMenuClyde,  0.f, .08f,  Vector2{ -4.f * 26.f, 0.f } }
        };

        while (WindowShouldClose() == false) {
            if (IsKeyPressed(KEY_P)) {
                mPaused = !mPaused;
            }

            BeginDrawing();
            ClearBackground(BLACK);

            if (mPaused == false && mState != State::PACMAN_DYING) {
                moveStuff();

                checkCollisions();

                updateTimers();
            }

            if (mState == State::PACMAN_DYING) {
                if (mPacman->finishedDying()) {
                    mState = State::PAUSED;
                    resetGame();
                }
            }

            if (mState == State::GAME_OVER) {
                std::cout << "game over baby\n";
            }

            drawStuff();

            if (mState == State::MENU) {
                Vector2 animPathStart = Vector2{ -15.f, 375.f };
                Vector2 animPathEnd = Vector2{ 650.f, 375.f };
                
                for (auto& entity : splashScreenAnimations) {
                    entity.t += entity.speed * GetFrameTime();
                    if (entity.t > 1.f) { entity.t = 0.f; }
                    if (entity.t < 0.) { entity.t = 0.f; }
                    entity.position = Vector2{
                        entity.initialPosition.x + animPathStart.x + (animPathEnd.x - animPathStart.x) * entity.t,
                        entity.initialPosition.y + animPathStart.y + (animPathEnd.y - animPathStart.y) * entity.t
                    };

                    if (entity.name == "pacman" && entity.t >= .5f) {
                        for (auto& p : splashScreenFixtures) {
                            if (std::abs(entity.position.x - p.position.x) < 4.f) {
                                if (p.name == "powerup") {
                                    entity.t -= entity.speed * GetFrameTime();
                                }
                                p.draw = false;
                            }
                        }
                    }
                }

                for (auto& entity : splashScreenFixtures) {
                    if (entity.draw == false) { continue; }
                    
                }

                mPaused = true;
                menuModal(splashScreenAnimations, splashScreenFixtures);
            }

            EndDrawing();
        }
    }

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

    bool    mPaused = false;

private:
    // things to keep track of
    int     mDotsEaten = 0;
    Vector2 mGhostStartingPoint;
    int     mPowerUpsEaten = 0;
    bool    mPowerUpTime = false;
    int     mPacmanLives = 3;
    float   mPacmanSpeed = 50.f;
    Texture mPacmanGuy;
    int     mScore = 0;
    int     mScoreExtraLife = 0;
    State   mState = State::MENU;
    float   mTimerChaseMode = 0.f;
    float   mTimerPowerUp = 0.f;
    float   mTimerLeavePrison = 0.f;

    AnimatedSprite mMenuPacman;
    AnimatedSprite mMenuInky;
    AnimatedSprite mMenuPinky;
    AnimatedSprite mMenuBlinky;
    AnimatedSprite mMenuClyde;
    AnimatedSprite mMenuPellet;
    AnimatedSprite mMenuPowerUp;

    Font    mHudFont;
    Font    mHudFont1;
    Font    mHudFont2;

    shared_ptr<Pacman>             mPacman;
    shared_ptr<Labyrinth>          mLabyrinth;
    std::vector<shared_ptr<Ghost>> mGhosts;
};