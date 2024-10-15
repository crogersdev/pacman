#pragma once

#include <SFML/Graphics.hpp>

#include <functional>
#include <map>
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

  Pacman m_pacman;
  Ghost m_inky;
  Ghost m_blinky;
  Ghost m_pinky;
  Ghost m_clyde;

  void drawLabyrinth();
  void handleInputs();
  void updateEntities();
  void updateWindow();

private:
  struct HUD {
    sf::Font font;

    HUD() {
      if (!font.loadFromFile("./res/PublicPixel.ttf"))
        throw std::runtime_error("Failed to load font from file!");
    }
  };

  struct ExtraDebugHUD : public HUD {
    sf::Text debugText;

    ExtraDebugHUD() {
      if (!font.loadFromFile("./res/Bitty.ttf"))
        throw std::runtime_error("Failed to load font from file!");

      debugText.setFont(font);
      debugText.setCharacterSize(28);
      debugText.setFillColor(sf::Color::White);

      debugText.setPosition(TILE_SIZE * (LABYRINTH_COLS + 1), 10.f);
    }
  };

  struct GameHUD : public HUD {
    sf::Text debugText;
    sf::Text score;
    static constexpr unsigned int numGuys = 3;
    std::array<sf::CircleShape, numGuys> guys;

    GameHUD() {
      debugText.setFont(font);
      debugText.setCharacterSize(18);
      debugText.setFillColor(sf::Color::White);
      debugText.setPosition(10.f, TILE_SIZE * LABYRINTH_ROWS + 36.f);

      score.setFont(font);
      score.setCharacterSize(22);
      score.setFillColor(sf::Color::White);
      score.setPosition(10.f, TILE_SIZE * LABYRINTH_ROWS + 10.f);

      for (unsigned int g = 0; g < numGuys; ++g) {
        guys[g].setRadius(12.f);
        guys[g].setFillColor(sf::Color::Yellow);
        guys[g].setPosition(TILE_SIZE * LABYRINTH_COLS - 35.f - (g+1)*26.f, TILE_SIZE * LABYRINTH_ROWS + 10.f);
      }
    }

    void drawGuys(std::shared_ptr<sf::RenderWindow> pWin) {
      for (const auto& g : guys)
        pWin->draw(g);
    }
  };

  sf::Clock m_clock;
  sf::FloatRect m_windowBounds;
  GameHUD m_gameHud;
  ExtraDebugHUD m_debugHud;

  sf::Time m_deltaTime;
  std::shared_ptr<sf::RenderWindow> m_pGameWindow;
  Labyrinth m_labyrinth;

  float m_fps;

  int m_score;
  int m_pelletValue;

  bool m_debugMode;
};
