#ifndef HEART_H
#define HEART_H

#include "raylib.h"
#include "entity.h"

class Heart : public Entity {
public:
    float frameSpeed = 0.005f;
    int goalFrame = 9;
    
    Heart();
    void animate(float dt) override;
    void changeState(int state);
};

#endif // HEART_H