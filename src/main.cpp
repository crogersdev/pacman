#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

void movePacman(sf::CircleShape &, float, float, sf::RenderWindow &);

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 720), "Pacman!");

    sf::CircleShape pacman(30.0f);
    pacman.setFillColor(sf::Color::Yellow);

    sf::Clock gameClock;
    sf::Time deltaTime;
    float movementSpeed = 200.0f;

    std::unordered_map<sf::Keyboard::Key, std::function<void()>> keyActions = {
        {sf::Keyboard::Left,  [&]() { movePacman(pacman, -movementSpeed * deltaTime.asSeconds(), 0, window); }},
        {sf::Keyboard::Right, [&]() { movePacman(pacman,  movementSpeed * deltaTime.asSeconds(), 0, window); }},
        {sf::Keyboard::Up,    [&]() { movePacman(pacman, 0, -movementSpeed * deltaTime.asSeconds(), window); }},
        {sf::Keyboard::Down,  [&]() { movePacman(pacman, 0,  movementSpeed * deltaTime.asSeconds(), window); }}
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

void movePacman(sf::CircleShape &rPacman, float offset, float direction, sf::RenderWindow &rWindow) {
    sf::FloatRect windowBounds(0, 0, rWindow.getSize().x, rWindow.getSize().y);
    rPacman.move(sf::Vector2f(offset, direction));

    sf::Vector2f newPosition = rPacman.getPosition();

    // tricky: this breaks when rPacman is no longer a CircleShape - to say nothing of the function signataure
    if (newPosition.x < windowBounds.left ||
        newPosition.x + 60.0f > windowBounds.left + windowBounds.width ||
        newPosition.y < windowBounds.top ||
        newPosition.y + 60.0f > windowBounds.top + windowBounds.height)
    {
        std::cout << "out!\n";
    }
    else
    {
        std::cout << "in!!!\n";
    }
    // std::cout << "current position x: " << newPosition.x << "\t\ty: " << newPosition.y << "\n";
}