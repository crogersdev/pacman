#pragma once

#include <SFML/Graphics.hpp>

#include <functional>
#include <memory>
#include <stack>
#include <unordered_map>

#include "../entities/Labyrinth.hpp"

class GameManager
{
private:
    sf::Clock m_clock;
    sf::FloatRect m_windowBounds;
    sf::Time m_deltaTime;

    float m_movementSpeed;

    std::shared_ptr<sf::RenderWindow> m_pWindow;

    Labyrinth m_labyrinth;

public:
    GameManager(std::shared_ptr<sf::RenderWindow> pWindow);
    ~GameManager();

    std::unordered_map<sf::Keyboard::Key, std::function<void()>> m_keyActions;
    sf::CircleShape m_pacman;
    sf::RectangleShape m_wall;
    float m_pacmanRadius;

    void handleInputs();
    void movePacman(sf::Vector2f);
    void updateWindow();
};