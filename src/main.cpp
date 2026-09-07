#include "game.h"
#include "addon.h"
#include <cstdlib>
#include <ctime>

int main() {
    // Seed random
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Load all addons
    loadAllAddons();
    
    // Create and run game
    Game game;
    game.init();
    game.run();
    
    // Cleanup
    textureManager.unloadAll();
    soundManager.unloadAll();
    
    for (Addon* addon : allAddons) {
        delete addon;
    }
    allAddons.clear();
    
    return 0;
}