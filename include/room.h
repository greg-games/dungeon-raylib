#ifndef ROOM_H
#define ROOM_H

#include "raylib.h"
#include "constants.h"
#include "tile.h"
#include "entity.h"
#include <vector>
#include <string>
#include <array>

class Chest;

class Room {
public:
    std::array<int, 4> exits = {0, 0, 0, 0}; // W, N, E, S
    int distance = 0;
    std::vector<Tile*> tiles;
    std::vector<std::string> tileNames;
    int chestsInRange = 0;
    bool isChestInRange = false;
    bool hasClosedChest = false;
    int numSkeletons = 0;
    int lastVisited = 0;
    std::vector<Room*> roomsInRange;
    int northLadderIndex = -1;
    int southLadderIndex = -1;
    std::vector<int> animatedTilesIndexes;
    std::vector<Enemy*> enemies;
    bool visited = false;
    
    Room() = default;
    Room(const std::array<int, 4>& exits_, int distance_, std::vector<Tile*>&& tiles_);
    
    std::string toString() const;
    int numExits() const;
    bool west() const { return exits[WEST] == 1; }
    bool north() const { return exits[NORTH] == 1; }
    bool east() const { return exits[EAST] == 1; }
    bool south() const { return exits[SOUTH] == 1; }
    bool isDeadEnd() const { return numExits() == 1; }
    void setDistance(int d) { distance = d; }
    int chestType(int floor) const;
    bool chestIsAllowed(int maxAllowedDistance) const;
    bool skeletonIsAllowed() const;
    AnimatedTile* animatedTile(int i) const;
    void addTile(Tile* tile);
    void addChest(int floor);
    void clearEnemies();
    ~Room();
};

#endif // ROOM_H
