#pragma once

#include "helpers/AnimatedSprite.hpp"
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

    void act(const Labyrinth &);
    void draw();
    void meander(const Labyrinth &);

    Vector2        mDirection;
    AnimatedSprite mGhostSprite;
    Vector2        mPosition;
    float          mSpeed;
    State          mState;
     
};

/*
#include <list>
#include <memory>
#include <random>
#include <string>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

#include <raylib.h>

class Ghost {
public:
    // ctors with one callable arg should be marked explicit
    explicit Ghost(float s, sf::Vector2f p, sf::Color c, const Labyrinth &rl);
    ~Ghost();

    enum class State {
        CHASE = 0,
        FRIGHTENED = 1,
        MEANDER = 2,
        SCATTER = 3,
    };

    friend std::ostream &operator<<(std::ostream &os, State s) {
        switch (s)
        {
        case State::CHASE:
            os << "CHASE";
            break;
        case State::FRIGHTENED:
            os << "FRIGTHENED";
            break;
        case State::MEANDER:
            os << "MEANDER";
            break;
        case State::SCATTER:
            os << "SCATTER";
            break;
        default:
            os << "BUPKIS";
        }
        return os;
    }

    void act();
    void changeDirection(Direction);
    bool checkAndSnapToTile();
    void draw(std::shared_ptr<sf::RenderWindow>);
    std::list<sf::Vector2f> getPath() const { return std::list<sf::Vector2f>(mPath); }
    sf::Vector2f getPosition();
    State getState() const { return mState; }
    sf::Vector2f getTarget() const { return mTarget; }
    bool hasLeftCurrentTile();
    void resetPath();
    void setState(State s) { mState = s; }
    void setFrightenedTarget(sf::Vector2f pos) { mFrightenedTarget = pos; }
    void setChaseSpeed(float s) { mChaseSpeed = s; }
    void setMeanderSpeed(float s) { mMeanderSpeed = s; }
    void setTarget(sf::Vector2f t) { mTarget = t; }

    // conversion methods so we don't have to write a getter for
    // the ghost shape when we use the collides(sf::Shape, sf::Shape)
    // method
    operator sf::Shape &() { return mGhostShape; }
    operator const sf::Shape &() const { return mGhostShape; }

private:
    void chase();
    void meander();
    void scatter();

    float mChaseSpeed;
    sf::Color mColor;
    sf::Vector2f mDirection;
    bool mDebugMode;
    float mFrightenedSpeed;
    sf::Vector2f mFrightenedTarget;
    sf::RectangleShape mGhostShape;
    sf::Vector2f mInitialPosition;
    float mMeanderOdds;
    float mMeanderSpeed;
    sf::Vector2f mMovement;
    std::list<sf::Vector2f> mPath;
    std::shared_ptr<sf::RenderWindow> mPGameWindow;
    const Labyrinth &mRLabyrinth;
    std::mt19937 mRandGenerator;
    unsigned mSeed;
    State mState;
    sf::Vector2f mTarget;

    struct TileScore {
        int positionOffset;
        int score;
        TileScore(int p, int s) : positionOffset(p),
                                  score(s)
        {
        }
    };

    struct OrderByScore {
        bool operator()(const TileScore &a, const TileScore &b) const
        {
            return a.score > b.score;
        }
    };
};
*/
