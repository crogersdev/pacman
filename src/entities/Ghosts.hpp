#pragma once

#include <list>
#include <memory>
#include <random>

#include "./Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

#include <SFML/Graphics.hpp>

class Ghost {
public:
  // ctors with one callable arg should be marked explicit
  explicit Ghost(float, sf::Vector2f, sf::Color, bool = false);
  ~Ghost();

  float mMeanderOdds;

  bool occupiesSingleTile();
  void chase(const Labyrinth &, sf::Vector2f);
  void changeDirection(Direction);
  void draw(std::shared_ptr<sf::RenderWindow>);
  sf::Vector2f getPosition();
  sf::Vector2f getTarget() const { return mTarget; }
  void meander(const Labyrinth &);
  void scatter();
  void drawPath(Labyrinth &);
  void resetPath(Labyrinth &);

  // conversion methods so we don't have to write a getter for
  // the ghost shape when we use the collides(sf::Shape, sf::Shape)
  // method
  operator sf::Shape&() { return mGhostShape; }
  operator const sf::Shape&() const { return mGhostShape; }

private:
  enum State {
    SCATTER    = 1,
    MEANDER    = 2,
    CHASE      = 3,
    FRIGHTENED = 4,
    WAIT       = 5
  };

  unsigned mSeed;
  std::mt19937 mRandGenerator;

  sf::Color mColor;
  sf::Vector2f mDirection;
  bool mDebugMode;
  float mSpeedMultiplier;
  sf::RectangleShape mGhostShape;
  sf::Vector2f mMovement;
  sf::Vector2f mInitialPosition;
  sf::Vector2f mTarget;
  std::list<sf::Vector2f> mPath;
  std::shared_ptr<sf::RenderWindow> mPGameWindow;
  State mState;

  struct TileScore {
    int positionOffset;
    int score;
    TileScore(int p, int s) :
      positionOffset(p),
      score(s)
    {}
  };

  struct OrderByScore {
    bool operator() (const TileScore &a, const TileScore &b) {
      return a.score > b.score;
    }
  };
};
