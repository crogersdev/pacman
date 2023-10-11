#include "GameManager.hpp"

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pWindow(std::move(pWindow))
{
    m_windowBounds = sf::FloatRect(0, 0, m_pWindow->getSize().x, m_pWindow->getSize().y);
    m_pacmanRadius = 30.f;
    m_pacman = sf::CircleShape(m_pacmanRadius);
    m_pacman.setFillColor(sf::Color::Yellow);

    m_movementSpeed = 200.0f;

    m_keyActions =
    {
        {sf::Keyboard::Left,  [&]() { movePacman(sf::Vector2f(-m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
        {sf::Keyboard::Right, [&]() { movePacman(sf::Vector2f( m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
        {sf::Keyboard::Up,    [&]() { movePacman(sf::Vector2f(0, -m_movementSpeed * m_deltaTime.asSeconds())); }},
        {sf::Keyboard::Down,  [&]() { movePacman(sf::Vector2f(0,  m_movementSpeed * m_deltaTime.asSeconds())); }}
    };
}

GameManager::~GameManager() {}

void GameManager::handleInputs()
{
    sf::Event event;
    while (m_pWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            m_pWindow->close();
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            m_pWindow->close();
        }
    }

    m_deltaTime = m_clock.restart();

    for (const auto& pair : m_keyActions)
    {
        if (sf::Keyboard::isKeyPressed(pair.first)) {
            pair.second();
        }
    }
}

void GameManager::movePacman(sf::Vector2f movement)
{
    const float radius = m_pacman.getRadius();
    m_pacman.move(movement);

    sf::Vector2f newPosition = m_pacman.getPosition();
    
    auto wrapCoordinate = [](float &coord, float min, float max)
    {
        if (coord < min)
            coord = max;
        else if (coord > max)
            coord = min;
    };

    wrapCoordinate(newPosition.x, -radius * 2, m_windowBounds.width);
    wrapCoordinate(newPosition.y, -radius * 2, m_windowBounds.height);

    m_pacman.setPosition(newPosition.x, newPosition.y);
}

void GameManager::updateWindow()
{
    m_pWindow->clear();
    m_pWindow->draw(m_pacman);
    m_pWindow->display();

}