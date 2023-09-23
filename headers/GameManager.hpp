#pragma once
#include <stack>

#include <SFML/Graphics.hpp>

class State;
typedef std::unique_ptr<State> StatePointer;

class GameManager
{
private:
    sf::Clock mClock;

public:
    std::unique_ptr<sf::RenderWindow> m_pWindow;
    std::stack<StatePointer> m_gameStates;
    float m_deltaTime;
    float m_aspectRatio;

    /*
    GameManager(
        std::unique_ptr<sf::RenderWindow> pNewWindow,
        std::stack<StatePointer> newGameStates,
        float newDeltaTime = 0.0,
        float newAspectRatio
    ) :
        m_pWindow(std::move(pNewWindow)),
        m_gameStates(std::move(newGameStates)),
        m_deltaTime(newDeltaTime),
        m_aspectRatio(newAspectRatio)
    {}*/

    ~GameManager();

    void update();
    void startGameManager();
};