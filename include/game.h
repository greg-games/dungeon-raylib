#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "entity.h"
#include "maze.h"
#include "maze_map.h"
#include "button.h"
#include "loot.h"
#include "heart.h"
#include "addon.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <array>

enum GameState {
    STATE_TITLE,
    STATE_PLAYING,
    STATE_GAME_OVER
};

class Game {
public:
    GameState state = STATE_TITLE;
    bool gameEnded = true;
    float dt = 1.0f;
    
    // Player and entities
    Player* player = nullptr;
    Maze* maze = nullptr;
    MazeMap* mazeMap = nullptr;
    Room* currentRoom = nullptr;
    
    // UI
    std::vector<Loot*> allLoot;
    std::vector<Heart*> hearts;
    std::unordered_map<std::string, int> lootCollected = {
        {"crystal_violet", 0},
        {"crystal_green", 0},
        {"crystal_red", 0},
        {"coin", 0}
    };
    std::vector<Entity*> lootIcons;
    Entity* chestIcon = nullptr;
    Rect uiBarLeft;
    Rect uiBarRight;
    Entity* background = nullptr;
    Entity* titleText = nullptr;
    Entity* shop = nullptr;
    
    // Game variables
    int roomNumber = 0;
    int floor = 0;
    int numberOfChests = 0;
    int numberOfFoundChests = 0;
    int numVisitedRooms = 0;
    bool mapOpen = false;
    bool showHitboxes = false;
    
    // Input
    bool goLeft = false;
    bool goRight = false;
    bool goUp = false;
    bool goDown = false;
    
    // Buttons
    Button* playButton = nullptr;
    Button* mapButton = nullptr;
    Button* duckButton = nullptr;
    Button* jumpButton = nullptr;
    Button* attackButton = nullptr;
    Button* leftButton = nullptr;
    Button* rightButton = nullptr;
    Button* upButton = nullptr;
    Button* downButton = nullptr;
    
    // Timing
    float tileFrameSpeed = TILE_FRAME_SPEED;
    
    Game();
    ~Game();
    
    void init();
    void run();
    void update();
    void draw();
    
    // Game logic
    void setUpGame();
    void titleScreen();
    void makeUI();
    void makeButtons();
    void loadMazes();
    void makeTiles();
    void makeRoomFrame(Room* room);
    void makeAddons(Room* room);
    void makeChests(Room* room);
    void addMoreChests();
    void makeDoor();
    void buildRoom();
    void addSkeletons();
    void spawnSkeleton();
    float skeletonChance(float x);
    void toggleMap(bool setTo = false);
    void openChest(Chest* chest);
    void exitGame();
    void updateButtons();
    void changePlayerSpeed();
    void pressingUpOrDown();
    void realignPlayer();
    void entityMove();
    void animateLoot();
    void animateEntities();
    void animateTiles();
    void animateHearts();
    void playSounds();
    void drawScene();
    void drawUI();
    
    // Input handlers
    void onKeyDown(int key);
    void onKeyUp(int key);
    void onMouseDown(int button);
    void onMouseUp(int button);
    void onMouseMove(Vector2 pos);
};

#endif // GAME_H