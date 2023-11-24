#pragma once

#include <SDL2/SDL_mixer.h>
#include <stdexcept>
#include <map>

class SoundPlayer {
private:
    static std::map<std::string, Mix_Chunk*> soundMap;
    static Mix_Music* backgroundMusic;

public:
    // Initialize SDL_mixer
    static void init() {
        // Make sure to call this only once in your program
        if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
            throw std::runtime_error("Unable to initialize SDL_mixer! SDL_mixer Error: " + std::string(Mix_GetError()));
        }
    }

    // Load a sound file and store it with a key
    static void load(const std::string& key, const char* filePath) {
        Mix_Chunk* sound = Mix_LoadWAV(filePath);
        if (!sound) {
            throw std::runtime_error("Unable to load sound! SDL_mixer Error: " + std::string(Mix_GetError()));
        }
        soundMap[key] = sound;
    }

    // Play a sound indefinitely using a key
    static void playIndefinitely(const std::string& key) {
        auto it = soundMap.find(key);
        if (it != soundMap.end()) {
            if (Mix_PlayChannel(-1, it->second, -1) == -1) {
                throw std::runtime_error("Unable to play sound! SDL_mixer Error: " + std::string(Mix_GetError()));
            }
        } else {
            throw std::runtime_error("Sound key not found!");
        }
    }

    // Play a sound for a specified duration using a key
    static void playTimed(const std::string& key, int durationInMilliseconds) {
        auto it = soundMap.find(key);
        if (it != soundMap.end()) {
            if (Mix_PlayChannelTimed(-1, it->second, 0, durationInMilliseconds) == -1) {
                throw std::runtime_error("Unable to play sound! SDL_mixer Error: " + std::string(Mix_GetError()));
            }
        } else {
            throw std::runtime_error("Sound key not found!");
        }
    }

    static void loadBackgroundMusic(const char* filePath) {
        backgroundMusic = Mix_LoadMUS(filePath);
        if (!backgroundMusic) {
            throw std::runtime_error("Unable to load background music! SDL_mixer Error: " + std::string(Mix_GetError()));
        }
        Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Set music volume to half of the maximum
    }

    static void playBackgroundMusic() {
        if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
            throw std::runtime_error("Unable to play background music! SDL_mixer Error: " + std::string(Mix_GetError()));
        }
    }

    // Clean up SDL_mixer
    static void cleanup() {
        for (auto& pair : soundMap) {
            if (pair.second) {
                Mix_FreeChunk(pair.second);
            }
        }
        soundMap.clear();
        // Free background music
        if (backgroundMusic)
            Mix_FreeMusic(backgroundMusic);
        Mix_Quit();
    }
};

std::map<std::string, Mix_Chunk*> SoundPlayer::soundMap;
Mix_Music* SoundPlayer::backgroundMusic;
