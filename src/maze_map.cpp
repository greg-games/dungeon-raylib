#include "maze_map.h"
#include "constants.h"

void Line::draw(int currentRoomNumber) const {
    if (!room->visited) return;
    
    Color color = (index == currentRoomNumber) ? YELLOW_COLOR : WHITE_COLOR;
    
    // Draw room as a small rectangle
    Rectangle rect = {pos.x, pos.y, scale * 2, scale * 2};
    DrawRectangleRec(rect, color);
    
    // Draw exits
    if (room->west()) {
        DrawLineEx({pos.x, pos.y + scale}, {pos.x - scale, pos.y + scale}, 2, color);
    }
    if (room->east()) {
        DrawLineEx({pos.x + scale * 2, pos.y + scale}, {pos.x + scale * 3, pos.y + scale}, 2, color);
    }
    if (room->north()) {
        DrawLineEx({pos.x + scale, pos.y}, {pos.x + scale, pos.y - scale}, 2, color);
    }
    if (room->south()) {
        DrawLineEx({pos.x + scale, pos.y + scale * 2}, {pos.x + scale, pos.y + scale * 3}, 2, color);
    }
}

MazeMap::MazeMap(Maze* maze) {
    background = &textureManager.load("map_background");
    
    scale = std::min((SCENE_WIDTH - 100.0f) / (maze->width - 2.0f/3.0f), 
                     (HEIGHT - 86.0f) / (maze->height - 2.0f/3.0f)) / 3.0f;
    
    float xOffset = (SCENE_WIDTH - 100.0f - (maze->width - 2.0f/3.0f) * scale * 3.0f) / 2.0f + scale/2.0f + UI_BAR_WIDTH + 50;
    float yOffset = (HEIGHT - 86.0f - (maze->height - 2.0f/3.0f) * scale * 3.0f) / 2.0f + scale/2.0f + 43;
    
    for (int i = 0; i < maze->size; i++) {
        Vector2 pos = {
            (i % maze->width) * scale * 3.0f + xOffset,
            (i / maze->width) * scale * 3.0f + yOffset
        };
        lines.emplace_back(maze->rooms[i], i, scale + 1, pos);
    }
}

void MazeMap::draw(Maze* maze, int roomNumber) {
    if (background && background->id > 0) {
        DrawTexture(*background, WIDTH/2 - background->width/2, HEIGHT/2 - background->height/2, WHITE);
    }
    
    for (const auto& line : lines) {
        if (maze->rooms[line.index]->visited) {
            line.draw(roomNumber);
        }
    }
}