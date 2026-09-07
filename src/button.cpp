#include "button.h"
#include "asset_manager.h"
#include <algorithm>

std::vector<Button*> allButtons;
Rect mouseHitbox(0, 0, 2, 2);

Button::Button(const std::string& name_,
               std::function<void()> onClick_,
               std::function<void()> onRelease_,
               std::function<void()> onPress_,
               std::function<void()> onUnpress_)
    : name(name_), onClick(onClick_), onRelease(onRelease_), onPress(onPress_), onUnpress(onUnpress_) {
    
    texture = &textureManager.load("ui/buttons/" + name);
    backgroundTex = &textureManager.load("ui/buttons/" + name);
    disabledTex = &textureManager.load("ui/buttons/" + name + "_disabled");
    
    if (texture->id > 0) {
        hitbox = Rect(0, 0, (float)texture->width, (float)texture->height);
    }
    
    allButtons.push_back(this);
}

void Button::setPos(const Vector2& pos) {
    hitbox.setCenter(pos);
}

void Button::onReleaseEvent(const Rect& mouse) {
    if (isColliding(mouse, hitbox) && canInteract && mode != BUTTON_UNPRESSED) {
        mode = BUTTON_UNPRESSED;
        if (onRelease) onRelease();
    }
}

void Button::onClickEvent(const Rect& mouse) {
    if (isColliding(mouse, hitbox) && canInteract && mode != BUTTON_PRESSED) {
        mode = BUTTON_PRESSED;
        if (onClick) onClick();
    }
}

void Button::onUnpressEvent() {
    if (mode == BUTTON_UNPRESSED && onUnpress && canInteract) {
        onUnpress();
    }
}

void Button::onPressEvent() {
    if (mode == BUTTON_PRESSED && onPress && canInteract) {
        onPress();
    }
}

void Button::disable() {
    canInteract = false;
}

void Button::enable() {
    canInteract = true;
}

void Button::hide() {
    isVisible = false;
    canInteract = false;
}

void Button::show() {
    isVisible = true;
    canInteract = true;
}

void Button::draw() const {
    if (!isVisible || texture->id == 0) return;
    
    Texture2D* tex = texture;
    if (!canInteract && disabledTex->id > 0) {
        tex = disabledTex;
    }
    
    Rectangle source = {0, 0, (float)tex->width, (float)tex->height};
    Rectangle dest = {hitbox.x, hitbox.y, (float)tex->width, (float)tex->height};
    
    // Apply alpha for pressed state
    Color tint = WHITE;
    if (mode == BUTTON_PRESSED) {
        tint.a = 127;
    }
    
    DrawTexturePro(*tex, source, dest, {0, 0}, 0.0f, tint);
    
    // Draw background
    if (backgroundTex->id > 0) {
        DrawTexturePro(*backgroundTex, source, dest, {0, 0}, 0.0f, WHITE);
    }
}

void Button::update(const Rect& mouse) {
    // This can be used for hover effects if needed
}

void buttonsOn(const std::string& event, const Rect& mouse) {
    for (Button* btn : allButtons) {
        if (event == "pressed") {
            btn->onPressEvent();
        } else if (event == "unpressed") {
            btn->onUnpressEvent();
        } else if (event == "clicked") {
            btn->onClickEvent(mouse);
        } else if (event == "released") {
            btn->onReleaseEvent(mouse);
        }
    }
}