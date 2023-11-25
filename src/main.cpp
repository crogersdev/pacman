#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

#include "GameManager.hpp"

int main()
{
  auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(60*10, 60*15), "Pacman!");
  GameManager gameManager = GameManager(window);

  while (window->isOpen())
  {
    gameManager.handleInputs();
    gameManager.updateWindow();
  }

  return 0;
}
