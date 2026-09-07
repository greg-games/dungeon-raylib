#include "loot.h"
#include "asset_manager.h"

Loot::Loot(const std::string& name_, const Vector2& pos)
    : Entity(name_, "", 0, 1) { // Loot doesn't need running speed or health really
    this->name = name_;
    hitbox = Rect(pos.x, pos.y, 0, 0);
    currentTexturePath = "loot/" + name;
    currentTexture = &textureManager.load(currentTexturePath);
    
    if (currentTexture) {
        hitbox.width = currentTexture->width;
        hitbox.height = currentTexture->height;
    }
    
    step = raiseHeight / numFrames;
    endY = pos.y - raiseHeight;
    frame = 0;
    state = "idle";
}

bool Loot::isAnimatingFinished(float dt) {
    if (currentTexture) {
        // Fade in
        // Note: Raylib doesn't have per-texture alpha easily, we'd need to use DrawTexturePro with tint
    }
    
    if (hitbox.y > endY) {
        hitbox.y -= step * dt / 10.0f;
        return false;
    }
    return true;
}

void Loot::draw(float offsetX) {
    if (!currentTexture) return;
    
    // Calculate alpha based on animation progress
    float progress = 1.0f - (hitbox.y - endY) / raiseHeight;
    unsigned char alpha = (unsigned char)(255 * progress);
    Color tint = {255, 255, 255, alpha};
    
    Rectangle source = {0, 0, (float)currentTexture->width, (float)currentTexture->height};
    Rectangle dest = {hitbox.x + offsetX, hitbox.y, (float)currentTexture->width, (float)currentTexture->height};
    DrawTexturePro(*currentTexture, source, dest, {0, 0}, 0.0f, tint);
}