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
    m_tileLabelLut({
      {Labyrinth::EMPTY, "Empty"},
      {Labyrinth::WALL, "Wall"},
      {Labyrinth::PELLET, "Pellet"},
      {Labyrinth::POWERUP, "Powerup"},
      {Labyrinth::PACMAN, "Pacman"},
      {Labyrinth::BLINKY, "Blinky"},
      {Labyrinth::PINKY, "Pinky"},
      {Labyrinth::INKY, "Inky"},
      {Labyrinth::CLYDE, "Clyde"},
      {Labyrinth::GATE, "Gate"}
    }),
    m_labyrinthRows(LABYRINTH_ROWS),
    m_labyrinthCols(LABYRINTH_COLS),
    m_labyrinthTileSize(TILE_SIZE),
    m_wallTile(sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE))),
    m_pellet(sf::CircleShape(3.f))
{
  m_wallTile.setFillColor(sf::Color::Blue);
  m_pellet.setFillColor(sf::Color(255, 255, 191)); // "faint yellow, #FFFFBF"
}

Labyrinth::Tile Labyrinth::at(int x, int y) const
{
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
          break;
        case Labyrinth::PELLET:
        { // EXPLAIN: new scope is needed for declaring a variable in
          //          a case that might not be invoked at runtime, causing
          //          some weird memory errors because we'd be avoiding 
          //          an outcome of executing this program
          //          where the variables were not initialized - and
          //          in C++ you must initialize variables that are used
          //          in switch statements
          auto x = TILE_SIZE * col + TILE_SIZE / 2.f - m_pellet.getRadius();
          auto y = TILE_SIZE * row + TILE_SIZE / 2.f - m_pellet.getRadius();
          m_pellet.setPosition(sf::Vector2f(x, y));
          pGameWindow->draw(m_pellet);
        }
          break;
        case Labyrinth::POWERUP:
        case Labyrinth::GATE:
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

void Labyrinth::set(int x, int y, Tile entity)
{
  m_labyrinth[y][x] = entity;
}
