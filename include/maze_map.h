#ifndef MAZE_MAP_H
#define MAZE_MAP_H

#include "raylib.h"
#include "maze.h"
#include <vector>

class Line {
public:
    Room* room;
    int index;
    float scale;
    Vector2 pos;
    bool visited = false;
    
    Line(Room* room_, int index_, float scale_, const Vector2& pos_)
        : room(room_), index(index_), scale(scale_), pos(pos_) {}
    
    void draw(int currentRoomNumber) const;
};

class MazeMap {
public:
    std::vector<Line> lines;
    float scale;
    Texture2D* background;
    
    MazeMap(Maze* maze);
    void draw(Maze* maze, int roomNumber);
};

#endif // MAZE_MAP_H