#include <iostream> 

#include "Labyrinth.hpp"

Labyrinth::Labyrinth() {
    mPelletSprite = LoadTexture("res/pellet.png");
    mPowerupSprite = LoadTexture("res/powerup.png");
    // why not range based?
    // because we need pixel offset to know where the pellets and powerups will be
    for (int row = 0; row < LABYRINTH_ROWS; ++row) {
        for (int col = 0; col < LABYRINTH_COLS; ++col) {
            Tile tile = this->at(row, col);
            float centerTileX = col * TILE_SIZE + TILE_SIZE / 2;
            float centerTileY = row * TILE_SIZE + TILE_SIZE / 2;

            if (tile == Tile::PELLET) {
                // std::cout << "adding a pellet at row: " << row << ", col: " << col << "\n";
                mPellets.insert(std::make_pair(std::make_pair(col, row), LabyrinthObject(
                        AnimatedSprite(mPelletSprite, 26, 26, 2, 10),
                        {centerTileX, centerTileY},
                        3)));
            } else if (tile == Tile::POWERUP) {
                mPowerups.push_back(
                    LabyrinthObject(
                        AnimatedSprite(mPowerupSprite, 26, 26, 2, 12),
                        {centerTileX, centerTileY},
                        8));
            }
        }
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
        powerup.draw();
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
