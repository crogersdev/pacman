#include <iostream>
#include <memory>

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
    std::shared_ptr<Ghost>     inky = std::make_shared<Ghost>("Inky", "res/inky.png", Vector2{ 10, 13 }, Vector2{ 2, 2 });
    std::shared_ptr<Ghost>     pinky = std::make_shared<Ghost>("Pinky", "res/pinky.png", Vector2{ 10, 11 }, Vector2{ 22, 2 });
    std::shared_ptr<Ghost>     blinky = std::make_shared<Ghost>("Blinky", "res/blinky.png", Vector2{ 12, 9 }, Vector2{ 22, 23 }); 
    std::shared_ptr<Ghost>     clyde = std::make_shared<Ghost>("Clyde", "res/clyde.png", Vector2{ 14, 13 }, Vector2{ 2, 23 });

    // std::vector< std::shared_ptr<Ghost>> ghosts = { inky, pinky, blinky, clyde };
    std::vector< std::shared_ptr<Ghost>> ghosts = { blinky };
    GameManager gm = GameManager(ghosts, pacman, labyrinth);

    gm.startGame();

    bool paused = true;

    while (WindowShouldClose() == false) {
        if (IsKeyPressed(KEY_P)) {
            paused = !paused;
        }
 
        BeginDrawing();
        ClearBackground(BLACK);

        if (!paused) {
            gm.moveStuff();

            gm.checkCollisions();

            gm.updateTimers();
        }

        gm.drawStuff();

        EndDrawing();

    }

    CloseWindow();
    return 0;
}
