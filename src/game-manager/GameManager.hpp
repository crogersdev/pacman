#pragma once

#include <functional>
#include <map>
#include <memory>

#include <SFML/Graphics.hpp>

#include "../entities/Ghosts.hpp"
#include "../entities/Labyrinth.hpp"
#include "../entities/Pacman.hpp"

class GameManager
{
private:
  Labyrinth mLabyrinth;

public:
  // NOTE: apparently single arg ctors _should_ be explicit.
  explicit GameManager(std::shared_ptr<sf::RenderWindow> pWindow);
  ~GameManager();

  Pacman mPacman;
  Ghost mInky;
  Ghost mBlinky;
  Ghost mPinky;
  Ghost mClyde;

  void handleInputs();
  void updateEntities();
  void updateWindow();

  std::map<sf::Keyboard::Key, std::function<void(sf::Time)>> mKeyActions;
  bool mPaused;

private:
  struct HUD {
    sf::Font hudFont;

    HUD() {
      if (!hudFont.openFromFile("./res/PublicPixel.ttf"))
        throw std::runtime_error("Failed to load font from file!");
    }
  };

  struct ExtraDebugHUD : public HUD {
    sf::Text debugText;
    sf::Font debugHudFont;

    ExtraDebugHUD()
      : debugText(debugHudFont) {
      if (!debugHudFont.openFromFile("./res/Bitty.ttf"))
        throw std::runtime_error("Failed to load font from file!");

      debugText.setFont(hudFont);
      debugText.setCharacterSize(28);
      debugText.setFillColor(sf::Color::White);
      debugText.setPosition(sf::Vector2f(TILE_SIZE * (LABYRINTH_COLS + 1), 10.f));
    }
  };

  struct GameHUD : public HUD {
    sf::Text scoreText;
    static constexpr unsigned int numGuys = 3;
    std::array<sf::CircleShape, numGuys> guys;

    GameHUD()
    : scoreText(hudFont) {
      scoreText.setCharacterSize(22);
      scoreText.setFillColor(sf::Color::White);
      scoreText.setPosition(sf::Vector2f(10.f, TILE_SIZE * LABYRINTH_ROWS + 10.f));

      for (unsigned int g = 0; g < numGuys; ++g) {
        guys[g].setRadius(12.f);
        guys[g].setFillColor(sf::Color::Yellow);
        guys[g].setPosition(
          sf::Vector2f(TILE_SIZE * LABYRINTH_COLS - 35.f - (g+1)*26.f,
          TILE_SIZE * LABYRINTH_ROWS + 10.f));
      }
    }

    void drawGuys(std::shared_ptr<sf::RenderWindow> pWin) const {
      for (const auto& g : guys)
        pWin->draw(g);
    }
  };

  sf::Clock mClock;
  sf::Clock mStateClock;
  sf::IntRect mWindowBounds;
  sf::Vector2i mMousePos;
  GameHUD mGameHud;
  ExtraDebugHUD mDebugHud;
  std::shared_ptr<sf::RenderWindow> mpGameWindow;
  float mFps;
  int mScore;
  int mPelletValue;
  bool mDebugMode;
};
