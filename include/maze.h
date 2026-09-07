#ifndef MAZE_H
#define MAZE_H

#include "raylib.h"
#include "room.h"
#include <vector>
#include <string>

class Maze {
public:
    int width;
    int height;
    int size;
    std::vector<Room*> rooms;
    std::vector<int> distance;
    
    Maze(int width_, int height_, std::vector<Room*>&& rooms_);
    Maze(int width_, int height_);
    ~Maze();
    
    std::string toString() const;
    std::vector<Room*> roomsInRange(int i, int distance) const;
    Chest* getChestFor(int roomNumber) const;
    
private:
    void generate();
    void generateDistance();
    void generateNextTile(int i, int prevDir);
    std::vector<std::pair<int, int>> checkAvailable(int i, bool empty) const;
    void makeLoops();
};

#endif // MAZE_H