#pragma once

#include <array>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include <SFML/Graphics.hpp>

const int   LABYRINTH_ROWS = 31;
const int   LABYRINTH_COLS = 29;
const float TILE_SIZE = 25.f;

class Labyrinth {
public:
  enum Tile {
    EMPTY   = ' ',
    GATE    = '-',
    WALL    = '#',
    PELLET  = '.',
    POWERUP = '@',
    PACMAN  = 'M',
    BLINKY  = 'B',
    PINKY   = 'P',
    INKY    = 'I',
    CLYDE   = 'C'
  };

  std::map<char, Tile> m_tileLut;
  std::map<Tile, std::string> m_tileLabelLut;

  int m_labyrinthRows;
  int m_labyrinthCols;
  float m_labyrinthTileSize;
  sf::RectangleShape m_wallTile;
  sf::CircleShape m_pellet;

  int getOffset(std::pair<int, int>) const;
  int getOffset(int, int) const;
  int getOffset(sf::Vector2f) const;
  std::list<int> getNeighbors(int) const;

  std::pair<int, int> getPairFromOffset(int) const;
  sf::Vector2f getSfVecFromOFfset(int) const;

  std::pair<int, int> at(sf::Vector2f) const;
  Tile at(int, int) const;
  Tile at(std::pair<int, int>) const;
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
