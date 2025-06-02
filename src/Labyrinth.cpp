#include <iostream> 

#include "Labyrinth.hpp"

Labyrinth::Labyrinth() {
    mPelletSprite = LoadTexture("res/pellet.png");
    mPowerupSprite = LoadTexture("res/powerup.png");

    float centerTileY, centerTileX;
    int c = 0, r = 0;

    for (const auto& row : mLabyrinth) {
        centerTileY = r + TILE_SIZE / 2;
        for (const auto& col : row) {
            centerTileX = c + TILE_SIZE / 2;
            Tile tile = static_cast<Tile>(col);

            switch (tile) {
            case Tile::PELLET:
                // std::cout << "adding a pellet at row: " << row << ", col: " << col << "\n";
                mPellets.insert(std::make_pair(std::make_pair(c, r), LabyrinthObject(
                        AnimatedSprite(mPelletSprite, TILE_SIZE, TILE_SIZE, 2, 10),
                        {centerTileX, centerTileY},
                        3)));
                break;
            case Tile::POWERUP:
                mPowerups.insert(std::make_pair(std::make_pair(c, r), LabyrinthObject(
                        AnimatedSprite(mPowerupSprite, TILE_SIZE, TILE_SIZE, 2, 12),
                        {centerTileX, centerTileY},
                        8)));
                break;
            default:
                break;
            }

            c += TILE_SIZE;
        }
        r += TILE_SIZE;
        c = 0;
    } 
}

Labyrinth::~Labyrinth() {
    UnloadTexture(mPelletSprite);
    UnloadTexture(mPowerupSprite);
}

Labyrinth::Tile Labyrinth::at(int row, int col) const {
    if ((row < 0 || row >= LABYRINTH_ROWS) ||
        (col < 0 || col >= LABYRINTH_COLS)) {
        return Labyrinth::Tile::ERROR;
    }

    return static_cast<Labyrinth::Tile>(mLabyrinth.at(row).at(col));
}

void Labyrinth::draw() {
    int x = 0, y = 0;
    Color wallColor = { 33, 33, 255, 255 };

    for (const auto &pellet: mPellets) {
        pellet.second.draw();
    }

    for (const auto &powerup: mPowerups) {
        powerup.second.draw();
    }

    for (const auto &row : mLabyrinth) {
        for (const auto &col : row) {
            Tile t = (Tile) col;
            switch (t) {
            case Tile::WALL:
                DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, wallColor);
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
