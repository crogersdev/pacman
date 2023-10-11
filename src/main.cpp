#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

#include "GameManager.hpp"

int main()
{
    auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(500, 720), "Pacman!");
    GameManager gameManager = GameManager(window);

    while (window->isOpen())
    {
        gameManager.handleInputs();

    }

    return 0;
}
