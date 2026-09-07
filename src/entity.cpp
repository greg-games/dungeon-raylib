#include "entity.h"
#include <filesystem>
#include <iostream>

std::vector<Entity*> allEntities;
std::vector<Entity*> allSprites;

void updateAllSprites(std::vector<Entity*>& newAllSprites) {
    allSprites = newAllSprites;
}

Entity::Entity(const std::string& name_, const std::string& variant_, 
               float runningSpeed_, int health_,
               float hitboxWidth, float hitboxHeight, float hitboxOffset_,
               float attackHitboxWidth, float attackHitboxHeight)
    : name(name_), variant(variant_), runningSpeed(runningSpeed_), health(health_),
      hitboxOffset(hitboxOffset_) {
    
    loadAnimations();
    
    // Initialize hitboxes
    hitbox = Rect(0, 0, 0, 0);
    attackHitbox = Rect(0, 0, 0, 0);
    
    // Set initial frame
    frame = static_cast<float>(rand() % numFrames[STATE_IDLE]);
    
    allEntities.push_back(this);
}

void Entity::loadAnimations() {
    std::string basePath = std::string(IMAGES_PATH) + "/entities/" + name + "/" + variant;
    
    if (!std::filesystem::exists(basePath)) {
        std::cerr << "Warning: Entity path not found: " << basePath << std::endl;
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
            frameSpeed[animName] = static_cast<int>(ENTITY_FRAME_SPEED * 1000); // Convert to int for easier handling
            
            // Initialize sound states
            std::string soundPath = std::string(SOUNDS_PATH) + "/" + name;
            if (std::filesystem::exists(soundPath)) {
                for (const auto& soundEntry : std::filesystem::directory_iterator(soundPath)) {
                    if (soundEntry.is_regular_file() && soundEntry.path().extension() == ".ogg") {
                        std::string soundName = soundEntry.path().stem().string();
                        soundStates[soundName] = SOUND_NOT_PLAYING;
                    }
                }
            }
        }
    }
}

void Entity::animate(float dt) {
    int frameSpeedVal = frameSpeed[state];
    if (frameSpeedVal == 0) frameSpeedVal = static_cast<int>(ENTITY_FRAME_SPEED * 1000);
    
    frame += frameSpeedVal * dt / 1000.0f;
    
    int maxFrame = numFrames[state] - 1;
    if (frame > maxFrame) {
        frame = 0;
        if (state == STATE_DIE) {
            isDead = true;
            auto it = std::find(allEntities.begin(), allEntities.end(), this);
            if (it != allEntities.end()) {
                allEntities.erase(it);
            }
            return;
        }
    }
    
    currentTexturePath = getCurrentTexturePath();
    currentTexture = &textureManager.load(currentTexturePath);
    
    // Note: For flipping, we'll handle it in draw()
}

void Entity::updateHitboxes() {
    if (currentTexture) {
        hitbox.width = currentTexture->width * (hitbox.width > 0 ? hitbox.width / currentTexture->width : 1.0f);
        hitbox.height = currentTexture->height * (hitbox.height > 0 ? hitbox.height / currentTexture->height : 1.0f);
    }
    
    hitbox.x = x - hitbox.width/2 + hitboxOffset * dir;
    hitbox.y = y - hitbox.height/2;
    
    attackHitbox = hitbox;
    attackHitbox.width *= 1.0f; // Will be overridden by derived classes
    attackHitbox.height *= 1.0f;
}

void Entity::changeState(const std::string& newState, int dir_, int damage) {
    if (dir_ != 0) dir = dir_;
    
    if (state != newState) {
        frame = 0;
        state = newState;
        updateHitboxes();
        toggleSound("running", {STATE_RUNNING});
        
        if (newState == STATE_HIT) {
            health -= damage;
            if (health <= 0) {
                changeState(STATE_DIE);
            }
        }
    }
}

void Entity::toggleSound(const std::string& soundName, const std::vector<std::string>& states) {
    auto it = soundStates.find(soundName);
    if (it != soundStates.end()) {
        bool shouldPlay = std::find(states.begin(), states.end(), state) != states.end();
        if (shouldPlay && it->second == SOUND_NOT_PLAYING) {
            it->second = SOUND_WILL_BE_PLAYED;
        } else if (!shouldPlay) {
            it->second = SOUND_NOT_PLAYING;
        }
    }
}

void Entity::changeX(float x_) {
    // This will be set via position in draw
    // The actual position is handled by the sprite system
}

void Entity::updateColliding() {
    colliding.clear();
    attacking.clear();
    
    for (Entity* sprite : allSprites) {
        if (sprite->name != "background" && sprite != this) {
            if (isColliding(hitbox, sprite->hitbox, 0)) {
                colliding.push_back(sprite);
            }
            if (isColliding(attackHitbox, sprite->hitbox, 0)) {
                attacking.push_back(sprite);
            }
        }
    }
}

bool Entity::isLookingAt(Entity* object) {
    return (object->hitbox.x - hitbox.x) * dir >= 0;
}

void Entity::move() {
    changeX(hitbox.x + speed * dir);
    if (speed == 0) {
        changeState(STATE_IDLE);
    } else {
        changeState(STATE_RUNNING, dir);
    }
}

std::string Entity::soundPath(const std::string& sound) {
    return name + "/" + sound;
}

std::string Entity::getCurrentTexturePath() const {
    int frameInt = static_cast<int>(frame);
    return "entities/" + name + "/" + variant + "/" + state + "/" + std::to_string(frameInt);
}

void Entity::draw(float offsetX) {
    if (!currentTexture) return;
    
    Rectangle source = {0, 0, (float)currentTexture->width, (float)currentTexture->height};
    if (dir == LEFT) {
        source.width = -source.width; // Flip horizontally
    }
    
    Rectangle dest = {
        hitbox.x + hitbox.width/2 + offsetX,
        hitbox.y + hitbox.height/2,
        (float)currentTexture->width,
        (float)currentTexture->height
    };
    
    Vector2 origin = {(float)currentTexture->width/2, (float)currentTexture->height/2};
    DrawTexturePro(*currentTexture, source, dest, origin, 0.0f, WHITE);
}

void Entity::drawHitboxes() const {
    DrawRectangleLinesEx(hitbox.toRaylib(), 2, GREEN);
    DrawRectangleLinesEx(attackHitbox.toRaylib(), 2, RED);
}

// Player implementation
Player::Player() 
    : Entity("player", "variant_0", 0.4f, 5, 0.2f, 1.0f, -0.2f) {
    
    // Override frame speeds for specific animations
    frameSpeed["duck"] = static_cast<int>(0.4f * 1000);
    frameSpeed["idle"] = static_cast<int>(0.6f * 1000);
    frameSpeed["running"] = static_cast<int>(1.2f * 1000);
    frameSpeed["jump"] = static_cast<int>(1.1f * 1000);
    frameSpeed["hit"] = static_cast<int>(0.073f * 1000);
}

void Player::updateHitboxes() {
    Entity::updateHitboxes();
    
    if (state == STATE_JUMP) {
        hitbox.height = hitbox.height * 3.0f / 5.0f;
        hitbox.y = hitbox.y; // top stays same
    } else if (state == STATE_DUCK) {
        hitbox.height = hitbox.height / 3.0f;
        hitbox.y = hitbox.y + hitbox.height * 2.0f / 3.0f; // bottom stays same
    } else {
        hitbox.height = hitbox.height; // original height
    }
}

void Player::move() {
    updateHitboxes();
    
    if (state == STATE_IDLE || state == STATE_RUNNING) {
        Entity::move();
    } else if (state == STATE_JUMP) {
        changeX(hitbox.x + speed * dir);
        changeState(STATE_JUMP, dir);
    }
}

void Player::animate(float dt) {
    int maxFrame = numFrames[state] - 1;
    if (frame + frameSpeed[state] * dt / 1000.0f > maxFrame) {
        if (state == STATE_DIE) {
            isDead = true;
            return;
        }
        if (speed == 0) {
            changeState(STATE_IDLE);
        } else {
            changeState(STATE_RUNNING, dir);
        }
    }
    Entity::animate(dt);
    updateColliding();
    
    // Attack logic
    if (state == STATE_ATTACK1 && static_cast<int>(frame) >= maxFrame - 2) {
        for (Entity* obj : attacking) {
            if (obj->name == "skeleton") {
                obj->changeState(STATE_HIT);
            }
        }
    }
}

// Enemy implementation
Enemy::Enemy(const std::string& name_, const std::string& variant_, int difficulty)
    : Entity(name_, variant_, 0.2f, (difficulty < 4) ? (rand() % 2 + 1) : (rand() % 2 + 2),
             0.5f, 1.0f, -1.0f/3.0f, 0.8f, 1.0f) {
    
    // Override frame speeds
    frameSpeed["idle"] = static_cast<int>(1.1f * 1000);
    frameSpeed["attack1"] = static_cast<int>(0.9f * 1000);
    frameSpeed["attack2"] = static_cast<int>(0.92f * 1000);
    frameSpeed["hit"] = static_cast<int>(0.75f * 1000);
    frameSpeed["die"] = static_cast<int>(1.4f * 1000);
}

void Enemy::goToPlayer(float playerX, float dt) {
    speed = runningSpeed * dt;
    if (hitbox.x > playerX) {
        dir = LEFT;
    } else {
        dir = RIGHT;
    }
}

void Enemy::updateHitboxes() {
    Entity::updateHitboxes();
    
    if (state == STATE_ATTACK1 || state == STATE_ATTACK2) {
        if (state == STATE_ATTACK1) {
            attackHitbox.height = hitbox.height / 3.0f;
            attackHitbox.y = hitbox.y + hitbox.height / 6.0f;
        } else {
            attackHitbox.height = hitbox.height / 3.0f;
            attackHitbox.y = hitbox.y + hitbox.height * 2.0f / 3.0f - attackHitbox.height;
        }
    } else {
        attackHitbox.height = hitbox.height;
        attackHitbox.y = hitbox.y;
    }
}

void Enemy::animate(float dt) {
    Entity::animate(dt);
    updateColliding();
    
    int maxFrame = numFrames[state] - 1;
    if ((state == STATE_ATTACK1 || state == STATE_ATTACK2) && 
        static_cast<int>(frame) == maxFrame - 3) {
        for (Entity* obj : attacking) {
            if (obj->name == "player") {
                obj->changeState(STATE_HIT);
                changeState(STATE_IDLE);
                Entity::animate(dt);
                break;
            }
        }
    }
}

void Enemy::attack() {
    if (state == STATE_RUNNING || frame == 0) {
        if (attackProgress % (numAttacks + 1) == 0) {
            changeState(STATE_IDLE);
        } else if (rand() % 2 == 0) {
            changeState(STATE_ATTACK1);
        } else {
            changeState(STATE_ATTACK2);
        }
        attackProgress++;
        speed = 0;
        updateColliding();
    }
}