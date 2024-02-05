#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <memory>

const int   LABYRINTH_ROWS = 31;
const int   LABYRINTH_COLS = 29;
const float TILE_SIZE = 25.f;

class Labyrinth
{
public:
  enum Tile {
    EMPTY   = ' ',
    GATE    = '-',
    WALL    = '#',
    PELLET  = '.',
    POWERUP = '@',
    PACMAN  = 'M',
    BLINKY  = 'B',
    PINKY   = 'P',
    INKY    = 'I',
    CLYDE   = 'C'
  };

  std::map<char, Tile> m_tileLut;
  std::map<Tile, std::string> m_tileLabelLut;

  int m_labyrinthRows;
  int m_labyrinthCols;
  float m_labyrinthTileSize;
  sf::RectangleShape m_wallTile;
  sf::CircleShape m_pellet;

  Tile at(int, int) const;
  Tile at(std::pair<int, int>) const;
  void draw(std::shared_ptr<sf::RenderWindow>);
  void set(sf::Vector2f, Tile);
  void set(int, int, Tile);

  Labyrinth();

private:

  std::array<std::array<char, LABYRINTH_COLS>, LABYRINTH_ROWS> m_labyrinth = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####@#",
    "#@#  #.#   #.##.#   #.#  #@#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.#####.##.#####.######",
    "     #.#####.##.#####.#     ",
    "     #.##..........##.#     ",
    "     #.##.###--###.##.#     ",
    "######.##.#------#.##.######",
    "..........#------#..........",
    "######.##.#------#.##.######",
    "     #.##.########.##.#     ",
    "     #.##..........##.#     ",
    "     #.##.########.##.#     ",
    "######.##.########.##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#",
    "#@..##................##..@#",
    "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"    // this is row 30
  };
};
