#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

#include "GameManager.hpp"

int main() {
  auto height = TILE_SIZE * LABYRINTH_ROWS + 6 * TILE_SIZE;
  auto width  = TILE_SIZE * (LABYRINTH_COLS - 1) + 400;  // extra 400 to print out some info

  auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(width, height)), "Pacman!");
  GameManager gameManager = GameManager(window);

  while (window->isOpen()) {
    //gameManager.handleInputs();

    if (!gameManager.mPaused) {
      gameManager.updateEntities();
    }

    gameManager.updateWindow();
  }

  return 0;
}
