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
  // EXPLAIN
  // i never did figure out why the # of cols was 2 larger than what appeared
  // on the screen.  maybe two null terminating characters because of the doubly
  // nested std::array?  idk.  it really gets to me to have to do this wonky
  // arithmetic but what can i do
  // also we prefix ++ on position.second because we need to count the 0th
  // position as a position
  auto col = position.first;
  auto row = position.second;
  return (col + row * (m_labyrinthCols - 1));
}

int Labyrinth::getOffset(int col, int row) const {
  return getOffset(std::pair<int, int>(col, row));
}

int Labyrinth::getOffset(sf::Vector2f position) const {
  std::pair<int, int> coords = at(position);
  return getOffset(coords);
}

int Labyrinth::heuristic(sf::Vector2f start, sf::Vector2f end) const {
  return heuristic(
    std::pair<int, int>(static_cast<int>(start.x), static_cast<int>(start.y)),
    std::pair<int, int>(static_cast<int>(end.x),   static_cast<int>(end.y)));
}

int Labyrinth::heuristic(std::pair<int, int> start, std::pair<int, int> end) const {
  int heuristicCost = 0;

  while ((std::abs(start.first  - end.first))  > 0 &&
         (std::abs(start.second - end.second)) > 0) {
    heuristicCost += 14;

    if (start.first > end.first)
      start.first--;
    else
      start.first++;

    if (start.second > end.second)
      start.second--;
    else
      start.second++;
  }

  heuristicCost += 10 * std::abs(start.first  - end.first);
  heuristicCost += 10 * std::abs(start.second - end.second);

  return heuristicCost;
}

int Labyrinth::heuristic(int startOffset, int endOffset) const {
  return heuristic(getPairFromOffset(startOffset), getPairFromOffset(endOffset));
}

std::list<int> Labyrinth::getNeighbors(int offset) const {
  std::pair<int, int> coords = getPairFromOffset(offset);
  auto c = coords.first;
  auto r = coords.second;
  std::list<int> neighbors;

  for (int row = -1; row < 2; ++row) {
    for (int col = -1; col < 2; ++col) {
      if ((c + col < 0 || c + col > m_labyrinthCols) ||
          (r + row < 0 || r + row > m_labyrinthRows)) {
        continue;
      }
      if (getOffset(c+col, r+row) == offset) continue;

      auto n = at(c+col, r+row);
      if ((n != Tile::GATE) && (n != Tile::WALL)) {
        neighbors.push_back(getOffset(c+col, r+row));
      }
    }
  }
  return neighbors;
}

std::pair<int, int> Labyrinth::getPairFromOffset(int offset) const {
  int col = offset % (m_labyrinthCols - 1);
  int row = offset / (m_labyrinthCols - 1);
  return std::pair<int, int>(col, row);
}

sf::Vector2f Labyrinth::getSfVecFromOffset(int offset) const {
  int col = offset % (m_labyrinthCols - 1);
  int row = offset / (m_labyrinthCols - 1);
  return sf::Vector2f(col * TILE_SIZE, row * TILE_SIZE);
}

std::pair<int, int> Labyrinth::at(sf::Vector2f position) const {
  int row = std::floor(static_cast<int>(position.y) / TILE_SIZE);
  int col = std::floor(static_cast<int>(position.x) / TILE_SIZE);

  return std::pair<int, int>(col, row);
}

Labyrinth::Tile Labyrinth::at(int col, int row) const {
  if (col <= 0)
    col = 0;
  if (row <= 0)
    row = 0;
  if (col >= LABYRINTH_COLS - 1)
    col = LABYRINTH_COLS - 2;
  if (row >= LABYRINTH_ROWS)
    row = LABYRINTH_ROWS - 1;

  char tile = static_cast<char>(m_labyrinth[row][col]);
  Tile t;
  try {
    t = m_tileLut.at(tile);
  }
  catch (std::out_of_range& exc) {
    std::cout << "oh no!  failed to access m_labyrinth[" << row << "][" << col << "]\n";
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
