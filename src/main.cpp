#include <SFML/Graphics.hpp>

#include <memory>
#include <functional>
#include <unordered_map>

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 720), "Pacman!");

    sf::CircleShape pacman(30.0f);
    pacman.setFillColor(sf::Color::Yellow);

    sf::Clock gameClock;
    sf::Time deltaTime;
    float movementSpeed = 200.0f;

    std::unordered_map<sf::Keyboard::Key, std::function<void()>> keyActions = {
        {sf::Keyboard::Left,  [&]() { pacman.move(-movementSpeed * deltaTime.asSeconds(), 0); }},
        {sf::Keyboard::Right, [&]() { pacman.move( movementSpeed * deltaTime.asSeconds(), 0); }},
        {sf::Keyboard::Up,    [&]() { pacman.move(0, -movementSpeed * deltaTime.asSeconds()); }},
        {sf::Keyboard::Down,  [&]() { pacman.move(0,  movementSpeed * deltaTime.asSeconds()); }}
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
