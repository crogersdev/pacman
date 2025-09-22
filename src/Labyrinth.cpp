#include <iostream> 

#include "helpers/Movement.hpp"
#include "Labyrinth.hpp"

Labyrinth::Labyrinth()
    :
      mPelletSpritePath("res/pellet.png"),
      mPowerupSpritePath("res/powerup.png"),
      mLabyrinthTileMap{ {}, LoadTexture("res/labyrinth.png"), 26, 26 } {

    float centerTileY, centerTileX;
    int c = 0, r = 0;

    createTileSpriteOffsets();

    for (const auto& row : mLabyrinth) {
        centerTileY = r + TILE_SIZE / 2.f;
        for (const auto& col : row) {
            centerTileX = c + TILE_SIZE / 2.f;
            Tile tile = static_cast<Tile>(col);

            switch (tile) {
            case Tile::PELLET:
                mPellets.insert(std::make_pair(
                    std::make_pair(c / TILE_SIZE, r / TILE_SIZE), LabyrinthObject(
                        AnimatedSprite(mPelletSpritePath, TILE_SIZE, TILE_SIZE, 2, 10),
                        { centerTileX, centerTileY },
                        3)
                    ));
                break;
            case Tile::POWERUP:
                mPowerups.insert(std::make_pair(
                    std::make_pair(c / TILE_SIZE, r / TILE_SIZE), LabyrinthObject(
                        AnimatedSprite(mPowerupSpritePath, TILE_SIZE, TILE_SIZE, 2, 12),
                        { centerTileX, centerTileY },
                        8)
                    ));
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

void Labyrinth::createTileSpriteOffsets() {
    int spriteOffset = 0;

    for (int row = 0; row < LABYRINTH_ROWS; row++) {
        for (int col = 0; col < LABYRINTH_COLS; col++) {
            // check up
            if (row - 1 >= 0 && mLabyrinth.at(row - 1).at(col) == '#') { spriteOffset += 1; }
            // check down
            if (row + 1 < LABYRINTH_ROWS && mLabyrinth.at(row + 1).at(col) == '#') { spriteOffset += 2; }
            // check left
            if (col - 1 >= 0 && mLabyrinth.at(row).at(col - 1) == '#') { spriteOffset += 4; }
            // check right
            if (col + 1 < LABYRINTH_COLS && mLabyrinth.at(row).at(col + 1) == '#') { spriteOffset += 8; }

            mLabyrinthTileMap.tiles[row][col] = spriteOffset;         
            spriteOffset = 0;
        }
    }
}

void Labyrinth::draw() {

    for (const auto &pellet:  mPellets)  { pellet.second.draw(); }
    for (const auto &powerup: mPowerups) { powerup.second.draw(); }

    int labyrinthSpritesheetCols = 4;
    int frame;
    int x = 0, y = 0;

    Rectangle src;
    Rectangle dst;

    for (const auto &row : mLabyrinth) {
        for (const auto &col : row) {
            Tile t = static_cast<Tile>(col);
            switch (t) {
            case Tile::WALL:
                frame = mLabyrinthTileMap.tiles[y][x];
                src = {
                    (frame % labyrinthSpritesheetCols) * TILE_SIZE,
                    (frame / labyrinthSpritesheetCols) * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };
                dst = { static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE), TILE_SIZE, TILE_SIZE };
                DrawTexturePro(mLabyrinthTileMap.spritesheet, src, dst, Vector2{ 0.f, 0.f }, 0.f, WHITE);
                break;
            default:
                break;
            }
            x++;
        }
        y++;
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
    default:
        break;
    }

    return isLegal;
}