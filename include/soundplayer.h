#pragma once

#include <SDL2/SDL_mixer.h>
#include <stdexcept>
#include <map>

class SoundPlayer {
private:
    static std::map<std::string, Mix_Chunk*> soundMap;

public:
    // Initialize SDL_mixer
    static void init() {
        // Make sure to call this only once in your program
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
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

    // Clean up SDL_mixer
    static void cleanup() {
        // Make sure to call this only once in your program
        for (auto& pair : soundMap) {
            if (pair.second) {
                Mix_FreeChunk(pair.second);
            }
        }
        soundMap.clear();
        Mix_Quit();
    }
};

std::map<std::string, Mix_Chunk*> SoundPlayer::soundMap;
