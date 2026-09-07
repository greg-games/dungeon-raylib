#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"
#include "utils.h"
#include "asset_manager.h"
#include <string>
#include <functional>
#include <vector>

extern std::vector<class Button*> allButtons;
extern Rect mouseHitbox;

enum ButtonMode {
    BUTTON_UNPRESSED,
    BUTTON_PRESSED
};

class Button {
public:
    std::string name;
    std::function<void()> onClick;
    std::function<void()> onRelease;
    std::function<void()> onPress;
    std::function<void()> onUnpress;
    
    ButtonMode mode = BUTTON_UNPRESSED;
    bool canInteract = true;
    bool isVisible = true;
    
    Texture2D* texture = nullptr;
    Texture2D* backgroundTex = nullptr;
    Texture2D* disabledTex = nullptr;
    Rect hitbox;
    
    Button(const std::string& name_,
           std::function<void()> onClick_ = nullptr,
           std::function<void()> onRelease_ = nullptr,
           std::function<void()> onPress_ = nullptr,
           std::function<void()> onUnpress_ = nullptr);
    
    void setPos(const Vector2& pos);
    void onReleaseEvent(const Rect& mouse);
    void onClickEvent(const Rect& mouse);
    void onUnpressEvent();
    void onPressEvent();
    void disable();
    void enable();
    void hide();
    void show();
    void draw() const;
    void update(const Rect& mouse);
};

void buttonsOn(const std::string& event, const Rect& mouse);

#endif // BUTTON_H