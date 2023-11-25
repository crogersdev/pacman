#pragma once

#include <SFML/Graphics.hpp>

#include <array>

const int LABYRINTH_ROWS = 18;
const int LABYRINTH_COLS = 12 + 1;

class Labyrinth
{
public:
  int m_labyrinthRows;
  int m_labyrinthCols;

  std::array<std::array<char, LABYRINTH_COLS>, LABYRINTH_ROWS> m_labyrinth =
  {
    "############",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#     #    #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "#          #",
    "############",
  };

  std::array<int, LABYRINTH_COLS * LABYRINTH_ROWS> m_levelOne;

  enum Tile {
    WALL  = '#',
    PELLET  = '.',
    POWERUP = 'O'
  };

  Tile at(int, int);
  std::unordered_map<char, Tile> m_tileLut;

  Labyrinth();
};