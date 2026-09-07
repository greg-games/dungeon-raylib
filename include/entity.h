#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "constants.h"
#include "utils.h"
#include "asset_manager.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

extern std::vector<class Entity*> allEntities;
extern std::vector<class Entity*> allSprites;

void updateAllSprites(std::vector<Entity*>& newAllSprites);

class Entity {
public:
    std::string name;
    std::string variant;
    std::unordered_map<std::string, int> frameSpeed;
    std::unordered_map<std::string, int> numFrames;
    std::unordered_map<std::string, int> soundStates;
    
    std::string state = STATE_IDLE;
    int health = 1;
    int dir = RIGHT;
    float speed = 0;
    float runningSpeed = 0;
    bool isDead = false;
    float frame = 0;
    float hitboxOffset = 0;
    
    Rect hitbox;
    Rect attackHitbox;
    std::vector<Entity*> colliding;
    std::vector<Entity*> attacking;
    
    Texture2D* currentTexture = nullptr;
    std::string currentTexturePath;
    
    Entity(const std::string& name_, const std::string& variant_, 
           float runningSpeed_, int health_,
           float hitboxWidth = 1.0f, float hitboxHeight = 1.0f, float hitboxOffset_ = 0,
           float attackHitboxWidth = 1.0f, float attackHitboxHeight = 1.0f);
    
    virtual ~Entity() = default;
    
    virtual void animate(float dt);
    virtual void updateHitboxes();
    virtual void changeState(const std::string& newState, int dir_ = 0, int damage = 1);
    virtual void toggleSound(const std::string& soundName, const std::vector<std::string>& states);
    virtual void changeX(float x);
    virtual void updateColliding();
    virtual bool isLookingAt(Entity* object);
    virtual void move();
    virtual std::string soundPath(const std::string& sound);
    virtual void draw(float offsetX = 0);
    virtual void drawHitboxes() const;
    
    // Get current frame texture path
    std::string getCurrentTexturePath() const;
    
protected:
    void loadAnimations();
    void flipTextureHorizontally();
};

class Player : public Entity {
public:
    bool canMove = true;
    
    Player();
    void updateHitboxes() override;
    void move() override;
    void animate(float dt) override;
};

class Enemy : public Entity {
public:
    int attackProgress = 1;
    int numAttacks = 1;
    
    Enemy(const std::string& name_, const std::string& variant_, int difficulty);
    void goToPlayer(float playerX, float dt);
    void updateHitboxes() override;
    void animate(float dt) override;
    void attack();
};

#endif // ENTITY_H