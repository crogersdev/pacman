#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

#include "game-manager/GameManager.hpp"

int main() {
  unsigned int height = TILE_SIZE * LABYRINTH_ROWS + 6 * TILE_SIZE;
  unsigned int width  = TILE_SIZE * (LABYRINTH_COLS - 1) + 400;  // extra 400 to print out some info

  auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(width, height), 24), "Pacman!");
  GameManager gameManager = GameManager(window);

  while (window->isOpen()) {
    gameManager.handleInputs();

    if (!gameManager.mPaused) {
      gameManager.updateEntities();
    }

    gameManager.updateWindow();
  }

  return 0;
}
