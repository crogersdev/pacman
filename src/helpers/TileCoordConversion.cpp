#include "TileCoordConversion.hpp"

std::pair<int, int> positionToCoords(sf::Vector2f pos) {
    return std::make_pair<int, int>(
        floor(pos.x / TILE_SIZE),
        floor(pos.y / TILE_SIZE)
    );
}

sf::Vector2f coordsToPosition(int col, int row) {
    return sf::Vector2f((col * TILE_SIZE), (row * TILE_SIZE));
}

sf::Vector2f tileCoordsAtPosition(sf::Vector2f pos) {
    auto p = positionToCoords(pos);
    return sf::Vector2f(
        p.first * TILE_SIZE,
        p.second * TILE_SIZE
    );
}

void wrapCoordinate(float &coord, float min, float max) {
    if (coord < min)
        coord = max;
    else if (coord > max)
        coord = min;
}