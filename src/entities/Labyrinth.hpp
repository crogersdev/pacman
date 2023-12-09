#pragma once

#include <SFML/Graphics.hpp>

#include <array>

const int LABYRINTH_ROWS = 18;
const int LABYRINTH_COLS = 12 + 1;

class Labyrinth
{
public:
  enum Tile {
    WALL  = '#',
    PELLET  = '.',
    POWERUP = 'O',
    PACMAN = 'M',
    BLINKY = 'B',
    PINKY = 'P',
    INKY = 'I',
    CLYDE = 'C'
  };

  int m_labyrinthRows;
  int m_labyrinthCols;

  std::array<std::array<char, LABYRINTH_COLS>, LABYRINTH_ROWS> m_labyrinth =
  {
    "############",
    "#          #",
    "# ######## #",
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
  std::unordered_map<char, Tile> m_tileLut;

  Tile at(int, int);
  void set(sf::Vector2f, Tile);
  
  void print();

  Labyrinth();
};