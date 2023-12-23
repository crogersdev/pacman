#include "TileCoordConversion.hpp"
#include "../game-manager/GameManager.hpp"

void wrapCoordinate(float &coord, float min, float max)
{
    if (coord < min)
        coord = max;
    else if (coord > max)
        coord = min;
}

std::pair<int, int> tileCoordsAtPosition(sf::Vector2f pos)
{
    return std::make_pair<int, int>(
        floor(pos.x / TILE_SIZE),
        floor(pos.y / TILE_SIZE)
    );
}