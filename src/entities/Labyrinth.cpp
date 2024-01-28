#include <iostream>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

Labyrinth::Labyrinth()
  : m_tileLut({
      {' ', Labyrinth::EMPTY},
      {'#', Labyrinth::WALL},
      {'.', Labyrinth::PELLET},
      {'@', Labyrinth::POWERUP},
      {'M', Labyrinth::PACMAN},
      {'B', Labyrinth::BLINKY},
      {'P', Labyrinth::PINKY},
      {'I', Labyrinth::INKY},
      {'C', Labyrinth::CLYDE},
      {'-', Labyrinth::GATE}
    }),
    m_labyrinthRows(LABYRINTH_ROWS),
    m_labyrinthCols(LABYRINTH_COLS),
    m_labyrinthTileSize(TILE_SIZE)
{
  m_wallTile = sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
  m_wallTile.setFillColor(sf::Color::Blue);
}

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
  Tile t;
  try
  {
    t = m_tileLut.at(tile);
  }
  catch (std::out_of_range& exc)
  {
    std::cout << "oh no!  failed to access m_labyrinth[" << y << "][" << x << "]\n";
  }
  return t;
}

Labyrinth::Tile Labyrinth::at(std::pair<int, int> coords) const
{
  // TODO: do i need "this", e.g. this.at() ?
  return at(coords.first, coords.second);
}

void Labyrinth::draw(std::shared_ptr<sf::RenderWindow> pGameWindow)
{
  for (int row = 0; row < m_labyrinthRows; ++row)
  {
    for (int col = 0; col < m_labyrinthCols; ++col)
    {
      auto tile = at(col, row);
      switch(tile) {
        case Labyrinth::WALL:
          m_wallTile.setPosition(sf::Vector2f(TILE_SIZE * col, TILE_SIZE * row));
          pGameWindow->draw(m_wallTile);
        case Labyrinth::GATE:
        case Labyrinth::PELLET:
        case Labyrinth::POWERUP:
          break;
        default:
          break;
      }
    }
  }
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
