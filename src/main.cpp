#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

#include "GameManager.hpp"

int main()
{
  auto height = TILE_SIZE * LABYRINTH_ROWS + 4 * TILE_SIZE;
  auto width  = TILE_SIZE * (LABYRINTH_COLS - 1);
  auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), "Pacman!");
  GameManager gameManager = GameManager(window);

  while (window->isOpen())
  {
    gameManager.handleInputs();
    gameManager.updateWindow();
  }

  return 0;
}
