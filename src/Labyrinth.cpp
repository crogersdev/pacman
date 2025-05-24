#include <iostream> 

#include "Labyrinth.hpp"

Labyrinth::Labyrinth() {}

Labyrinth::Tile Labyrinth::at(int row, int col) const {
    if ((row < 0 || row >= LABYRINTH_ROWS) ||
        (col < 0 || col >= LABYRINTH_COLS)) {
        return Labyrinth::Tile::ERROR;
    }

    return static_cast<Labyrinth::Tile>(mLabyrinth.at(row).at(col));
}

void Labyrinth::draw() {
    int x = 0, y = 0;
    Color mTile = { 33, 33, 255, 255 };
    Color mPellet = { 255, 243, 153, 255 };
    Color mPowerup = GetColor(0xff00ffff);

    for (const auto &row : mLabyrinth) {
        for (const auto &col : row) {
            Tile t = (Tile) col;
            switch (t) {
            case Tile::WALL:
                DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, mTile);
                break;
            case Tile::PELLET:
                DrawRectangle(x+10, y+10, 5, 5, mPellet);
                break;
            case Tile::POWERUP:
                DrawCircle(x+13, y+13, 10, mPowerup);
                break;
            default:
                break;
            }
            x += TILE_SIZE;
        }
        y += TILE_SIZE;
        x = 0;
    }
}
