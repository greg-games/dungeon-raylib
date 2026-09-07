#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"
#include <string>
#include <unordered_map>
#include <vector>

// Screen dimensions
constexpr int TILE_SIZE = 120;
constexpr int HEIGHT = 5 * TILE_SIZE;
constexpr int SCENE_WIDTH = 9 * TILE_SIZE;
constexpr int UI_BAR_WIDTH = 360;
constexpr int WIDTH = UI_BAR_WIDTH + SCENE_WIDTH + UI_BAR_WIDTH;

// Directions
constexpr int WEST = 0;
constexpr int NORTH = 1;
constexpr int EAST = 2;
constexpr int SOUTH = 3;

constexpr int LEFT = -1;
constexpr int RIGHT = 1;

// Entity states
constexpr const char* STATE_IDLE = "idle";
constexpr const char* STATE_RUNNING = "running";
constexpr const char* STATE_JUMP = "jump";
constexpr const char* STATE_DUCK = "duck";
constexpr const char* STATE_ATTACK1 = "attack1";
constexpr const char* STATE_ATTACK2 = "attack2";
constexpr const char* STATE_HIT = "hit";
constexpr const char* STATE_DIE = "die";

// Sound states
constexpr int SOUND_NOT_PLAYING = -1;
constexpr int SOUND_WILL_BE_PLAYED = 0;
constexpr int SOUND_IS_PLAYING = 1;

// Tile variants
constexpr int NO_VARIANT = 0;

// Game constants
constexpr float ENTITY_FRAME_SPEED = 0.009f;
constexpr float TILE_FRAME_SPEED = 0.01f;

// Colors
const Color UI_BAR_COLOR = {50, 22, 15, 255};
const Color WHITE_COLOR = {255, 255, 255, 255};
const Color YELLOW_COLOR = {255, 255, 0, 255};
const Color GREEN_COLOR = {0, 255, 0, 255};
const Color RED_COLOR = {255, 0, 0, 255};
const Color BLUE_COLOR = {0, 0, 255, 255};

// Asset paths
constexpr const char* ASSETS_PATH = "assets";
constexpr const char* IMAGES_PATH = "assets/images";
constexpr const char* SOUNDS_PATH = "assets/sounds";

#endif // CONSTANTS_H