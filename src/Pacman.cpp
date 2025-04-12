#include "Pacman.hpp"

// if this isn't included last, then the definitions for Vector2
// get mangled or something.  the Vector2Add() function breaks
#include <raymath.h>

Pacman::Pacman()
    : mColor(GetColor(0xFFFF00FF)),
      mDirection{0., 0.},
      mRadius(12),
      mPosition{11*TILE_SIZE + TILE_SIZE / 2, 9*TILE_SIZE + TILE_SIZE / 2},
      mSpeed(2.)
{}

void Pacman::draw() {
    // #ifndef NDEBUG
    std::cout << "\t rendering pacman at row,  col: "
              << static_cast<int>(mPosition.y / TILE_SIZE)
              << ", "
              << static_cast<int>(mPosition.x / TILE_SIZE)
              << "\n";
    // #endif

    DrawCircle(mPosition.x, mPosition.y, mRadius, mColor);
}

void Pacman::move(const Labyrinth &rLabyrinth) {
    // Store previous position in case we need to revert
    Vector2 prevPosition = mPosition;
    
    // Reset velocity and get new direction
    mDirection = {0.0f, 0.0f};
    
    // Only allow one direction at a time (pacman can't move diagonally)
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) mDirection.y = -1.0f;
    else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) mDirection.x = -1.0f;
    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) mDirection.y = 1.0f;
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) mDirection.x = 1.0f;
    
    if (mDirection.x == 0.0f && mDirection.y == 0.0f) {
        return; // No movement needed
    }
    
    // Calculate desired movement
    Vector2 movement = Vector2Scale(mDirection, mSpeed * GetFrameTime());
    Vector2 newPosition = Vector2Add(mPosition, movement);
    
    // Get current grid position
    int currentCol = static_cast<int>(mPosition.x / TILE_SIZE);
    int currentRow = static_cast<int>(mPosition.y / TILE_SIZE);
    
    // Grid alignment when moving in corridors
    // This keeps pacman centered in tiles when moving along straight paths
    if (mDirection.x != 0.0f && fabs(mPosition.y - (currentRow * TILE_SIZE + TILE_SIZE/2)) > 1.0f) {
        // Align Y position to center of tile when moving horizontally
        newPosition.y = lerp(mPosition.y, currentRow * TILE_SIZE + TILE_SIZE/2, 0.2f);
    }
    else if (mDirection.y != 0.0f && fabs(mPosition.x - (currentCol * TILE_SIZE + TILE_SIZE/2)) > 1.0f) {
        // Align X position to center of tile when moving vertically
        newPosition.x = lerp(mPosition.x, currentCol * TILE_SIZE + TILE_SIZE/2, 0.2f);
    }
    
    // Check if new position will cause a collision
    bool collision = false;
    
    // For accurate collision detection, check multiple points on pacman's circumference
    // in the direction of movement
    if (mDirection.x != 0.0f) {
        // Check left/right collision points (center + offset up and down)
        int checkX = static_cast<int>((newPosition.x + mDirection.x * mRadius) / TILE_SIZE);
        
        for (float yOffset = -10.0f; yOffset <= 10.0f; yOffset += 5.0f) {
            int checkY = static_cast<int>((newPosition.y + yOffset) / TILE_SIZE);
            Labyrinth::Tile tile = rLabyrinth.at(checkY, checkX);
            if (tile == Labyrinth::Tile::WALL || tile == Labyrinth::Tile::GATE) {
                collision = true;
                break;
            }
        }
    }
    else if (mDirection.y != 0.0f) {
        // Check up/down collision points (center + offset left and right)
        int checkY = static_cast<int>((newPosition.y + mDirection.y * mRadius) / TILE_SIZE);
        
        for (float xOffset = -10.0f; xOffset <= 10.0f; xOffset += 5.0f) {
            int checkX = static_cast<int>((newPosition.x + xOffset) / TILE_SIZE);
            Labyrinth::Tile tile = rLabyrinth.at(checkY, checkX);
            if (tile == Labyrinth::Tile::WALL || tile == Labyrinth::Tile::GATE) {
                collision = true;
                break;
            }
        }
    }
    
    if (collision) {
        // Handle collision: snap to grid when hitting a wall to allow for clean turns
        if (mDirection.x != 0.0f) {
            // For horizontal movement, snap to the edge of the tile
            float snapX = (mDirection.x > 0.0f) ? 
                          (static_cast<int>(mPosition.x / TILE_SIZE) + 1) * TILE_SIZE - mRadius - 1.0f :
                          (static_cast<int>(mPosition.x / TILE_SIZE)) * TILE_SIZE + mRadius;
            mPosition.x = snapX;
        }
        else if (mDirection.y != 0.0f) {
            // For vertical movement, snap to the edge of the tile
            float snapY = (mDirection.y > 0.0f) ? 
                          (static_cast<int>(mPosition.y / TILE_SIZE) + 1) * TILE_SIZE - mRadius - 1.0f :
                          (static_cast<int>(mPosition.y / TILE_SIZE)) * TILE_SIZE + mRadius;
            mPosition.y = snapY;
        }
    }
    else {
        // Move if no collision
        mPosition = newPosition;
    }
}

// Helper function for smooth alignment
float Pacman::lerp(float a, float b, float f) {
    return a + f * (b - a);
}

// void Pacman::move(const Labyrinth &rLabyrinth) {

//     // reset velocity after each frame
//     mDirection = {0., 0.};
    
//     if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    mDirection.y = -1.0;
//     if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  mDirection.x = -1.0;
//     if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  mDirection.y = 1.0;
//     if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) mDirection.x = 1.0;

//     if (mDirection.x == 0. && mDirection.y == 0.) {
//         std::cout << "no keys pressed\n";
//         return;
//     }

//     int currentCol = static_cast<int>(mPosition.x / TILE_SIZE);
//     int currentRow = static_cast<int>(mPosition.y / TILE_SIZE);
//     int nextTileX = static_cast<int>((mPosition.x + mRadius) / TILE_SIZE) + mDirection.x;
//     int nextTileY = static_cast<int>((mPosition.y + mRadius) / TILE_SIZE) + mDirection.y;

//     Labyrinth::Tile nextTile = rLabyrinth.at(nextTileY, nextTileX);

//     if (nextTile == Labyrinth::Tile::WALL || nextTile == Labyrinth::Tile::GATE) {
//         std::cout << "next tile at row: " << nextTileY << ", col: " << nextTileX << " is a " << nextTile << "\n";
//         std::cout << "not letting you go in that direction\n";
//         return;
//     }

//     mPosition = Vector2Add(
//         mPosition, 
//         Vector2Scale(mDirection, mSpeed + GetFrameTime())
//     );


// }