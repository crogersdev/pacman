#pragma once

#include <SFML/Graphics.hpp>

#include <functional>
#include <memory>
#include <stack>
#include <unordered_map>

class GameManager
{
private:
    sf::Clock m_clock;
    sf::FloatRect m_windowBounds;
    sf::Time m_deltaTime;

    float m_movementSpeed;

    std::shared_ptr<sf::RenderWindow> m_pWindow;

public:
    std::unordered_map<sf::Keyboard::Key, std::function<void()>> m_keyActions;
    sf::CircleShape m_pacman;
    float m_pacmanRadius;
    GameManager(std::shared_ptr<sf::RenderWindow> pWindow);
    ~GameManager();

    void handleInputs();
    void movePacman(sf::Vector2f);
    void updateWindow();
};