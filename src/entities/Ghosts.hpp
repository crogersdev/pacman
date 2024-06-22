#pragma once

#include <memory>
#include <random>

#include "./Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

#include <SFML/Graphics.hpp>

/*

frontier = PriorityQueue()
frontier.put(start, 0)
came_from = dict()
cost_so_far = dict()
came_from[start] = None
cost_so_far[start] = 0

while not frontier.empty():
   current = frontier.get()

   if current == goal:
      break
   
   for next in graph.neighbors(current):
      new_cost = cost_so_far[current] + graph.cost(current, next)
      if next not in cost_so_far or new_cost < cost_so_far[next]:
         cost_so_far[next] = new_cost
         priority = new_cost + heuristic(goal, next)
         frontier.put(next, priority)
         came_from[next] = current 

*/

class Ghost {
public:
  // ctors with one callable arg should be marked explicit
  explicit Ghost(float, bool = false);
  ~Ghost();

  float mMeanderOdds;

  void chase(const Labyrinth &, sf::Vector2f);
  void changeDirection(Direction);
  void draw(std::shared_ptr<sf::RenderWindow>);
  sf::Vector2f getPosition();
  void meander(const Labyrinth &);
  void scatter();

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

  bool mDebugMode;
  float mSpeed;
  sf::RectangleShape mGhostShape;
  sf::Vector2f mMovement;
  sf::Vector2f mPosition;
  Direction mDirection;
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
      return a.score < b.score;
    }
  };

};
