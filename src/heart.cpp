#include "heart.h"
#include "asset_manager.h"

Heart::Heart() : Entity("heart", "", 0, 1) {
    name = "heart";
    frame = 9;
    goalFrame = 9;
    currentTexturePath = "ui/heart/9";
    currentTexture = &textureManager.load(currentTexturePath);
    state = "idle";
}

void Heart::animate(float dt) {
    if (static_cast<int>(frame) != goalFrame) {
        if (frame > goalFrame) {
            frame -= frameSpeed * dt;
            if (frame < goalFrame) frame = goalFrame;
        } else {
            frame += frameSpeed * dt;
            if (frame > goalFrame) frame = goalFrame;
        }
        currentTexturePath = "ui/heart/" + std::to_string(static_cast<int>(frame));
        currentTexture = &textureManager.load(currentTexturePath);
    }
}

void Heart::changeState(int state) {
    goalFrame = state * 3;
}