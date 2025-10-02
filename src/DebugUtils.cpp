#include "DebugUtils.hpp"

std::ostream &operator<<(std::ostream &os, Ghost::State s) {
    switch(s) {
    case Ghost::State::CHASE:
        os << "CHASE";
        break;
    case Ghost::State::FRIGHTENED:
        os << "FRIGHTENED";
        break;
    case Ghost::State::SCATTER:
        os << "SCATTER";
        break;
    case Ghost::State::MEANDER:
        os << "MEANDER";
        break;
    case Ghost::State::IN_PRISON:
        os << "IN_PRISON";
        break;
    case Ghost::State::LEAVING_PRISON:
        os << "LEAVING_PRISON";
        break;
    case Ghost::State::GOING_TO_PRISON:
        os << "GOING_TO_PRISON";
        break;
    default:
        os << "duh";
    }
    return os;
}

// NOTE: use a switch statement so you can make your enums any number value
std::ostream &operator<<(std::ostream &os, Labyrinth::Labyrinth::Tile t) {
    switch (t) {
    case Labyrinth::Tile::EMPTY:
        os << "EMPTY";
        break;
    case Labyrinth::Tile::GATE:
        os << "GATE";
        break;
    case Labyrinth::Tile::WALL:
        os << "WALL";
        break;
    case Labyrinth::Tile::PELLET:
        os << "PELLET";
        break;
    case Labyrinth::Tile::POWERUP:
        os << "POWERUP";
        break;
    case Labyrinth::Tile::PACMAN:
        os << "PACMAN";
        break;
    case Labyrinth::Tile::BLINKY:
        os << "BLINKY";
        break;
    case Labyrinth::Tile::PINKY:
        os << "PINKY";
        break;
    case Labyrinth::Tile::INKY:
        os << "INKY";
        break;
    case Labyrinth::Tile::CLYDE:
        os << "CLYDE";
        break;
    case Labyrinth::Tile::PATH:
        os << "PATH";
        break;
    case Labyrinth::Tile::ERROR:
        os << "ERROR";
        break;
    default:
        os << "DUH";
    }
    return os;
}