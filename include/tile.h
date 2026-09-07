#ifndef TILE_H
#define TILE_H

#include "raylib.h"
#include "constants.h"
#include "utils.h"
#include "asset_manager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

class Tile {
public:
    std::string name;
    int variant;
    Vector2 pos;
    
    Tile(const std::string& name_, int variant_, const Vector2& pos_) 
        : name(name_), variant(variant_), pos(pos_) {}
    virtual ~Tile() = default;
    
    virtual std::string imagePath() const;
    virtual void draw(float offsetX = 0) const;
    virtual Rect getRect() const;
};

class TileAddon : public Tile {
public:
    TileAddon(const std::string& name_, int variant_, const Vector2& pos_)
        : Tile(name_, variant_, pos_) {}
    
    std::string imagePath() const override;
};

class AnimatedTile : public Tile {
public:
    std::string state;
    float frame = 0;
    std::unordered_map<std::string, int> numFrames;
    std::string trigger;
    bool isAnimating = false;
    bool hasFinishedAnimating = false;
    
    AnimatedTile(const std::string& name_, int variant_, const std::string& state_, const Vector2& pos_);
    
    void loadFrames();
    virtual std::string imagePath() const override;
    void nextFrame(float frameSpeed);
    virtual void draw(float offsetX = 0) const override;
    virtual Rect getRect() const override;
    virtual std::string soundPath() const;
};

class AnimatedTileAddon : public AnimatedTile {
public:
    AnimatedTileAddon(const std::string& name_, int variant_, const std::string& state_, const Vector2& pos_);
    void loadFrames() override;
    std::string imagePath() const override;
};

extern const std::vector<std::unordered_map<std::string, std::pair<int, int>>> CHEST_LOOT_TABLES;

class Chest : public AnimatedTile {
public:
    int chestVariant;
    std::unordered_map<std::string, std::pair<int, int>> lootTable;
    float bottom;
    
    Chest(const std::string& name_, int variant_, float x);
    std::string soundPath() const override;
};

class Door : public AnimatedTile {
public:
    Door(const std::string& name_, int variant_, const std::string& state_, const Vector2& pos_);
    std::string soundPath() const override;
};

class Spikes : public AnimatedTile {
public:
    Spikes(const std::string& name_, int variant_, const Vector2& pos_);
};

#endif // TILE_H