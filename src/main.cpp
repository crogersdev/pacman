#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

void movePacman(sf::CircleShape &, float, float);

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 720), "Pacman!");

    sf::CircleShape pacman(30.0f);
    pacman.setFillColor(sf::Color::Yellow);

    sf::Clock gameClock;
    sf::Time deltaTime;
    float movementSpeed = 200.0f;

    std::unordered_map<sf::Keyboard::Key, std::function<void()>> keyActions = {
        {sf::Keyboard::Left,  [&]() { movePacman(pacman, -movementSpeed * deltaTime.asSeconds(), 0); }},
        {sf::Keyboard::Right, [&]() { movePacman(pacman,  movementSpeed * deltaTime.asSeconds(), 0); }},
        {sf::Keyboard::Up,    [&]() { movePacman(pacman, 0, -movementSpeed * deltaTime.asSeconds()); }},
        {sf::Keyboard::Down,  [&]() { movePacman(pacman, 0,  movementSpeed * deltaTime.asSeconds()); }}
    };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        deltaTime = gameClock.restart();

        for (const auto& pair : keyActions) {
            if (sf::Keyboard::isKeyPressed(pair.first)) {
                pair.second();
            }
        }

        window.clear();
        window.draw(pacman);
        window.display();
    }

    return 0;
}

void movePacman(sf::CircleShape &rPacman, float offset, float direction) {
    rPacman.move(sf::Vector2f(offset, direction));
}