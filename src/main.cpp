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

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pacman!");
    SetTargetFPS(60);

    std::shared_ptr<Labyrinth> labyrinth = std::make_shared<Labyrinth>();
    std::shared_ptr<Pacman>    pacman = std::make_shared<Pacman>();
    std::shared_ptr<Ghost>     inky = std::make_shared<Ghost>("Inky", "res/inky.png", std::make_pair(10, 13), Vector2{ 2.f, 2.f });
    std::shared_ptr<Ghost>     pinky = std::make_shared<Ghost>("Pinky", "res/pinky.png", std::make_pair(10, 11), Vector2{ 22.f, 2.f });
    std::shared_ptr<Ghost>     blinky = std::make_shared<Ghost>("Blinky", "res/blinky.png", std::make_pair(12, 9), Vector2{ 22.f, 23.f }); 
    std::shared_ptr<Ghost>     clyde = std::make_shared<Ghost>("Clyde", "res/clyde.png", std::make_pair(14, 13), Vector2{ 2.f, 23.f });

    std::vector< std::shared_ptr<Ghost>> ghosts = { inky, pinky, blinky, clyde };
    GameManager gm = GameManager(ghosts, pacman, labyrinth);

    gm.initEntities();
    gm.runGame();

    CloseWindow();
    return 0;
}
