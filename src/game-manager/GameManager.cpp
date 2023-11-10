#include "GameManager.hpp"
#include "../entities/Labyrinth.hpp"
#include <iostream>

GameManager::GameManager(std::shared_ptr<sf::RenderWindow> pWindow)
  : m_pWindow(std::move(pWindow))
{
    m_windowBounds = sf::FloatRect(0, 0, m_pWindow->getSize().x, m_pWindow->getSize().y);

    m_pacmanRadius = 30.f;
    m_pacman = sf::CircleShape(m_pacmanRadius);
    m_pacman.setFillColor(sf::Color::Yellow);
    m_pacman.setPosition(sf::Vector2f(60., 60.));

    m_wallTile = sf::RectangleShape(sf::Vector2f(60.f, 60.f));
    m_wallTile.setFillColor(sf::Color::Blue);

    m_labyrinth = Labyrinth();

    m_movementSpeed = 200.0f;

    m_keyActions = {
        {sf::Keyboard::Left,  [&]() { movePacman(sf::Vector2f(-m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
        {sf::Keyboard::Right, [&]() { movePacman(sf::Vector2f( m_movementSpeed * m_deltaTime.asSeconds(), 0)); }},
        {sf::Keyboard::Up,    [&]() { movePacman(sf::Vector2f(0, -m_movementSpeed * m_deltaTime.asSeconds())); }},
        {sf::Keyboard::Down,  [&]() { movePacman(sf::Vector2f(0,  m_movementSpeed * m_deltaTime.asSeconds())); }}
    };
}

GameManager::~GameManager() {}

void GameManager::handleInputs() {
    sf::Event event;
    while (m_pWindow->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            m_pWindow->close();
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            m_pWindow->close();
    }

    m_deltaTime = m_clock.restart();

    for (const auto& pair : m_keyActions) {
         if (sf::Keyboard::isKeyPressed(pair.first)) {
            // note: this invokes the lambdas defined in
            //       the ctor that move pacman
            pair.second();
        }
    }
}

void GameManager::movePacman(sf::Vector2f movement)
{
    const float radius = m_pacman.getRadius();
    sf::Vector2f newPosition = m_pacman.getPosition() + movement;

    auto wrapCoordinate = [](float &coord, float min, float max) {
        if (coord < min)
            coord = max;
        else if (coord > max)
            coord = min;
    };

    wrapCoordinate(newPosition.x, -radius * 2, m_windowBounds.width);
    wrapCoordinate(newPosition.y, -radius * 2, m_windowBounds.height);

    int currentLeft = static_cast<int>(m_pacman.getPosition().x / 60);
    int currentTop = static_cast<int>(m_pacman.getPosition().y / 60);
    int currentRight = static_cast<int>((m_pacman.getPosition().x + 60) / 60);
    int currentBottom = static_cast<int>((m_pacman.getPosition().y + 60) / 60);

    int leftNeighbor = static_cast<int>(newPosition.x / 60);
    int topNeighbor = static_cast<int>(newPosition.y / 60);
    int rightNeighbor = static_cast<int>((newPosition.x + 60) / 60);
    int bottomNeighbor = static_cast<int>((newPosition.y + 60) / 60);

    if (static_cast<bool>(m_labyrinth.m_labyrinth[currentTop][leftNeighbor])) {
        std::cout << "you have collided with tile (" << currentTop << ", " << leftNeighbor << ")\n"; return;
    }
    if (static_cast<bool>(m_labyrinth.m_labyrinth[currentTop][rightNeighbor])) {
        std::cout << "you have collided with tile (" << currentTop << ", " << rightNeighbor << ")\n"; return;
    }

    if (static_cast<bool>(m_labyrinth.m_labyrinth[currentBottom][leftNeighbor])) {
        std::cout << "you have collided with tile (" << currentBottom << ", " << leftNeighbor << ")\n"; return;
    }
    if (static_cast<bool>(m_labyrinth.m_labyrinth[currentBottom][rightNeighbor])) {
        std::cout << "you have collided with tile (" << currentBottom << ", " << rightNeighbor << ")\n"; return;
    }

    if (static_cast<bool>(m_labyrinth.m_labyrinth[topNeighbor][currentLeft])) { std::cout << "you have collided with tile (" << topNeighbor << ", " << currentLeft << ")\n"; return; }
    if (static_cast<bool>(m_labyrinth.m_labyrinth[bottomNeighbor][currentLeft])) { std::cout << "you have collided with tile (" << bottomNeighbor << ", " << currentLeft << ")\n"; return; }

    if (static_cast<bool>(m_labyrinth.m_labyrinth[topNeighbor][currentRight])) { std::cout << "you have collided with tile (" << topNeighbor << ", " << currentRight << ")\n"; return; }
    if (static_cast<bool>(m_labyrinth.m_labyrinth[bottomNeighbor][currentLeft])) { std::cout << "you have collided with tile (" << bottomNeighbor << ", " << currentRight << ")\n"; return; }

    // TRICKY: we avoid .move(movement) here because doing so would ignore
    //         the arithmetic we implemented to wrap pacman around the edges
    m_pacman.setPosition(newPosition);
}

void GameManager::updateWindow()
{
    m_pWindow->clear();

    for (int row = 0; row < m_labyrinth.m_labyrinthRows; ++row)
    {
        for (int col = 0; col < m_labyrinth.m_labyrinthCols; ++col)
        {
            if (bool(m_labyrinth.m_labyrinth[row][col]))
            {
                m_wallTile.setPosition(sf::Vector2f((float)60*col, (float)60*row));
                m_walls.push_back(m_wallTile);
                m_pWindow->draw(m_wallTile);
            }
        }
    }

    m_pWindow->draw(m_pacman);
    m_pWindow->display();
}
