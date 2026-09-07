#include "tile.h"
#include <filesystem>
#include <iostream>

const std::vector<std::unordered_map<std::string, std::pair<int, int>>> CHEST_LOOT_TABLES = {
    {{"coin", {3, 5}}},
    {{"crystal_red", {-1, 2}}, {"coin", {4, 6}}},
    {{"crystal_green", {0, 3}}, {"crystal_red", {1, 3}}, {"coin", {5, 7}}},
    {{"crystal_violet", {1, 2}}, {"crystal_green", {1, 3}}, {"crystal_red", {2, 4}}, {"coin", {6, 8}}},
};

const std::unordered_map<std::string, std::string> TRIGGERS = {
    {"chest", "click"},
    {"door", "click"},
    {"spikes", "always"}
};

std::string Tile::imagePath() const {
    return "tiles/" + name + "/variant_" + std::to_string(variant);
}

void Tile::draw(float offsetX) const {
    Texture2D* texture = &textureManager.load(imagePath());
    if (texture->id == 0) return;
    
    Rectangle source = {0, 0, (float)texture->width, (float)texture->height};
    Rectangle dest = {pos.x + offsetX, pos.y, (float)texture->width, (float)texture->height};
    DrawTexturePro(*texture, source, dest, {0, 0}, 0.0f, WHITE);
}

Rect Tile::getRect() const {
    Texture2D* texture = textureManager.get(imagePath());
    if (texture) {
        return Rect(pos.x, pos.y, (float)texture->width, (float)texture->height);
    }
    return Rect(pos.x, pos.y, 0, 0);
}

std::string TileAddon::imagePath() const {
    return "tiles/addons/" + name + "/variant_" + std::to_string(variant);
}

AnimatedTile::AnimatedTile(const std::string& name_, int variant_, const std::string& state_, const Vector2& pos_)
    : Tile(name_, variant_, pos_), state(state_) {
    
    auto it = TRIGGERS.find(name_);
    trigger = (it != TRIGGERS.end()) ? it->second : "click";
    isAnimating = (trigger == "always");
    hasFinishedAnimating = false;
    loadFrames();
}

void AnimatedTile::loadFrames() {
    std::string basePath = std::string(IMAGES_PATH) + "/tiles/" + name + "/variant_" + std::to_string(variant);
    
    if (!std::filesystem::exists(basePath)) {
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
        if (entry.is_directory()) {
            std::string animName = entry.path().filename().string();
            int frameCount = 0;
            for (const auto& frameEntry : std::filesystem::directory_iterator(entry.path())) {
                if (frameEntry.is_regular_file() && frameEntry.path().extension() == ".png") {
                    frameCount++;
                }
            }
            numFrames[animName] = frameCount;
        }
    }
}

std::string AnimatedTile::imagePath() const {
    int frameInt = static_cast<int>(frame);
    return "tiles/" + name + "/variant_" + std::to_string(variant) + "/" + state + "/" + std::to_string(frameInt);
}

void AnimatedTile::nextFrame(float frameSpeed) {
    if (isAnimating) {
        int maxFrame = numFrames[state] - 1;
        if (frame + frameSpeed < maxFrame) {
            frame += frameSpeed;
        } else if (trigger == "always") {
            frame = 0;
        } else {
            hasFinishedAnimating = true;
        }
    }
}

void AnimatedTile::draw(float offsetX) const {
    Texture2D* texture = &textureManager.load(imagePath());
    if (texture->id == 0) return;
    
    Rectangle source = {0, 0, (float)texture->width, (float)texture->height};
    Rectangle dest = {pos.x + offsetX, pos.y, (float)texture->width, (float)texture->height};
    DrawTexturePro(*texture, source, dest, {0, 0}, 0.0f, WHITE);
}

Rect AnimatedTile::getRect() const {
    Texture2D* texture = textureManager.get(imagePath());
    if (texture) {
        return Rect(pos.x, pos.y, (float)texture->width, (float)texture->height);
    }
    return Rect(pos.x, pos.y, 0, 0);
}

std::string AnimatedTile::soundPath() const {
    return name + "/" + std::to_string(variant);
}

AnimatedTileAddon::AnimatedTileAddon(const std::string& name_, int variant_, const std::string& state_, const Vector2& pos_)
    : AnimatedTile(name_, variant_, state_, pos_) {}

void AnimatedTileAddon::loadFrames() {
    std::string basePath = std::string(IMAGES_PATH) + "/tiles/addons/" + name + "/variant_" + std::to_string(variant);
    
    if (!std::filesystem::exists(basePath)) {
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
        if (entry.is_directory()) {
            std::string animName = entry.path().filename().string();
            int frameCount = 0;
            for (const auto& frameEntry : std::filesystem::directory_iterator(entry.path())) {
                if (frameEntry.is_regular_file() && frameEntry.path().extension() == ".png") {
                    frameCount++;
                }
            }
            numFrames[animName] = frameCount;
        }
    }
}

std::string AnimatedTileAddon::imagePath() const {
    int frameInt = static_cast<int>(frame);
    return "tiles/addons/" + name + "/variant_" + std::to_string(variant) + "/" + state + "/" + std::to_string(frameInt);
}

Chest::Chest(const std::string& name_, int variant_, float x)
    : AnimatedTile(name_, variant_, "opening", {0, 0}), chestVariant(variant_) {
    
    Texture2D* brickTex = &textureManager.load("tiles/brick/variant_0");
    bottom = HEIGHT - brickTex->height;
    
    Texture2D* chestTex = &textureManager.load("tiles/chest/variant_0/opening/0");
    pos = {x, bottom - chestTex->height / 2.0f};
    
    lootTable = CHEST_LOOT_TABLES[std::min(chestVariant, (int)CHEST_LOOT_TABLES.size() - 1)];
}

std::string Chest::soundPath() const {
    return "chest/" + std::to_string(chestVariant);
}

Door::Door(const std::string& name_, int variant_, const std::string& state_, const Vector2& pos_)
    : AnimatedTile(name_, variant_, state_, pos_) {}

std::string Door::soundPath() const {
    return variant ? "door/close" : "door/open";
}

Spikes::Spikes(const std::string& name_, int variant_, const Vector2& pos_)
    : AnimatedTile(name_, variant_, "default", pos_) {
    isAnimating = true;
}
