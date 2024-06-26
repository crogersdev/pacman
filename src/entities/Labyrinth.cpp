#include <iostream>

#include "./Labyrinth.hpp"
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
    m_pellet(sf::CircleShape(3.f)) {
  m_wallTile.setFillColor(sf::Color::Blue);
  m_pellet.setFillColor(sf::Color(255, 255, 191));  // "faint yellow, #FFFFBF"
}

int Labyrinth::getOffset(std::pair<int, int> position) const {
  return (position.first * m_labyrinthRows) + position.second;
}

int Labyrinth::getOffset(int row, int col) const {
  return getOffset(std::pair<int, int>(row, col));
}

int Labyrinth::getOffset(sf::Vector2f position) const {
  std::pair<int, int> coords = at(position);
  return getOffset(coords);
}

std::list<int> Labyrinth::getNeighbors(int offset) const {
  std::pair<int, int> coords = getPairFromOffset(offset);
  auto c = coords.first;
  auto r = coords.second;
  std::list<int> neighbors;

  for (int col = -1; col < 2; ++col) {
    for (int row = -1; row < 2; ++row) {
      if ((c + col < 0 || c + col > m_labyrinthCols) ||
          (r + row < 0 || r + row > m_labyrinthRows)) {
        continue;
      }
      auto n = m_labyrinth[c+col][r+row];
      if ((n != Tile::GATE) || (n != Tile::WALL))
        neighbors.push_back(getOffset(r+row, c+col));
    }
  }
  return neighbors;
}

std::pair<int, int> Labyrinth::getPairFromOffset(int offset) const {
  int col = offset % m_labyrinthCols;
  int row = std::floor(offset / m_labyrinthRows);
  return std::pair<int, int>(col, row);
}

sf::Vector2f Labyrinth::getSfVecFromOFfset(int offset) const {
  int col = offset % m_labyrinthCols;
  int row = std::floor(offset / m_labyrinthRows);
  return sf::Vector2f(col, row);
}

std::pair<int, int> Labyrinth::at(sf::Vector2f position) const {
  int row = std::floor(position.y / static_cast<float>(m_labyrinthRows));
  int col = std::floor(position.x / static_cast<float>(m_labyrinthCols));

  return std::pair<int, int>(row, col);
}

Labyrinth::Tile Labyrinth::at(int x, int y) const {
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
  try {
    t = m_tileLut.at(tile);
  }
  catch (std::out_of_range& exc) {
    std::cout << "oh no!  failed to access m_labyrinth[" << y << "][" << x << "]\n";
  }
  return t;
}

Labyrinth::Tile Labyrinth::at(std::pair<int, int> coords) const {
  // TODO(crogers): do i need "this", e.g. this.at() ?
  return at(coords.first, coords.second);
}

void Labyrinth::draw(std::shared_ptr<sf::RenderWindow> pGameWindow) {
  for (int row = 0; row < m_labyrinthRows; ++row) {
    for (int col = 0; col < m_labyrinthCols; ++col) {
      auto tile = at(col, row);
      switch (tile) {
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

void Labyrinth::set(sf::Vector2f pos, Tile entity) {
  auto coords = tileCoordsAtPosition(pos);

  m_labyrinth[coords.first][coords.second] = entity;
}

void Labyrinth::set(int row, int col, Tile entity) {
  // EXPLAIN: when pacman goes through his tunnel we have a possible
  //          scenario where the x coord is 0 and then -1, or 29 and we
  //          don't ever want to set the labyrinth tile at that
  //          position to anything at all
  if (col == 29 || col <= -1)
    return;

  m_labyrinth[row][col] = entity;
}
