#pragma once

#include <cmath>
#include <map>

#include <raylib.h>

enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// order here of consecutive opposite directions lets us
// use direction XOR 1 to get the opposite
const std::map<Direction, Vector2> directionLut = {
    { Direction::UP,    Vector2{  0.f, -1.f } },
    { Direction::DOWN,  Vector2{  0.f,  1.f } },
    { Direction::LEFT,  Vector2{ -1.f,  0.f } },
    { Direction::RIGHT, Vector2{  1.f,  0.f } }
};

inline bool isCentered(const Vector2 pos, const float TILE_SIZE) {
    const float ALIGNMENT_THRESHOLD = 1.5f; // for 60fps
    // const float ALIGNMENT_THRESHOLD = 6.f; // for 10fps
    const float TILE_CENTER_OFFSET = TILE_SIZE / 2.f;

    float distFromTileCenterX = fabs(
        (pos.x - TILE_CENTER_OFFSET) -
        (static_cast<int>(pos.x / TILE_SIZE) * TILE_SIZE));
    float distFromTileCenterY = fabs(
        (pos.y - TILE_CENTER_OFFSET) -
        (static_cast<int>(pos.y / TILE_SIZE) * TILE_SIZE));

    return (distFromTileCenterX < ALIGNMENT_THRESHOLD && 
            distFromTileCenterY < ALIGNMENT_THRESHOLD);
}

/*

LIGHTGRAY  // Color{ 200, 200, 200, 255 }
GRAY       // Color{ 130, 130, 130, 255 }
DARKGRAY   // Color{ 80, 80, 80, 255 }
YELLOW     // Color{ 253, 249, 0, 255 }
GOLD       // Color{ 255, 203, 0, 255 }
ORANGE     // Color{ 255, 161, 0, 255 }
PINK       // Color{ 255, 109, 194, 255 }
RED        // Color{ 230, 41, 55, 255 }
MAROON     // Color{ 190, 33, 55, 255 }
GREEN      // Color{ 0, 228, 48, 255 }
LIME       // Color{ 0, 158, 47, 255 }
DARKGREEN  // Color{ 0, 117, 44, 255 }
SKYBLUE    // Color{ 102, 191, 255, 255 }
BLUE       // Color{ 0, 121, 241, 255 }
DARKBLUE   // Color{ 0, 82, 172, 255 }
PURPLE     // Color{ 200, 122, 255, 255 }
VIOLET     // Color{ 135, 60, 190, 255 }
DARKPURPLE // Color{ 112, 31, 126, 255 }
BEIGE      // Color{ 211, 176, 131, 255 }
BROWN      // Color{ 127, 106, 79, 255 }
DARKBROWN  // Color{ 76, 63, 47, 255 }
WHITE      // Color{ 255, 255, 255, 255 }
BLACK      // Color{ 0, 0, 0, 255 }
BLANK      // Color{ 0, 0, 0, 0 }
MAGENTA    // Color{ 255, 0, 255, 255 }
RAYWHITE   // Color{ 245, 245, 245, 255 }
*/