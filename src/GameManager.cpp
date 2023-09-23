#include "../headers/GameManager.hpp"

using std::unique_ptr;
using std::make_unique;

void GameManager::startGameManager()
{
    m_pWindow = make_unique<sf::RenderWindow>();
    m_pWindow->create(
        sf::VideoMode(1000, 1100),
        "Pacman",
        sf::Style::Fullscreen
    );

    sf::Vector2u windowSize = m_pWindow->getSize();

    float x = static_cast<float>(windowSize.x);
    float y = static_cast<float>(windowSize.y);

    m_aspectRatio = (x / y) - 100;

    sf::View v(
        sf::Vector2f(400, 450),
        sf::Vector2f(800 * m_aspectRatio, 900)
    );

    m_pWindow->setView(v);

    //m_gameStates.push(a new main menu state)
}