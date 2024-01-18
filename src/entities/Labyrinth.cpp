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
    m_labyrinthCols(LABYRINTH_COLS),
    m_labyrinthTileSize(TILE_SIZE)
{}

Labyrinth::Tile Labyrinth::at(int x, int y) const
{
  // NOTE: x is a column
  //       y is a row
  if (x <= 0)
    x = 0;
  if (y <= 0)
    y = 0;
  if (x >= LABYRINTH_COLS - 1)
    x = LABYRINTH_COLS - 2;
  if (y >= LABYRINTH_ROWS)
    y = LABYRINTH_ROWS - 1;

  char tile = static_cast<char>(m_labyrinth[y][x]);
  Tile foo;
  try
  {
    foo = m_tileLut.at(tile);
  }
  catch (std::out_of_range& exc)
  {
    std::cout << "oh no!  failed to access m_labyrinth[" << y << "][" << x << "]\n";
  }
  return foo;
}

Labyrinth::Tile Labyrinth::at(std::pair<int, int> coords) const
{
  // TODO: do i need "this", e.g. this.at() ?
  return at(coords.first, coords.second);
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
