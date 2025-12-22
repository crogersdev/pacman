#pragma once

#include <raylib.h>

class AudioManager {
public:
    enum class GameSound {
        EAT_PELLET    = 0,
        EAT_POWERUP   = 1,
        EAT_GHOST     = 2,
        POWERUP       = 3,
        DEATH         = 4,
        GAME_START    = 5,
        GAME_PLAY     = 6,
        GAME_OVER     = 7,
    };

    inline AudioManager()
        : mChomp(LoadSound("../../res/pacman_chomp.ogg"))
    {}

    inline void play(AudioManager::GameSound sound) {
        switch(sound) {
        case GameSound::EAT_PELLET:
            PlaySound(mChomp);
            break;
        case GameSound::EAT_POWERUP:
        case GameSound::EAT_GHOST:
        case GameSound::POWERUP:
        case GameSound::DEATH:
        case GameSound::GAME_START:
        case GameSound::GAME_PLAY:
        case GameSound::GAME_OVER:
        default:
            break;
        }
    }
private:
    Music mGameplay;
    Music mGameStart;
    Music mPlayerDies;

    Sound mChomp;
    Sound mPowerup;
    Sound mPlayerDeath;
    Sound mGhostDeath;

};
