#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <memory>

const int   LABYRINTH_ROWS = 31;
const int   LABYRINTH_COLS = 29;
const float TILE_SIZE = 25.f;

class Labyrinth
{
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

  std::array<int, LABYRINTH_COLS * LABYRINTH_ROWS> m_levelOne;
  std::unordered_map<char, Tile> m_tileLut;

  int m_labyrinthRows;
  int m_labyrinthCols;

  Tile at(int, int) const;
  Tile at(std::pair<int, int>) const;
  void set(sf::Vector2f, Tile);
  void set(int, int, Tile);
  
  void print();
  void draw();

  Labyrinth();

private:

  std::array<std::array<char, LABYRINTH_COLS>, LABYRINTH_ROWS> m_labyrinth = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#@#  #.#   #.##.#   #.#  #@#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.##### ## #####.######",
    "     #.##### ## #####.#     ",
    "     #.##          ##.#     ",
    "     #.## ###--### ##.#     ",
    "######.## #      # ##.######",
    "      .   #      #   .      ",
    "######.## #      # ##.######",
    "     #.## ######## ##.#     ",
    "     #.##          ##.#     ",
    "     #.## ######## ##.#     ",
    "######.## ######## ##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#",
    "#@..##.......  .......##..@#",
    "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
  };
};