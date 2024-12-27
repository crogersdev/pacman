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

  void drawLabyrinth();
  void handleInputs();
  void updateEntities();
  void updateWindow();

  std::map<sf::Keyboard::Key, std::function<void(sf::Time)>> mKeyActions;
  bool mPaused;

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

    void drawGuys(std::shared_ptr<sf::RenderWindow> pWin) const {
      for (const auto& g : guys)
        pWin->draw(g);
    }
  };

  sf::Clock mClock;
  sf::Clock mStateClock;
  sf::FloatRect mWindowBounds;
  sf::Vector2i mMousePos;
  GameHUD mGameHud;
  ExtraDebugHUD mDebugHud;

  std::shared_ptr<sf::RenderWindow> mpGameWindow;

  float mFps;

  int mScore;
  int mPelletValue;

  bool mDebugMode;
};
