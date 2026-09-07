#ifndef ADDON_H
#define ADDON_H

#include "raylib.h"
#include "tile.h"
#include <string>
#include <vector>
#include <filesystem>

class Addon {
public:
    std::string name;
    int numberOfVariants = 0;
    bool isAnim = false;
    float chance = 0.0f;
    int maxNumberOnScreen = 0;
    int minDistance = 0;
    int xStart = 0, xEnd = 0, yStart = 0, yEnd = 0;
    std::string canCollide = "";
    
    Addon(const std::string& name_);
    void loadProperties();
};

extern std::vector<Addon*> allAddons;

void loadAllAddons();

#endif // ADDON_H