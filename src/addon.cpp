#include "addon.h"
#include "constants.h"
#include "asset_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

std::vector<Addon*> allAddons;

// Simple expression evaluator for the properties
int evalExpression(const std::string& expr, int width, int height) {
    // Replace constants
    std::string e = expr;
    
    // Replace SCENE_WIDTH, HEIGHT, UI_BAR_WIDTH, WIDTH, TILE_SIZE
    std::regex re_scene_width("SCENE_WIDTH");
    e = std::regex_replace(e, re_scene_width, std::to_string(SCENE_WIDTH));
    
    std::regex re_height("HEIGHT");
    e = std::regex_replace(e, re_height, std::to_string(HEIGHT));
    
    std::regex re_ui_bar("UI_BAR_WIDTH");
    e = std::regex_replace(e, re_ui_bar, std::to_string(UI_BAR_WIDTH));
    
    std::regex re_width("WIDTH");
    e = std::regex_replace(e, re_width, std::to_string(WIDTH));
    
    std::regex re_tile_size("TILE_SIZE");
    e = std::regex_replace(e, re_tile_size, std::to_string(TILE_SIZE));
    
    // Replace width/height with actual texture dimensions (approximate)
    std::regex re_width_var("\\bwidth\\b");
    e = std::regex_replace(e, re_width_var, std::to_string(width));
    
    std::regex re_height_var("\\bheight\\b");
    e = std::regex_replace(e, re_height_var, std::to_string(height));
    
    // Simple evaluation for basic arithmetic
    try {
        int result = 0;
        int currentNum = 0;
        char op = '+';
        
        for (size_t i = 0; i <= e.length(); i++) {
            char c = (i < e.length()) ? e[i] : '+';
            
            if (c >= '0' && c <= '9') {
                currentNum = currentNum * 10 + (c - '0');
            } else if (c == '+' || c == '-' || c == '*' || c == '/' || i == e.length()) {
                if (op == '+') result += currentNum;
                else if (op == '-') result -= currentNum;
                else if (op == '*') result *= currentNum;
                else if (op == '/') result /= currentNum;
                
                op = c;
                currentNum = 0;
            }
        }
        return result;
    } catch (...) {
        return 0;
    }
}

Addon::Addon(const std::string& name_) : name(name_) {
    std::string path = std::string(IMAGES_PATH) + "/tiles/addons/" + name;
    
    if (std::filesystem::exists(path)) {
        numberOfVariants = 0;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory() && entry.path().filename().string().rfind("variant_", 0) == 0) {
                numberOfVariants++;
            }
        }
        numberOfVariants = std::max(0, numberOfVariants - 1); // Exclude properties.txt
        
        std::string variantPath = path + "/variant_0";
        if (std::filesystem::exists(variantPath) && std::filesystem::is_directory(variantPath)) {
            isAnim = true;
        }
        
        loadProperties();
    }
}

void Addon::loadProperties() {
    std::string propPath = std::string(IMAGES_PATH) + "/tiles/addons/" + name + "/properties.txt";
    std::ifstream file(propPath);
    
    if (!file.is_open()) return;
    
    // Get texture dimensions for width/height evaluation
    // Use defaults since texture manager may not be ready yet
    int texWidth = 32, texHeight = 32;
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string prop = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            prop.erase(0, prop.find_first_not_of(" \t"));
            prop.erase(prop.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Remove quotes
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            if (prop == "chance") chance = std::stof(value);
            else if (prop == "max_number_on_screan") maxNumberOnScreen = std::stoi(value);
            else if (prop == "min_distance") minDistance = evalExpression(value, texWidth, texHeight);
            else if (prop == "x_start") xStart = evalExpression(value, texWidth, texHeight);
            else if (prop == "x_end") xEnd = evalExpression(value, texWidth, texHeight);
            else if (prop == "y_start") yStart = evalExpression(value, texWidth, texHeight);
            else if (prop == "y_end") yEnd = evalExpression(value, texWidth, texHeight);
            else if (prop == "can_collide") canCollide = value;
        }
    }
}

void loadAllAddons() {
    std::string addonsPath = std::string(IMAGES_PATH) + "/tiles/addons";
    
    if (!std::filesystem::exists(addonsPath)) return;
    
    for (const auto& entry : std::filesystem::directory_iterator(addonsPath)) {
        if (entry.is_directory()) {
            allAddons.push_back(new Addon(entry.path().filename().string()));
        }
    }
}