#include "maze.h"
#include "tile.h"
#include <cstdlib>
#include <queue>
#include <algorithm>

Maze::Maze(int width_, int height_, std::vector<Room*>&& rooms_)
    : width(width_), height(height_), size(width_ * height_), rooms(std::move(rooms_)) {
    generateDistance();
    for (int i = 0; i < size; i++) {
        rooms[i]->roomsInRange = roomsInRange(i, 3);
    }
}

Maze::Maze(int width_, int height_) 
    : width(width_), height(height_), size(width_ * height_) {
    generate();
    generateDistance();
    for (int i = 0; i < size; i++) {
        rooms[i]->roomsInRange = roomsInRange(i, 3);
    }
}

Maze::~Maze() {
    for (Room* room : rooms) {
        delete room;
    }
}

std::string Maze::toString() const {
    std::string mazeStr;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            mazeStr += rooms[i * width + j]->toString() + " ";
        }
        mazeStr += "  ||  ";
        if (2 * i < height) {
            for (int j = 0; j < width; j++) {
                mazeStr += rooms[(2 * i) * width + j]->toString();
            }
        }
        mazeStr += "\n";
        for (int j = 0; j < width; j++) mazeStr += "  ";
        mazeStr += "  ||  ";
        if (2 * i + 1 < height) {
            for (int j = 0; j < width; j++) {
                mazeStr += rooms[(2 * i + 1) * width + j]->toString();
            }
        }
        mazeStr += "\n";
    }
    return "\nWidth: " + std::to_string(width) + "\nHeight: " + std::to_string(height) + "\n" + mazeStr;
}

void Maze::generate() {
    rooms.resize(size);
    for (int i = 0; i < size; i++) {
        rooms[i] = new Room({0, 0, 0, 0}, 0, {});
    }
    rooms[0]->exits = {0, 0, 1, 0};
    int prevDir = 2;
    int i = 1;
    generateNextTile(i, prevDir);
    makeLoops();
}

void Maze::generateDistance() {
    distance.assign(size, -1);
    std::queue<std::pair<int, int>> q;
    q.push({0, 0});
    
    while (!q.empty()) {
        auto [idx, d] = q.front();
        q.pop();
        
        if (distance[idx] == -1) {
            distance[idx] = d;
            for (int j = 0; j < 4; j++) {
                if (rooms[idx]->exits[j] == 1) {
                    int nextIdx;
                    if (j % 2 == 0) {
                        nextIdx = idx + j - 1;
                    } else {
                        nextIdx = idx + (j - 2) * width;
                    }
                    if (nextIdx >= 0 && nextIdx < size) {
                        q.push({nextIdx, d + 1});
                    }
                }
            }
        }
    }
}

void Maze::generateNextTile(int i, int prevDir) {
    rooms[i]->exits[(prevDir + 2) % 4] = 1;
    auto available = checkAvailable(i, true);
    
    if (!available.empty()) {
        int idx = rand() % available.size();
        int goTo = available[idx].first;
        int newDir = available[idx].second;
        rooms[i]->exits[newDir] = 1;
        generateNextTile(goTo, newDir);
        generateNextTile(i, prevDir);
    }
}

std::vector<std::pair<int, int>> Maze::checkAvailable(int i, bool empty) const {
    std::vector<std::pair<int, int>> available;
    
    for (int j = 0; j < 4; j++) {
        if (rooms[i]->exits[j] == 0) {
            int nextTile;
            bool inMaze;
            
            if (j % 2 == 0) {
                nextTile = i + j - 1;
                inMaze = (i % width + j - 1 >= 0) && (i % width + j - 1 < width);
            } else {
                nextTile = i + (j - 2) * width;
                inMaze = nextTile < size;
            }
            
            if (inMaze && nextTile > 0 && (!empty || rooms[nextTile]->exits == std::array<int, 4>{0, 0, 0, 0})) {
                available.push_back({nextTile, j});
            }
        }
    }
    return available;
}

void Maze::makeLoops() {
    for (int i = 0; i < size / 10; i++) {
        int x = 1 + rand() % (size - 1);
        auto available = checkAvailable(x, false);
        for (const auto& z : available) {
            if (rand() % 2 == 0) {
                rooms[x]->exits[z.second] = 1;
                rooms[z.first]->exits[(z.second + 2) % 4] = 1;
            }
        }
    }
}

std::vector<Room*> Maze::roomsInRange(int i, int dist) const {
    std::vector<bool> visited(size, false);
    std::vector<Room*> result;
    std::queue<std::pair<int, int>> q;
    q.push({i, 0});
    
    while (!q.empty()) {
        auto [j, d] = q.front();
        q.pop();
        
        if (!visited[j] && d <= dist) {
            visited[j] = true;
            result.push_back(rooms[j]);
            
            for (int k = 0; k < 4; k++) {
                if (rooms[j]->exits[k] == 1) {
                    int nextIdx;
                    if (k % 2 == 0) {
                        nextIdx = j + k - 1;
                    } else {
                        nextIdx = j + (k - 2) * width;
                    }
                    if (nextIdx >= 0 && nextIdx < size) {
                        q.push({nextIdx, d + 1});
                    }
                }
            }
        }
    }
    return result;
}

Chest* Maze::getChestFor(int roomNumber) const {
    Room* room = rooms[roomNumber];
    if (!room->animatedTilesIndexes.empty()) {
        return dynamic_cast<Chest*>(room->tiles[room->animatedTilesIndexes[0]]);
    }
    return nullptr;
}