#pragma once

#include <array>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <SFML/Graphics.hpp>

const int   LABYRINTH_ROWS = 31;
const int   LABYRINTH_COLS = 29;
const int   TUNNEL_ROW     = 14;
const float TILE_SIZE      = 25.f;  // WARNING: changing this value has different
                                    //          effects on different machines.

class Labyrinth {
public:
  enum class Tile {
    EMPTY   = ' ',
    GATE    = '-',
    WALL    = '#',
    PELLET  = '.',
    POWERUP = '@',
    PACMAN  = 'M',
    BLINKY  = 'B',
    PINKY   = 'P',
    INKY    = 'I',
    CLYDE   = 'C',
    PATH    = 'X',
    ERROR   = 'E'
  };

  // NOTE: overloaded ostream opers gotta be a 'friend' if it's in a class.  idk why.
  // NOTE: use a switch statement so you can make your enums any number value
  friend std::ostream &operator<<(std::ostream& os, Tile t) {
    switch (t) {
    case Tile::EMPTY:
      os << "EMPTY";
      break;
    case Tile::GATE:
      os << "GATE";
      break;
    case Tile::WALL:
      os << "WALL";
      break;
    case Tile::PELLET:
      os << "PELLET";
      break;
    case Tile::POWERUP:
      os << "POWERUP";
      break;
    case Tile::PACMAN:
      os << "PACMAN";
      break;
    case Tile::BLINKY:
      os << "BLINKY";
      break;
    case Tile::PINKY:
      os << "PINKY";
      break;
    case Tile::INKY:
      os << "INKY";
      break;
    case Tile::CLYDE:
      os << "CLYDE";
    case Tile::PATH:
      os << "PATH";
    case Tile::ERROR:
      os << "ERROR";
    default:
      os << "DUH";
    }
    return os;
  }

  std::map<char, Tile> m_tileLut;
  std::map<Tile, std::string> m_tileLabelLut;
  std::vector<std::pair<int, int>> mDirs = {
    std::pair<int, int>(0, 1),     // UP
    std::pair<int, int>(1, 0),     // RIGHT
    std::pair<int, int>(0, -1),    // DOWN
    std::pair<int, int>(-1, 0),    // LEFT    <--- all these are assuming that it goes col, row
  };

  float mMaxLabyrinthWidth = LABYRINTH_COLS * (TILE_SIZE - 1);
  float mMaxLabyrinthHeight = LABYRINTH_ROWS * (TILE_SIZE - 1);

  sf::RectangleShape m_wallTile;
  sf::CircleShape m_pellet;

  int getOffset(std::pair<int, int>) const;
  int getOffset(int, int) const;
  int getOffset(sf::Vector2f) const;

  int heuristic(int, int) const;
  int heuristic(sf::Vector2f, sf::Vector2f) const;
  int heuristic(std::pair<int, int>, std::pair<int, int>) const;

  int heuristicThroughTunnel(int, int) const;
  int heuristicThroughTunnel(sf::Vector2f, sf::Vector2f) const;
  int heuristicThroughTunnel(std::pair<int, int>, std::pair<int, int>) const;

  int movementCost(int, int) const;
  int movementCost(sf::Vector2f, sf::Vector2f) const;
  int movementCost(std::pair<int, int>, std::pair<int, int>) const;

  std::list<int> getNeighbors(int) const;

  std::pair<int, int> getPairFromOffset(int) const;
  std::pair<int, int> getPairFromSfVec(sf::Vector2f) const;
  sf::Vector2f getSfVecFromOffset(int) const;

  std::pair<int, int> at(sf::Vector2f) const;
  Labyrinth::Tile at(int, int) const;
  Labyrinth::Tile at(std::pair<int, int>) const;
  void draw(std::shared_ptr<sf::RenderWindow>);
  void set(sf::Vector2f, Tile);
  void set(int, int, Tile);

  Labyrinth();

private:
  std::array<std::array<char, LABYRINTH_COLS>, LABYRINTH_ROWS> m_labyrinth = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####@#",
    "#@#  #.#   #.##.#   #.#  #@#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.#####.##.#####.######",
    "     #.#####.##.#####.#     ",
    "     #.##..........##.#     ",
    "     #.##.###--###.##.#     ",
    "######.##.#------#.##.######",
    "..........#------#..........",
    "######.##.#------#.##.######",
    "     #.##.########.##.#     ",
    "     #.##..........##.#     ",
    "     #.##.########.##.#     ",
    "######.##.########.##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#",
    "#@..##................##..@#",
    "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"    // this is row 30
  };
};
