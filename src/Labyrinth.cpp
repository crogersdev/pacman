#include <iostream> 

#include "helpers/Movement.hpp"
#include "Labyrinth.hpp"

Labyrinth::Labyrinth() {
    mPelletSpritePath = "res/pellet.png";
    mPowerupSpritePath = "res/powerup.png";

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
                        AnimatedSprite(mPelletSpritePath, TILE_SIZE, TILE_SIZE, 2, 10),
                        {centerTileX, centerTileY},
                        3)));
                break;
            case Tile::POWERUP:
                mPowerups.insert(std::make_pair(std::make_pair(c, r), LabyrinthObject(
                        AnimatedSprite(mPowerupSpritePath, TILE_SIZE, TILE_SIZE, 2, 12),
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

Labyrinth::~Labyrinth() { }

Labyrinth::Tile Labyrinth::at(int row, int col) const {
    if ((row < 0 || row >= LABYRINTH_ROWS) ||
        (col < 0 || col >= LABYRINTH_COLS)) {
        return Labyrinth::Tile::ERROR;
    }

    return static_cast<Labyrinth::Tile>(mLabyrinth.at(row).at(col));
}

Labyrinth::Tile Labyrinth::at(Vector2 pos) const {
    int row = static_cast<int>(pos.y / TILE_SIZE);
    int col = static_cast<int>(pos.x / TILE_SIZE);

    return at(row, col);
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
            Tile t = static_cast<Tile>(col);
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

bool Labyrinth::isLegalMove(Vector2 pos) const {
    bool isLegal = true;
    auto halfTile = TILE_SIZE / 2;
    DrawRectangle(pos.x - halfTile, pos.y - halfTile, TILE_SIZE, TILE_SIZE, PURPLE);

    Tile t = at(pos);
    switch (t) {
    case Tile::WALL:
    case Tile::GATE:
        isLegal = false;
        std::cout << "ILLEGAL!!!\n";
        break;
    }

    return isLegal;
}