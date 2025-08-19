#include <iostream>

#include <raylib.h>

#include "GameManager.hpp"
#include "Ghost.hpp"
#include "Labyrinth.hpp"
#include "Pacman.hpp"
#include "helpers/Movement.hpp"

int main() {
    const int SCREEN_WIDTH = LABYRINTH_COLS * TILE_SIZE;
    const int SCREEN_HEIGHT = 800;

    // std::cout << "Current working directory: " << GetWorkingDirectory() << "\n";

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    std::shared_ptr<Labyrinth> labyrinth = std::make_shared<Labyrinth>();
    std::shared_ptr<Pacman>    pacman = std::make_shared<Pacman>();
    std::shared_ptr<Ghost>     pinky = std::make_shared<Ghost>("Pinky", "res/pinky.png", Vector2{ 10, 11 }, Vector2{ 1, 14 });
    std::shared_ptr<Ghost>     blinky = std::make_shared<Ghost>("Blinky", "res/blinky.png", Vector2{ 10, 13 }, Vector2{ 1, 1 }); 
    blinky->mState = Ghost::State::IN_PRISON;
    pinky->mState = Ghost::State::IN_PRISON;

    std::vector< std::shared_ptr<Ghost>> ghosts = { blinky, pinky };
    GameManager gm = GameManager(ghosts, pacman, labyrinth);

    gm.startGame();

    while (WindowShouldClose() == false) {
        std::cout << 
            "blinky state: " <<
            blinky->mState <<
            "\tchasing " <<
            static_cast<int>(blinky->mChaseTarget.y / TILE_SIZE) <<
            ", " <<
            static_cast<int>(blinky->mChaseTarget.x / TILE_SIZE) <<
            "\tcurrently at " <<
            static_cast<int>(blinky->mPosition.y / TILE_SIZE) <<
            ", " <<
            static_cast<int>(blinky->mPosition.x / TILE_SIZE) <<
            "\n";

        BeginDrawing();
        ClearBackground(BLACK);

        gm.moveStuff();

        gm.checkCollisions();

        gm.updateTimers();

        gm.drawStuff();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
