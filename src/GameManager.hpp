#pragma once
#include <stack>

#include <SFML/Graphics.hpp>

typedef std::unique_ptr<State> StatePointer;

class State;

class GameManager
{
private:
    sf::Clock mClock;

public:
    // NOTE: i think the reason there's no ctor is because the startGameManager initializes
    //       the values for the GameManager object and it also doesn't look like there's
    //       gonna be multiple 
    std::unique_ptr<sf::RenderWindow> m_pWindow;
    std::stack<StatePointer> m_gameStates;
    float m_deltaTime;
    float m_aspectRatio;

    ~GameManager();

    void update();
    void startGameManager();
};