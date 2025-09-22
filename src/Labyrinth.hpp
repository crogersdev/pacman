#pragma once

#include <cstdint>

#include "helpers/AnimatedSprite.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <raylib.h>

const int   LABYRINTH_ROWS = 24;
const int   LABYRINTH_COLS = 23;
const int   TUNNEL_ROW     = 11;
const float TILE_SIZE      = 26.f;

class Labyrinth {
public:
    Labyrinth();
    ~Labyrinth();

    struct LabyrinthObject { 
        AnimatedSprite sprite;
        Vector2 position;
        int radius;

        LabyrinthObject(AnimatedSprite s, Vector2 p, int r) 
        : sprite(s),
          position(p),
          radius(r) {}
        
        void draw() const {
            sprite.draw(position);
        }
    };

    enum class Tile {
        EMPTY   = ' ',
        GATE    = '-',
        WALL    = '#',
        PELLET  = '.',
        POWERUP = '@',
        PACMAN  = 'M',
        BLINKY  = 'B',
        PINKY   = 'P',
        INKY    = 'I',
        CLYDE   = 'C',
        PATH    = 'X',
        ERROR   = 'E'
    };

    // NOTE: overloaded ostream opers gotta be a 'friend' if it's in a class.  idk why.
    // NOTE: use a switch statement so you can make your enums any number value
    friend std::ostream &operator<<(std::ostream &os, Tile t) {
        switch (t) {
        case Tile::EMPTY:
            os << "EMPTY";
            break;
        case Tile::GATE:
            os << "GATE";
            break;
        case Tile::WALL:
            os << "WALL";
            break;
        case Tile::PELLET:
            os << "PELLET";
            break;
        case Tile::POWERUP:
            os << "POWERUP";
            break;
        case Tile::PACMAN:
            os << "PACMAN";
            break;
        case Tile::BLINKY:
            os << "BLINKY";
            break;
        case Tile::PINKY:
            os << "PINKY";
            break;
        case Tile::INKY:
            os << "INKY";
            break;
        case Tile::CLYDE:
            os << "CLYDE";
            break;
        case Tile::PATH:
            os << "PATH";
            break;
        case Tile::ERROR:
            os << "ERROR";
            break;
        default:
            os << "DUH";
        }
        return os;
    }

    struct LabyrinthTileMap {
        uint8_t   tiles[LABYRINTH_ROWS][LABYRINTH_COLS];
        Texture2D spritesheet;
        int       tileWidth;
        int       tileHeight; 
    };
    
    std::string mPelletSpritePath;
    std::string mPowerupSpritePath;
    // todo: add fruit

    std::map<std::pair<int, int>, LabyrinthObject> mPellets;
    std::map<std::pair<int, int>, LabyrinthObject> mPowerups;
    // todo: add fruit

    Tile at(int row, int col) const;
    Tile at(Vector2) const;
    void createTileSpriteOffsets();
    void draw();
    int  getHeight() const { return mLabyrinth.size(); }
    int  getRemainingPellets() const { return mPellets.size(); }
    int  getRemainingPowerups() const { return mPowerups.size(); }
    int  getWidth() const { return mLabyrinth.at(0).size(); }
    bool isLegalMove(Vector2) const;

    LabyrinthTileMap mLabyrinthTileMap;

    std::vector<std::string> mLabyrinth = {
        "#######################",  //  0
        "#..........#..........#",  //  1
        "#@###.####.#.####.###@#",  //  2      
        "#.###.####.#.####.###.#",  //  3
        "#.....................#",  //  4
        "#.###.#.#######.#.###.#",  //  5
        "#.....#....#....#.....#",  //  6
        "#####.####.#.####.#####",  //  7
        "    #.#.........#.#    ",  //  8
        "    #.#.###-###.#.#    ",  //  9
        "#####.#.#     #.#.#####",  // 10
        "........#     #........",  // 11 
        "#####.#.#     #.#.#####",  // 12 
        "    #.#.#######.#.#    ",  // 13 
        "    #.#.........#.#    ",  // 14 
        "#####.#.#######.#.#####",  // 15 
        "#..........#..........#",  // 16 
        "#.###.####.#.####.###.#",  // 17 
        "#@..#.............#..@#",  // 18 
        "###.#.#.#######.#.#.###",  // 19 
        "#.....#....#....#.....#",  // 20 
        "#.########.#.########.#",  // 21 
        "#.....................#",  // 22 
        "#######################"   // 23 
    };
};

/*

0  -- no neighbors, empty
1  -- neighbor above only,        U
2  -- neighbor below only,        upside down U
3  -- neighbor above and below,   vertical pipe 

4  -- neighbor left only,         side U opening right
5  -- neighbor left  (4)          
      neighbor above (1)          bottom right hand curve
6  -- neighbor left  (4)
      neighbor below (2)          top right hand curve
7  -- neighbor left  (4)
      neighbor below (2)
      neighbor above (1)          right hand side 

8  -- neighbor right only,        side U opening left
9  -- neighbor right (8)
      neighbor above (1)          bottom left hand curve
10 -- neighbor below (8)
      neighbor below (2)          top left hand curve
11 -- neighbor right (8)
      neighbor below (2)
      neighbor above (1)          sideways T pointing right
12 -- neighbor right (8)
   -- neighbor left  (4)          horizontal band
13 -- neighbor right (8)
      neighbor left  (4)
      neighbor above (1)          T pointing up
14 -- neighbor below (2)
      neighbor left  (4)
      neighbor right (8)          T pointing down
15 -- all neighbors               plus sign

*/