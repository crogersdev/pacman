#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <raylib.h>

using std::string;
using std::vector;

const int   LABYRINTH_ROWS = 24;
const int   LABYRINTH_COLS = 23;
const int   TUNNEL_ROW     = LABYRINTH_ROWS / 2;
const float TILE_SIZE      = 25.;

class Labyrinth {
public:
    Labyrinth();

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

    Tile at(int row, int col);
    void draw();


private:
    vector<string> mLabyrinth = {
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
        "#####.#.#-----#.#.#####",  // 10
        "........#-----#........",  // 11 
        "#####.#.#-----#.#.#####",  // 12 
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