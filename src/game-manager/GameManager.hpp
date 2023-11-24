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
    sf::Vector2f m_initialPosition;
    std::shared_ptr<sf::RenderWindow> m_pWindow;
    std::vector<sf::RectangleShape> m_walls;
    float m_movementSpeed;
    Labyrinth m_labyrinth;

    const float m_tileSizeX;
    const float m_tileSizeY;

public:
    GameManager(std::shared_ptr<sf::RenderWindow> pWindow);
    ~GameManager();

    std::unordered_map<sf::Keyboard::Key, std::function<void()>> m_keyActions;
    sf::CircleShape m_pacman;
    std::vector<sf::RectangleShape> m_collisionTiles;
    sf::RectangleShape m_wallTile;
    float m_pacmanRadius;

    void drawLabyrinth();
    void handleInputs();
    void movePacman(sf::Vector2f);
    void updateWindow();
};