#pragma once

#include <SFML/Graphics.hpp>

#include <functional>
#include <memory>

#include "../entities/Ghosts.hpp"
#include "../entities/Labyrinth.hpp"
#include "../entities/Pacman.hpp"

class GameManager
{
public:
  // NOTE: apparently single arg ctors _should_ be explicit.
  explicit GameManager(std::shared_ptr<sf::RenderWindow> pWindow);
  ~GameManager();

  std::map<sf::Keyboard::Key, std::function<void(sf::Time)>> m_keyActions;
  
  // TODO: should these guys be private?
  Pacman m_pacman;
  Ghost m_pinky;

  void drawLabyrinth();
  void handleInputs();
  void updateEntities();
  void updateWindow();

private:

  struct GameHUD
  {
    sf::Font font;
    sf::Text debugText;
    sf::Text score;
    static constexpr unsigned int numGuys = 3;
    std::array<sf::CircleShape, numGuys> guys;

    GameHUD()
    {
      if (!font.loadFromFile("./res/PublicPixel.ttf"))
        throw std::runtime_error("Failed to load font from file!");
      
      debugText.setFont(font);
      debugText.setCharacterSize(18);
      debugText.setFillColor(sf::Color::White);
      debugText.setPosition(10.f, TILE_SIZE * LABYRINTH_ROWS + 35.f);

      score.setFont(font);
      score.setCharacterSize(22);
      score.setFillColor(sf::Color::White);
      score.setPosition(10.f, TILE_SIZE * LABYRINTH_ROWS + 10.f);

      for (unsigned int g; g < numGuys; ++g)
      {
        guys[g].setRadius(12.f);
        guys[g].setFillColor(sf::Color::Yellow);
        guys[g].setPosition(TILE_SIZE * LABYRINTH_COLS - 35.f - (g+1)*26.f, TILE_SIZE * LABYRINTH_ROWS + 10.f);
      }
    }

    void drawGuys(std::shared_ptr<sf::RenderWindow> pWin)
    {
      for (const auto& g : guys)
        pWin->draw(g);
    }
  };

  sf::Clock m_clock;
  sf::FloatRect m_windowBounds;
  GameHUD m_hud;
  sf::Time m_deltaTime;
  std::shared_ptr<sf::RenderWindow> m_pGameWindow;
  Labyrinth m_labyrinth;

  float m_fps;

  int m_score;
  int m_pelletValue;

  bool m_debugMode;

  const float m_tileSizeX = TILE_SIZE;
  const float m_tileSizeY = TILE_SIZE;
};