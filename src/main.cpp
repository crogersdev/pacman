#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

void movePacman(sf::CircleShape &, sf::Vector2f, sf::RenderWindow &);

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 720), "Pacman!");

    sf::CircleShape pacman(30.0f);
    pacman.setFillColor(sf::Color::Yellow);

    sf::Clock gameClock;
    sf::Time deltaTime;
    float movementSpeed = 200.0f;

    std::unordered_map<sf::Keyboard::Key, std::function<void()>> keyActions =
    {
        {sf::Keyboard::Left,  [&]() { movePacman(pacman, sf::Vector2f(-movementSpeed * deltaTime.asSeconds(), 0), window); }},
        {sf::Keyboard::Right, [&]() { movePacman(pacman, sf::Vector2f( movementSpeed * deltaTime.asSeconds(), 0), window); }},
        {sf::Keyboard::Up,    [&]() { movePacman(pacman, sf::Vector2f(0, -movementSpeed * deltaTime.asSeconds()), window); }},
        {sf::Keyboard::Down,  [&]() { movePacman(pacman, sf::Vector2f(0,  movementSpeed * deltaTime.asSeconds()), window); }}
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

        for (const auto& pair : keyActions)
        {
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

void movePacman(sf::CircleShape &rPacman, sf::Vector2f movement, sf::RenderWindow &rWindow)
{
    sf::FloatRect windowBounds(0, 0, rWindow.getSize().x, rWindow.getSize().y);
    const float radius = rPacman.getRadius();
    rPacman.move(movement);

    sf::Vector2f newPosition = rPacman.getPosition();
    
    auto wrapCoordinate = [](float &coord, float min, float max)
    {
        if (coord < min)
            coord = max;
        else if (coord > max)
            coord = min;
    };

    wrapCoordinate(newPosition.x, -radius * 2, windowBounds.width);
    wrapCoordinate(newPosition.y, -radius * 2, windowBounds.height);

    rPacman.setPosition(newPosition.x, newPosition.y);
}