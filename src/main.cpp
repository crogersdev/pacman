#include <iostream>

#include <raylib.h>

#include "GameManager.hpp"
#include "Ghost.hpp"
#include "Labyrinth.hpp"
#include "Pacman.hpp"
#include "helpers/CollisionDetection.hpp"
#include "helpers/Movement.hpp"

int main() {
    const int SCREEN_WIDTH = LABYRINTH_COLS * static_cast<int>(TILE_SIZE);
    const int SCREEN_HEIGHT = 800;

    // std::cout << "Current working directory: " << GetWorkingDirectory() << "\n";

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    GameManager gm = GameManager();
    Labyrinth labyrinth = Labyrinth();
    Pacman pacman = Pacman();
    Ghost blinky = Ghost("res/blinky.png", Vector2{ 8, 13 }); 
    blinky.mState = Ghost::State::CHASE;
    Ghost pinky = Ghost("res/pinky.png", Vector2{ 8, 12 });
    pinky.mState = Ghost::State::MEANDER;

    Vector2 intendedDirection = { 0.f, 0.f };

    while (WindowShouldClose() == false) {
        BeginDrawing();
        ClearBackground(BLACK);
        labyrinth.draw();

        intendedDirection = { 0.f, 0.f };
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
            intendedDirection = { 0.f, 1.f };
        }
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
            intendedDirection = { 0.f, -1.f };
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            intendedDirection = { 1.f, 0.f };
        }
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            intendedDirection = { -1.f, 0.f };
        }

        pacman.move(intendedDirection, labyrinth);
        blinky.act(labyrinth, pacman.mPosition);
        pinky.act(labyrinth, pacman.mPosition);

        std::vector<Ghost> ghosts = { blinky, pinky };

        // collisions
        Vector2 pacmanTilePosition = pacman.getTilePosition();
        int pacmanTileX = static_cast<int>(pacmanTilePosition.x);
        int pacmanTileY = static_cast<int>(pacmanTilePosition.y);

        // pellet collisions
        if (labyrinth.mPellets.find({ pacmanTileX, pacmanTileY }) != labyrinth.mPellets.end()) {
            labyrinth.mPellets.erase({ pacmanTileX, pacmanTileY });
            gm.onDotsEaten();
            std::cout << "nomnomnom\n";
        }

        // powerup collisions
        if (labyrinth.mPowerups.find({ pacmanTileX, pacmanTileY }) != labyrinth.mPowerups.end()) {
            labyrinth.mPowerups.erase({ pacmanTileX, pacmanTileY });
            gm.onPowerUpEaten();
            for (Ghost& ghost : ghosts) {
                ghost.mState = Ghost::State::FRIGHTENED;
            }
        }

        // ghost collisions
        for (const Ghost& ghost : ghosts) {
            if (pacmanTilePosition.x == ghost.getTilePosition().x &&
                pacmanTilePosition.y == ghost.getTilePosition().y) {
                    // TODO: this is where pacman dies
                    std::cout << "PACMAN DEAD YO\n";
                }
        }

        std::cout << "score: " << gm.getScore() << "\n";

        pacman.draw();
        blinky.draw();
        pinky.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
