#include <iostream>

#include <raylib.h>

#include "GameManager.hpp"
#include "Ghost.hpp"
#include "Labyrinth.hpp"
#include "Pacman.hpp"
#include "helpers/Movement.hpp"

int main() {
    const int SCREEN_WIDTH = LABYRINTH_COLS * static_cast<int>(TILE_SIZE);
    const int SCREEN_HEIGHT = 800;

    // std::cout << "Current working directory: " << GetWorkingDirectory() << "\n";

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    Labyrinth labyrinth = Labyrinth();
    std::shared_ptr<Pacman> pacman = std::make_shared<Pacman>();
    std::shared_ptr<Ghost> blinky = std::make_shared<Ghost>("res/blinky.png", Vector2{ 8, 13 }, Vector2{ 1, 1 }); 
    blinky->mState = Ghost::State::MEANDER;
    std::shared_ptr<Ghost> pinky = std::make_shared<Ghost>("res/pinky.png", Vector2{ 8, 12 }, Vector2{ 1, 14 });
    pinky->mState = Ghost::State::MEANDER;

    std::vector< std::shared_ptr<Ghost>> ghosts = { blinky, pinky };
    GameManager gm = GameManager(ghosts, pacman);

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

        pacman->move(intendedDirection, labyrinth);
       
        for (const auto& ghost : ghosts) { ghost->act(labyrinth); }

        // collisions
        Vector2 pacmanTilePosition = pacman->getTilePosition();
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
        }

        // ghost collisions
        for (const auto& ghost : ghosts) {
            if (pacmanTilePosition.x == ghost->getTilePosition().x &&
                pacmanTilePosition.y == ghost->getTilePosition().y) {
                    gm.onDeath();
                }
        }

        gm.updateTimers();
        pacman->draw();
        for (const auto& ghost : ghosts) { ghost->draw(); }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
