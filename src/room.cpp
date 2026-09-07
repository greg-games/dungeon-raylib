#include "room.h"
#include "entity.h"
#include "constants.h"
#include <cstdlib>
#include <algorithm>

extern const std::unordered_map<std::array<int, 4>, std::string, std::function<size_t(const std::array<int, 4>&)>> SYMBOLS_MAP;

Room::Room(const std::array<int, 4>& exits_, int distance_, std::vector<Tile*>&& tiles_)
    : exits(exits_), distance(distance_), tiles(std::move(tiles_)) {
    
    for (Tile* tile : tiles) {
        tileNames.push_back(tile->name);
    }
}

std::string Room::toString() const {
    // This is a simplified version - in the original, SYMBOLS maps tuples to unicode chars
    // We'll just return a string representation
    return std::to_string(exits[0]) + std::to_string(exits[1]) + 
           std::to_string(exits[2]) + std::to_string(exits[3]);
}

int Room::numExits() const {
    return exits[0] + exits[1] + exits[2] + exits[3];
}

int Room::chestType(int floor) const {
    int chestType;
    if (floor <= 4) {
        chestType = std::min(distance / 7, 2);
    } else {
        chestType = std::min(distance / 7, 3);
    }
    if (numExits() > 1) {
        chestType -= 1;
    }
    return std::max(0, chestType);
}

bool Room::chestIsAllowed(int maxAllowedDistance) const {
    return (distance > 7 && !isChestInRange && distance * 2 < maxAllowedDistance);
}

bool Room::skeletonIsAllowed() const {
    return (distance != 0 && numSkeletons == 0 && lastVisited > 7);
}

AnimatedTile* Room::animatedTile(int i) const {
    if (i >= 0 && i < (int)animatedTilesIndexes.size()) {
        return dynamic_cast<AnimatedTile*>(tiles[animatedTilesIndexes[i]]);
    }
    return nullptr;
}

void Room::addTile(Tile* tile) {
    tiles.push_back(tile);
    tileNames.push_back(tile->name);
    
    if (dynamic_cast<AnimatedTile*>(tile)) {
        animatedTilesIndexes.push_back(tiles.size() - 1);
    }
}

void Room::addChest(int floor) {
    float x = SCENE_WIDTH * (rand() % 2 * 2 + 1) / 4.0f;
    
    if (isDeadEnd()) {
        if (east()) {
            x = SCENE_WIDTH / 4.0f;
        } else if (west()) {
            x = SCENE_WIDTH * 3.0f / 4.0f;
        }
    } else if (!north() && !south()) {
        x = SCENE_WIDTH / 2.0f;
    }
    
    Chest* chest = new Chest("chest", chestType(floor), x);
    addTile(chest);
    hasClosedChest = true;
}

void Room::clearEnemies() {
    for (Enemy* enemy : enemies) {
        auto it = std::find(allEntities.begin(), allEntities.end(), enemy);
        if (it != allEntities.end()) {
            allEntities.erase(it);
        }
        delete enemy;
    }
    enemies.clear();
    numSkeletons = 0;
}

Room::~Room() {
    for (Tile* tile : tiles) {
        delete tile;
    }
    clearEnemies();
}