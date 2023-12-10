#include <iostream>

#include "Labyrinth.hpp"
#include "../helpers/TileCoordConversion.hpp"

Labyrinth::Labyrinth()
  : m_labyrinthRows(LABYRINTH_ROWS),
    m_labyrinthCols(LABYRINTH_COLS),
    m_tileLut({{'#', WALL}, {'.', PELLET}, {'O', POWERUP}})
{}

Labyrinth::Tile Labyrinth::at(int x, int y) {
  // NOTE: x is a column
  //   y is a row
  char tile = static_cast<char>(m_labyrinth[y][x]);
  return m_tileLut[tile];
}

void Labyrinth::set(sf::Vector2f pos, Labyrinth::Tile entity) {
  auto coords = tileCoordsAtPosition(pos);

  m_labyrinth[coords.first][coords.second] = entity;
}

void Labyrinth::print() {
  for (auto row: m_labyrinth) {
    for (auto col: row) {
      std::cout << "\n";
    }
  }
}