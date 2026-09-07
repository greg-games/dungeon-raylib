#ifndef LOOT_H
#define LOOT_H

#include "raylib.h"
#include "entity.h"
#include <string>

class Loot : public Entity {
public:
    float raiseHeight = 100;
    int numFrames = 20;
    float step;
    float endY;
    bool finished = false;
    
    Loot(const std::string& name_, const Vector2& pos);
    bool isAnimatingFinished(float dt);
    void draw(float offsetX = 0) override;
};

#endif // LOOT_H