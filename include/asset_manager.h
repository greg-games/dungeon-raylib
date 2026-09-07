#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"
#include "constants.h"
#include <string>
#include <unordered_map>
#include <mutex>

class TextureManager {
private:
    std::unordered_map<std::string, Texture2D> textures;
    std::mutex mutex;
    
public:
    TextureManager() = default;
    ~TextureManager() { unloadAll(); }
    
    Texture2D& load(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex);
        std::string fullPath = std::string(IMAGES_PATH) + "/" + path + ".png";
        
        auto it = textures.find(fullPath);
        if (it != textures.end()) {
            return it->second;
        }
        
        Texture2D texture = LoadTexture(fullPath.c_str());
        textures[fullPath] = texture;
        return textures[fullPath];
    }
    
    Texture2D* get(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex);
        std::string fullPath = std::string(IMAGES_PATH) + "/" + path + ".png";
        auto it = textures.find(fullPath);
        return (it != textures.end()) ? &it->second : nullptr;
    }
    
    void unloadAll() {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& pair : textures) {
            UnloadTexture(pair.second);
        }
        textures.clear();
    }
};

class SoundManager {
private:
    std::unordered_map<std::string, Sound> sounds;
    std::mutex mutex;
    
public:
    SoundManager() = default;
    ~SoundManager() { unloadAll(); }
    
    Sound& load(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex);
        std::string fullPath = std::string(SOUNDS_PATH) + "/" + path + ".ogg";
        
        auto it = sounds.find(fullPath);
        if (it != sounds.end()) {
            return it->second;
        }
        
        Sound sound = LoadSound(fullPath.c_str());
        sounds[fullPath] = sound;
        return sounds[fullPath];
    }
    
    void play(const std::string& path, bool loop = false) {
        Sound& sound = load(path);
        PlaySound(sound);
        // Note: Raylib doesn't have built-in looping for sounds
        // Would need custom implementation for looping
    }
    
    void stop(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex);
        std::string fullPath = std::string(SOUNDS_PATH) + "/" + path + ".ogg";
        auto it = sounds.find(fullPath);
        if (it != sounds.end()) {
            StopSound(it->second);
        }
    }
    
    bool isPlaying(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex);
        std::string fullPath = std::string(SOUNDS_PATH) + "/" + path + ".ogg";
        auto it = sounds.find(fullPath);
        return (it != sounds.end()) && IsSoundPlaying(it->second);
    }
    
    void setVolume(const std::string& path, float volume) {
        std::lock_guard<std::mutex> lock(mutex);
        std::string fullPath = std::string(SOUNDS_PATH) + "/" + path + ".ogg";
        auto it = sounds.find(fullPath);
        if (it != sounds.end()) {
            SetSoundVolume(it->second, volume);
        }
    }
    
    void unloadAll() {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& pair : sounds) {
            UnloadSound(pair.second);
        }
        sounds.clear();
    }
};

// Global instances
extern TextureManager textureManager;
extern SoundManager soundManager;

#endif // ASSET_MANAGER_H
