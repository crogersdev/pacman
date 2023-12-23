#include <iostream>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

Labyrinth::Labyrinth()
  : m_tileLut({
      {' ', EMPTY},
      {'#', WALL},
      {'.', PELLET},
      {'@', POWERUP},
      {'M', PACMAN},
      {'B', BLINKY},
      {'P', PINKY},
      {'I', INKY},
      {'C', CLYDE},
      {'-', GATE}
    }),
    m_labyrinthRows(LABYRINTH_ROWS),
    m_labyrinthCols(LABYRINTH_COLS-1) // pesky \0!!!
{}

Labyrinth::Tile Labyrinth::at(int x, int y) const
{
  // NOTE: x is a column
  //       y is a row
  char tile = static_cast<char>(m_labyrinth[y][x]);
  return m_tileLut.at(tile);
}

void Labyrinth::set(sf::Vector2f pos, Tile entity)
{
  auto coords = tileCoordsAtPosition(pos);

  m_labyrinth[coords.first][coords.second] = entity;
}

void Labyrinth::set(int row, int col, Tile entity)
{
  m_labyrinth[col][row] = entity;
}

void Labyrinth::print()
{
  for (auto row: m_labyrinth) {
    for (auto col: row) {
      std::cout << "\n";
    }
  }
}