#include <iostream>

#include "./Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

Labyrinth::Labyrinth()
  : m_tileLut({
      {' ', Labyrinth::Tile::EMPTY},
      {'#', Labyrinth::Tile::WALL},
      {'.', Labyrinth::Tile::PELLET},
      {'@', Labyrinth::Tile::POWERUP},
      {'M', Labyrinth::Tile::PACMAN},
      {'B', Labyrinth::Tile::BLINKY},
      {'P', Labyrinth::Tile::PINKY},
      {'I', Labyrinth::Tile::INKY},
      {'C', Labyrinth::Tile::CLYDE},
      {'-', Labyrinth::Tile::GATE},
      {'X', Labyrinth::Tile::PATH},
      {'E', Labyrinth::Tile::ERROR}
    }),
    m_tileLabelLut({
      {Labyrinth::Tile::EMPTY, "Empty"},
      {Labyrinth::Tile::WALL, "Wall"},
      {Labyrinth::Tile::PELLET, "Pellet"},
      {Labyrinth::Tile::POWERUP, "Powerup"},
      {Labyrinth::Tile::PACMAN, "Pacman"},
      {Labyrinth::Tile::BLINKY, "Blinky"},
      {Labyrinth::Tile::PINKY, "Pinky"},
      {Labyrinth::Tile::INKY, "Inky"},
      {Labyrinth::Tile::CLYDE, "Clyde"},
      {Labyrinth::Tile::GATE, "Gate"},
      {Labyrinth::Tile::PATH, "Path"},
      {Labyrinth::Tile::ERROR, "Error"}
    }),
    m_wallTile(sf::RectangleShape(sf::Vector2f(TILE_SIZE, TILE_SIZE))),
    m_pellet(sf::CircleShape(3.f)) {
  m_wallTile.setFillColor(sf::Color::Blue);
  m_pellet.setFillColor(sf::Color(255, 255, 191));  // "faint yellow, #FFFFBF"
}

int Labyrinth::getOffset(std::pair<int, int> position) const {
  return getOffset(position.first, position.second);
}

int Labyrinth::getOffset(int col, int row) const {
  return col + row * (LABYRINTH_COLS - 1);
}

int Labyrinth::getOffset(sf::Vector2f position) const {
  std::pair<int, int> coords = getPairFromSfVec(position);
  return getOffset(coords);
}

int Labyrinth::heuristic(int startOffset, int endOffset) const {
  return heuristic(getPairFromOffset(startOffset), getPairFromOffset(endOffset));
}

int Labyrinth::heuristic(sf::Vector2f start, sf::Vector2f end) const {
  return heuristic(getPairFromSfVec(start), getPairFromSfVec(end));
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

int Labyrinth::heuristicThroughTunnel(int startOffset, int endOffset) const {
  return heuristicThroughTunnel(
    getPairFromOffset(startOffset),
    getPairFromOffset(endOffset));
}

int Labyrinth::heuristicThroughTunnel(sf::Vector2f start, sf::Vector2f end) const {
  return heuristicThroughTunnel(
    getPairFromSfVec(start),
    getPairFromSfVec(end));
}

int Labyrinth::heuristicThroughTunnel(std::pair<int, int> start, std::pair<int, int> end) const {
  auto leftTunnelEntrance  = std::pair<int, int>(0, TUNNEL_ROW);
  auto rightTunnelEntrance = std::pair<int, int>(LABYRINTH_COLS - 2, TUNNEL_ROW);

  auto leftTunnelHCost = heuristic(start, leftTunnelEntrance);
  leftTunnelHCost += 10 + heuristic(rightTunnelEntrance, end);
  auto rightTunnelHCost = heuristic(start, rightTunnelEntrance);;
  rightTunnelHCost += 10 + heuristic(leftTunnelEntrance, end);

  return std::min(rightTunnelHCost, leftTunnelHCost);
}

int Labyrinth::movementCost(int startOffset, int endOffset) const {
  return movementCost(
    getPairFromOffset(startOffset),
    getPairFromOffset(endOffset));
}

int Labyrinth::movementCost(sf::Vector2f startVec, sf::Vector2f endVec) const {
  return movementCost(
    getPairFromSfVec(startVec),
    getPairFromSfVec(endVec));
}

int Labyrinth::movementCost(std::pair<int, int> start, std::pair<int, int> end) const {
  return 10;
}

std::list<int> Labyrinth::getNeighbors(int offset) const {
  // NOTE: coords.first  == col
  //       coords.second == row
  std::pair<int, int> coords = getPairFromOffset(offset);

  std::list<int> neighbors;
  std::vector<int> offsets = { offset+LABYRINTH_COLS-1, offset-(LABYRINTH_COLS-1) };

  if (offset == 392) {
    offsets.push_back(393);
    offsets.push_back(419);
  } else if (offset == 419) {
    offsets.push_back(392);
    offsets.push_back(418);
  } else {
    offsets.push_back(offset-1);
    offsets.push_back(offset+1);
  }

  for (const auto& o : offsets) {
    auto potentialNeighborPair = getPairFromOffset(o);
    auto potentialNeighbor = at(potentialNeighborPair);

    if (potentialNeighbor != Labyrinth::Tile::ERROR &&
        potentialNeighbor != Labyrinth::Tile::WALL)
        neighbors.push_back(getOffset(potentialNeighborPair));
    }

  return neighbors;
}

std::pair<int, int> Labyrinth::getPairFromOffset(int offset) const {
  int col = offset % (LABYRINTH_COLS - 1);
  int row = offset / (LABYRINTH_COLS - 1);
  return std::pair<int, int>(col, row);
}

sf::Vector2f Labyrinth::getSfVecFromOffset(int offset) const {
  int col = offset % (LABYRINTH_COLS - 1);
  int row = offset / (LABYRINTH_COLS - 1);
  return sf::Vector2f(col * TILE_SIZE, row * TILE_SIZE);
}

std::pair<int, int> Labyrinth::getPairFromSfVec(sf::Vector2f position) const {
  int row = std::floor(static_cast<int>(position.y) / TILE_SIZE);
  int col = std::floor(static_cast<int>(position.x) / TILE_SIZE);

  return std::pair<int, int>(col, row);
}

Labyrinth::Tile Labyrinth::at(int col, int row) const {
  if (col <= 0 && row == 14)
    col = LABYRINTH_COLS - 1;
  if (col >= LABYRINTH_COLS - 3 && row == 14)
    col = 0;

  if (col <= 0)
    col = 0;
  if (row <= 0)
    row = 0;
  if (col >= LABYRINTH_COLS - 1)
    col = LABYRINTH_COLS - 2;
  if (row >= LABYRINTH_ROWS)
    row = LABYRINTH_ROWS - 1;

  char tile = static_cast<char>(m_labyrinth[row][col]);
  Tile t = Tile::ERROR;
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
  for (int row = 0; row < LABYRINTH_ROWS; ++row) {
    for (int col = 0; col < LABYRINTH_COLS - 1; ++col) {
      auto tile = at(col, row);
      switch (tile) {
        case Labyrinth::Tile::WALL:
          m_wallTile.setPosition(sf::Vector2f(TILE_SIZE * col, TILE_SIZE * row));
          pGameWindow->draw(m_wallTile);
          break;
        case Labyrinth::Tile::PELLET:
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
        case Labyrinth::Tile::PATH:
        {
          sf::RectangleShape rectangle(sf::Vector2f(25.f, 25.f));
          rectangle.setFillColor(sf::Color(139, 0, 0));  // Dark red
          auto x = TILE_SIZE * col;
          auto y = TILE_SIZE * row;
          rectangle.setPosition(sf::Vector2f(x, y));
          pGameWindow->draw(rectangle);
        }
        case Labyrinth::Tile::POWERUP:
        case Labyrinth::Tile::GATE:
          break;
        default:
          break;
      }
    }
  }
}

void Labyrinth::set(sf::Vector2f pos, Tile entity) {
  auto coords = tileCoordsAtPosition(pos);

  m_labyrinth[coords.second][coords.first] = static_cast<char>(entity);
}

void Labyrinth::set(int col, int row, Tile entity) {
  // EXPLAIN: when pacman goes through his tunnel we have a possible
  //          scenario where the x coord is 0 and then -1, or 29 and we
  //          don't ever want to set the labyrinth tile at that
  //          position to anything at all
  if (col == 29 || col <= -1)
    return;

  m_labyrinth[row][col] = static_cast<char>(entity);
}
