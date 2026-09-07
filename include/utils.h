#ifndef UTILS_H
#define UTILS_H

#include "raylib.h"
#include "constants.h"

// Extended Rectangle with helper methods
struct Rect {
    float x, y, width, height;
    
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(float x_, float y_, float w_, float h_) : x(x_), y(y_), width(w_), height(h_) {}
    Rect(const Rectangle& r) : x(r.x), y(r.y), width(r.width), height(r.height) {}
    
    float left() const { return x; }
    float right() const { return x + width; }
    float top() const { return y; }
    float bottom() const { return y + height; }
    
    Vector2 center() const { return {x + width/2, y + height/2}; }
    void setCenter(const Vector2& c) { x = c.x - width/2; y = c.y - height/2; }
    
    Vector2 topleft() const { return {x, y}; }
    void setTopleft(const Vector2& t) { x = t.x; y = t.y; }
    
    Rectangle toRaylib() const { return {x, y, width, height}; }
    Rect copy() const { return *this; }
};

inline bool isColliding(const Rect& a, const Rect& b, float distance = 0) {
    return (a.right() + distance > b.left() &&
            a.left() - distance < b.right() &&
            a.top() - distance < b.bottom() &&
            a.bottom() + distance > b.top());
}

inline bool checkCollisionRecs(const Rect& a, const Rect& b) {
    return CheckCollisionRecs(a.toRaylib(), b.toRaylib());
}

// Helper to load texture from path
Texture2D LoadTextureSafe(const char* path);

// Helper to get file list in directory
std::vector<std::string> GetDirectoryFiles(const char* dirPath);

// Helper to check if directory exists
bool DirectoryExists(const char* dirPath);

#endif // UTILS_H