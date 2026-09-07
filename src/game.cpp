#include "game.h"
#include "constants.h"
#include "utils.h"
#include "asset_manager.h"
#include "tile.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>

Game::Game() {
    uiBarLeft = Rect(0, 0, UI_BAR_WIDTH, HEIGHT);
    uiBarRight = Rect(WIDTH - UI_BAR_WIDTH, 0, UI_BAR_WIDTH, HEIGHT);
}

Game::~Game() {
    delete player;
    delete maze;
    delete mazeMap;
    delete background;
    delete titleText;
    delete shop;
    delete chestIcon;
    for (Heart* h : hearts) delete h;
    for (Entity* e : lootIcons) delete e;
    for (Loot* l : allLoot) delete l;
    for (Button* b : allButtons) delete b;
    allButtons.clear();
}

void Game::init() {
    InitWindow(WIDTH, HEIGHT, "Greg Games - Dungeon");
    SetTargetFPS(60);
    
    // Load mazes from file
    loadMazes();
    
    // Create player
    player = new Player();
    player->hitbox.x = SCENE_WIDTH / 2.0f;
    player->hitbox.y = HEIGHT - 120 - player->hitbox.height / 2.0f;
    
    // Create UI
    makeUI();
    makeButtons();
    
    // Background
    background = new Entity("background", "game", 0, 1);
    background->currentTexturePath = "background/game";
    background->currentTexture = &textureManager.load(background->currentTexturePath);
    background->hitbox = Rect(WIDTH/2, HEIGHT/2, 0, 0);
    
    // Title text
    titleText = new Entity("title", "", 0, 1);
    titleText->currentTexturePath = "title";
    titleText->currentTexture = &textureManager.load(titleText->currentTexturePath);
    
    // Shop
    shop = new Entity("tiles/shop", "variant_4", 0, 1);
    shop->currentTexturePath = "tiles/shop/variant_4";
    shop->currentTexture = &textureManager.load(shop->currentTexturePath);
    shop->hitbox = Rect(WIDTH/2, HEIGHT/2 - 3, 0, 0);
    
    // Start at title screen
    titleScreen();
}

void Game::loadMazes() {
    std::ifstream file("assets/labirynty.txt");
    if (!file.is_open()) {
        std::cerr << "Could not open labirynty.txt" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        int width = std::stoi(line);
        std::getline(file, line);
        int height = std::stoi(line);
        
        std::vector<Room*> rooms;
        for (int y = 0; y < height; y++) {
            std::getline(file, line);
            for (int x = 0; x < width; x++) {
                if (x < (int)line.size()) {
                    char c = line[x];
                    std::array<int, 4> exits = {0, 0, 0, 0};
                    // Parse the symbol to exits (simplified)
                    rooms.push_back(new Room(exits, 0, {}));
                }
            }
        }
        // Note: In the original, mazes are stored globally. We'll just use the first one or generate.
    }
}

void Game::makeUI() {
    // Chest icon
    chestIcon = new Entity("ui/chest_icon", "", 0, 1);
    chestIcon->currentTexturePath = "ui/chest_icon";
    chestIcon->currentTexture = &textureManager.load(chestIcon->currentTexturePath);
    chestIcon->hitbox = Rect(UI_BAR_WIDTH/2 - chestIcon->hitbox.width, 2.3f * chestIcon->hitbox.height, 0, 0);
    
    // Loot icons
    std::vector<std::pair<int, int>> positions = {{-2, 1}, {0, 1}, {2, 1}, {1, 3}};
    int i = 0;
    for (const auto& [key, _] : lootCollected) {
        Entity* icon = new Entity("ui/" + key + "_icon", "", 0, 1);
        icon->currentTexturePath = "ui/" + key + "_icon";
        icon->currentTexture = &textureManager.load(icon->currentTexturePath);
        icon->name = key;
        int x = positions[i].first;
        int y = positions[i].second;
        icon->hitbox = Rect(UI_BAR_WIDTH/2 + icon->hitbox.width * x, icon->hitbox.height * y, 0, 0);
        lootIcons.push_back(icon);
        i++;
    }
    
    // Hearts
    float x = WIDTH - (UI_BAR_WIDTH * 4.0f / 5.0f) - 30;
    for (int j = 0; j < 5; j++) {
        Heart* heart = new Heart();
        heart->hitbox = Rect(x, heart->hitbox.height * 1.5f, 0, 0);
        x += UI_BAR_WIDTH / 5.0f;
        hearts.push_back(heart);
    }
}

void Game::makeButtons() {
    playButton = new Button("play", [this]() { setUpGame(); });
    playButton->setPos({(float)WIDTH/2, (float)HEIGHT * 3.0f/4.0f});
    
    mapButton = new Button("map", [this]() { toggleMap(); });
    mapButton->setPos({(float)WIDTH - UI_BAR_WIDTH + mapButton->hitbox.width, (float)HEIGHT - mapButton->hitbox.height});
    
    duckButton = new Button("duck", [this]() { 
        if (player->state == STATE_IDLE || player->state == STATE_RUNNING) 
            player->changeState(STATE_DUCK); 
    });
    duckButton->setPos({(float)WIDTH - duckButton->hitbox.width, (float)HEIGHT/2 + duckButton->hitbox.height/2});
    
    jumpButton = new Button("jump", [this]() { 
        if (player->state == STATE_IDLE || player->state == STATE_RUNNING) 
            player->changeState(STATE_JUMP); 
    });
    jumpButton->setPos({(float)WIDTH - jumpButton->hitbox.width, (float)HEIGHT/2 - jumpButton->hitbox.height/2});
    
    attackButton = new Button("attack", [this]() { 
        if (player->state == STATE_IDLE || player->state == STATE_RUNNING) 
            player->changeState(STATE_ATTACK1); 
    });
    attackButton->setPos({(float)WIDTH - attackButton->hitbox.width * 2, (float)HEIGHT/2});
    attackButton->disable();
    
    // Movement buttons
    Texture2D* leftTex = textureManager.get("ui/buttons/left");
    float btnWidth = leftTex ? leftTex->width : 60;
    float btnHeight = leftTex ? leftTex->height : 60;
    Vector2 center = {btnWidth * 1.5f, (float)HEIGHT - btnHeight * 1.5f};
    
    leftButton = new Button("left", 
        [this]() { goLeft = true; }, 
        [this]() { goLeft = false; });
    leftButton->setPos({center.x - btnWidth, center.y});
    
    rightButton = new Button("right", 
        [this]() { goRight = true; }, 
        [this]() { goRight = false; });
    rightButton->setPos({center.x + btnWidth, center.y});
    
    upButton = new Button("up", [this]() { goUp = (!goDown && player->canMove); });
    upButton->setPos({center.x, center.y - btnHeight});
    
    downButton = new Button("down", [this]() { goDown = (!goUp && player->canMove); });
    downButton->setPos({center.x, center.y + btnHeight});
}

void Game::setUpGame() {
    gameEnded = false;
    floor++;
    
    // Generate maze
    int mazeWidth = std::min(rand() % 2 + floor + 4, 13);
    int mazeHeight = std::min(rand() % 2 + floor/2 + 2, 8);
    maze = new Maze(mazeWidth, mazeHeight);
    std::cout << maze->toString() << std::endl;
    
    mazeMap = new MazeMap(maze);
    
    player->hitbox.x = SCENE_WIDTH / 2.0f;
    player->changeState(STATE_IDLE);
    player->isDead = false;
    player->colliding.clear();
    
    background->currentTexturePath = "background/game";
    background->currentTexture = &textureManager.load(background->currentTexturePath);
    
    for (Button* btn : allButtons) btn->show();
    playButton->hide();
    attackButton->disable();
    
    allSprites.clear();
    allLoot.clear();
    
    roomNumber = 0;
    toggleMap(false);
    numberOfChests = 0;
    numberOfFoundChests = 0;
    numVisitedRooms = 0;
    
    makeTiles();
    buildRoom();
}

void Game::makeTiles() {
    for (int i = 0; i < maze->size; i++) {
        Room* room = maze->rooms[i];
        room->setDistance(maze->distance[i]);
        makeRoomFrame(room);
        if (i == 0) {
            makeDoor();
        } else if (room->isDeadEnd()) {
            makeChests(room);
        }
    }
    addMoreChests();
    for (Room* room : maze->rooms) {
        makeAddons(room);
    }
}

void Game::makeRoomFrame(Room* room) {
    Texture2D* brickTex = &textureManager.load("tiles/brick/variant_0");
    float brickW = brickTex->width;
    float brickH = brickTex->height;
    
    auto makeWall = [&](bool exit, float x) {
        if (!exit) {
            for (int j = 0; j < HEIGHT / brickH - 2; j++) {
                int variant = rand() % 8;
                Tile* tile = new Tile("brick", variant, {x, brickH * (1.5f + j)});
                room->addTile(tile);
            }
        }
    };
    
    auto makeLadder = [&](bool exit, float yBrick, float yLadder) {
        if (!exit) {
            int variant = rand() % 8;
            Tile* tile = new Tile("brick", variant, {SCENE_WIDTH/2, yBrick});
            room->addTile(tile);
        } else {
            Tile* tile = new Tile("ladder", NO_VARIANT, {SCENE_WIDTH/2, yLadder});
            room->addTile(tile);
        }
    };
    
    // Top and bottom walls
    for (int row = 0; row < 2; row++) {
        float y = (row == 0) ? brickH/2 : HEIGHT - brickH;
        for (int side = 0; side < 2; side++) {
            float x = (side == 0) ? 0 : SCENE_WIDTH/2 + brickW/2;
            for (int j = 0; j < (SCENE_WIDTH - 1) / (2 * brickW); j++) {
                int variant = rand() % 8;
                Tile* tile = new Tile("brick", variant, {brickW * (j + 0.5f) + x, y});
                room->addTile(tile);
            }
        }
    }
    
    makeWall(room->west(), brickW/2);
    makeLadder(room->north(), brickH/2, (HEIGHT - brickH)/2);
    makeWall(room->east(), SCENE_WIDTH - brickW/2);
    makeLadder(room->south(), HEIGHT - brickH/2, HEIGHT);
}

void Game::makeAddons(Room* room) {
    for (Addon* addon : allAddons) {
        for (int i = 0; i < addon->maxNumberOnScreen; i++) {
            if ((float)rand() / RAND_MAX < addon->chance) {
                int variant = rand() % std::max(1, addon->numberOfVariants);
                Tile* newAddon = nullptr;
                
                if (addon->isAnim) {
                    newAddon = new AnimatedTileAddon(addon->name, variant, "default", {0, 0});
                } else {
                    newAddon = new TileAddon(addon->name, variant, {0, 0});
                }
                
                bool posAvailable = false;
                for (int attempt = 0; attempt < 20; attempt++) {
                    newAddon->pos = {(float)(addon->xStart + rand() % (addon->xEnd - addon->xStart + 1)),
                                     (float)(addon->yStart + rand() % (addon->yEnd - addon->yStart + 1))};
                    
                    posAvailable = true;
                    for (Tile* tile : room->tiles) {
                        if (addon->canCollide.empty() || tile->name != addon->canCollide) {
                            int distance = 0;
                            if (tile->name == addon->name) {
                                distance = addon->minDistance;
                            }
                            Rect r1 = newAddon->getRect();
                            Rect r2 = tile->getRect();
                            if (isColliding(r1, r2, distance)) {
                                posAvailable = false;
                                break;
                            }
                        }
                    }
                    if (posAvailable) break;
                }
                
                if (posAvailable) {
                    room->addTile(newAddon);
                } else {
                    delete newAddon;
                }
            }
        }
    }
}

void Game::makeChests(Room* room) {
    room->addChest(floor);
    numberOfChests++;
}

void Game::addMoreChests() {
    for (int i = 0; i < maze->size; i++) {
        Room* room = maze->rooms[i];
        for (Tile* tile : room->tiles) {
            Chest* chest = dynamic_cast<Chest*>(tile);
            if (chest) {
                for (Room* r : maze->roomsInRange(i, 3)) {
                    r->isChestInRange = true;
                }
                break;
            }
        }
    }
    
    for (int i = 0; i < maze->size; i++) {
        Room* room = maze->rooms[i];
        int maxDist = rand() % maze->size;
        if (room->chestIsAllowed(maxDist)) {
            room->addChest(floor);
            numberOfChests++;
            for (Room* r : maze->roomsInRange(i, 3)) {
                r->isChestInRange = true;
            }
        }
    }
}

void Game::makeDoor() {
    Room* room = maze->rooms[0];
    Texture2D* doorTex = &textureManager.load("tiles/door/variant_0/closing/0");
    Door* door = new Door("door", 0, "closing", 
        {SCENE_WIDTH/2, static_cast<float>(HEIGHT - 120 - doorTex->height/2)});
    door->isAnimating = true;
    room->addTile(door);
}

void Game::buildRoom() {
    currentRoom = maze->rooms[roomNumber];
    numVisitedRooms++;
    addSkeletons();
    
    allSprites.clear();
    for (Tile* tile : currentRoom->tiles) {
        Entity* sprite = new Entity(tile->name, "", 0, 1);
        sprite->currentTexturePath = tile->imagePath();
        sprite->currentTexture = &textureManager.load(sprite->currentTexturePath);
        sprite->hitbox = Rect(tile->pos.x, tile->pos.y, 0, 0);
        if (sprite->currentTexture) {
            sprite->hitbox.width = sprite->currentTexture->width;
            sprite->hitbox.height = sprite->currentTexture->height;
        }
        sprite->name = tile->name;
        allSprites.push_back(sprite);
    }
    
    for (Enemy* enemy : currentRoom->enemies) {
        allSprites.push_back(enemy);
    }
    allSprites.push_back(player);
    updateAllSprites(allSprites);
    
    for (Room* r : maze->rooms) {
        r->lastVisited++;
    }
    currentRoom->lastVisited = 0;
    currentRoom->visited = true;
}

void Game::addSkeletons() {
    Room* room = maze->rooms[roomNumber];
    if (room->hasClosedChest && room->numSkeletons == 0) {
        room->numSkeletons++;
        spawnSkeleton();
    } else if (room->skeletonIsAllowed() && (float)rand() / RAND_MAX < skeletonChance((float)numVisitedRooms / maze->size)) {
        if ((float)rand() / RAND_MAX < 0.5f && std::find(room->tileNames.begin(), room->tileNames.end(), "ladder") == room->tileNames.end()) {
            room->numSkeletons++;
            spawnSkeleton();
        }
        room->numSkeletons++;
        spawnSkeleton();
    }
}

void Game::spawnSkeleton() {
    Enemy* skeleton = new Enemy("skeleton", "variant_0", floor);
    skeleton->hitbox.y = HEIGHT - 120 - skeleton->hitbox.height/2;
    
    bool canSpawn = false;
    for (int attempt = 0; attempt < 1000; attempt++) {
        float x = SCENE_WIDTH/2 + (SCENE_WIDTH/4 - 30) * (2 * (rand() % 2) - 1) + 
                  (float)(rand() % (int)(SCENE_WIDTH/2 - 300) - (SCENE_WIDTH/4 - 150));
        skeleton->hitbox.x = x;
        canSpawn = true;
        for (Enemy* e : currentRoom->enemies) {
            if (isColliding(skeleton->hitbox, e->hitbox, 10)) {
                canSpawn = false;
                break;
            }
        }
        if (canSpawn) break;
    }
    
    if (canSpawn) {
        currentRoom->enemies.push_back(skeleton);
    } else {
        delete skeleton;
    }
}

float Game::skeletonChance(float x) {
    return std::min(pow(2.0f, 2.0f * (x - 3)) + 0.075f, 0.75f);
}

void Game::toggleMap(bool setTo) {
    if (setTo) mapOpen = setTo;
    else mapOpen = !mapOpen;
    
    if (mapOpen) {
        player->canMove = false;
        jumpButton->disable();
        duckButton->disable();
    } else {
        player->canMove = allLoot.empty();
        jumpButton->enable();
        duckButton->enable();
    }
}

void Game::openChest(Chest* chest) {
    numberOfFoundChests++;
    chest->isAnimating = true;
    currentRoom->hasClosedChest = false;
    soundManager.play(chest->soundPath());
    
    for (const auto& [type, range] : chest->lootTable) {
        int count = range.first + rand() % (range.second - range.first + 1);
        for (int i = 0; i < count; i++) {
            Loot* loot = new Loot(type, chest->pos);
            allSprites.push_back(loot);
            allLoot.push_back(loot);
        }
    }
}

void Game::exitGame() {
    std::cout << "game ended" << std::endl;
    gameEnded = true;
    setUpGame();
}

void Game::titleScreen() {
    gameEnded = true;
    background->currentTexturePath = "background/title";
    background->currentTexture = &textureManager.load(background->currentTexturePath);
    floor = 0;
    player->health = 15;
    for (Button* btn : allButtons) btn->hide();
    playButton->show();
    for (auto& [key, val] : lootCollected) val = 0;
}

void Game::updateButtons() {
    buttonsOn("unpressed", mouseHitbox);
    buttonsOn("pressed", mouseHitbox);
    
    attackButton->disable();
    for (Entity* thing : player->attacking) {
        if (thing->name == "skeleton" && thing->state == STATE_IDLE && 
            player->state != STATE_HIT && player->state != STATE_DIE) {
            attackButton->enable();
            break;
        }
    }
}

void Game::changePlayerSpeed() {
    player->speed = 0;
    bool leftPressed = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || goLeft;
    bool rightPressed = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || goRight;
    
    if (leftPressed ^ rightPressed) {
        if (player->canMove) {
            player->speed = player->runningSpeed * dt;
            if (player->state == STATE_IDLE || player->state == STATE_RUNNING || player->state == STATE_JUMP) {
                if (leftPressed) {
                    if (player->dir == RIGHT) player->hitbox.x -= player->hitbox.width / 20.0f;
                    player->dir = LEFT;
                } else {
                    if (player->dir == LEFT) player->hitbox.x += player->hitbox.width / 20.0f;
                    player->dir = RIGHT;
                }
            }
        }
    }
}

void Game::pressingUpOrDown() {
    if (gameEnded) return;
    
    if (goUp) {
        for (Entity* thing : player->colliding) {
            if (thing->name == "ladder" && currentRoom->north()) {
                soundManager.play("ladder/ladder");
                roomNumber -= maze->width;
                buildRoom();
                break;
            } else if (thing->name == "door" && numberOfChests == numberOfFoundChests) {
                exitGame();
                break;
            }
        }
    } else if (goDown && currentRoom->south()) {
        for (Entity* thing : player->colliding) {
            if (thing->name == "ladder") {
                soundManager.play("ladder/ladder");
                roomNumber += maze->width;
                buildRoom();
                break;
            }
        }
    }
    goUp = goDown = false;
}

void Game::realignPlayer() {
    player->updateColliding();
    for (Entity* thing : player->colliding) {
        if ((thing->name == "brick" || thing->name == "skeleton") && player->isLookingAt(thing)) {
            player->hitbox.x -= player->speed * player->dir;
            if (player->state == STATE_RUNNING) {
                player->changeState(STATE_IDLE);
            }
            player->updateColliding();
            break;
        }
    }
    
    for (Entity* thing : player->colliding) {
        if (thing->name == "spikes") {
            if (!player->isLookingAt(thing)) {
                player->hitbox.x += 10 * player->dir;
            }
            if (checkCollisionRecs(player->hitbox, thing->hitbox)) {
                if (player->hitbox.x > thing->hitbox.x) {
                    player->hitbox.x += thing->hitbox.right() - player->hitbox.left() + 10;
                } else {
                    player->hitbox.x += thing->hitbox.left() - player->hitbox.right() - 10;
                }
                player->changeState(STATE_HIT);
                player->updateColliding();
            }
        }
    }
    
    if (player->hitbox.x < player->hitbox.width * 0.6f && currentRoom->west()) {
        roomNumber--;
        player->hitbox.x = SCENE_WIDTH - player->hitbox.width * 0.61f;
        buildRoom();
    } else if (player->hitbox.x > SCENE_WIDTH - player->hitbox.width * 0.6f && currentRoom->east()) {
        roomNumber++;
        player->hitbox.x = player->hitbox.width * 0.61f;
        buildRoom();
    }
}

void Game::entityMove() {
    for (size_t i = 0; i < allSprites.size(); ) {
        Entity* sprite = allSprites[i];
        Enemy* enemy = dynamic_cast<Enemy*>(sprite);
        
        if (enemy) {
            if (enemy->state == STATE_DIE && enemy->frame > 2 && currentRoom->hasClosedChest) {
                Chest* chest = maze->getChestFor(roomNumber);
                if (chest) openChest(chest);
            }
            
            if (enemy->isDead) {
                currentRoom->enemies.erase(
                    std::remove(currentRoom->enemies.begin(), currentRoom->enemies.end(), enemy),
                    currentRoom->enemies.end()
                );
                allSprites.erase(allSprites.begin() + i);
                currentRoom->numSkeletons--;
                delete enemy;
                continue;
            } else {
                if (enemy->state == STATE_RUNNING || enemy->frame == 0) {
                    if (enemy->hitbox.x > player->hitbox.x) enemy->dir = LEFT;
                    else enemy->dir = RIGHT;
                    
                    enemy->updateColliding();
                    bool canMove = true;
                    for (Entity* thing : enemy->colliding) {
                        if (thing->name == "player" || (thing->name == "skeleton" && enemy->isLookingAt(thing))) {
                            canMove = false;
                        }
                    }
                    if (canMove) {
                        enemy->goToPlayer(player->hitbox.x, dt);
                        enemy->move();
                    }
                }
                enemy->updateColliding();
                for (Entity* thing : enemy->colliding) {
                    if (thing->name == "skeleton" && enemy->isLookingAt(thing)) {
                        enemy->hitbox.x -= enemy->speed * enemy->dir;
                        enemy->changeState(STATE_IDLE);
                        enemy->speed = 0;
                        enemy->updateColliding();
                    }
                    if (thing->name == "player") {
                        enemy->hitbox.x -= enemy->speed * enemy->dir;
                        if (thing->state == STATE_HIT) {
                            enemy->attackProgress = 1;
                        }
                        enemy->attack();
                    }
                }
            }
        }
        i++;
    }
    
    player->move();
    realignPlayer();
}

void Game::animateLoot() {
    if (!allLoot.empty()) {
        player->canMove = false;
        if (allLoot.back()->isAnimatingFinished(dt)) {
            lootCollected[allLoot.back()->name]++;
            allSprites.erase(
                std::remove(allSprites.begin(), allSprites.end(), allLoot.back()),
                allSprites.end()
            );
            delete allLoot.back();
            allLoot.pop_back();
        }
    } else {
        player->canMove = !mapOpen;
    }
}

void Game::animateEntities() {
    player->animate(dt);
    for (Entity* sprite : allSprites) {
        Enemy* enemy = dynamic_cast<Enemy*>(sprite);
        if (enemy) enemy->animate(dt);
    }
}

void Game::animateTiles() {
    for (size_t i = 0; i < currentRoom->tiles.size(); i++) {
        Tile* tile = currentRoom->tiles[i];
        AnimatedTile* animTile = dynamic_cast<AnimatedTile*>(tile);
        if (animTile) {
            animTile->nextFrame(TILE_FRAME_SPEED * dt);
            if (i < allSprites.size()) {
                allSprites[i]->currentTexturePath = animTile->imagePath();
                allSprites[i]->currentTexture = &textureManager.load(allSprites[i]->currentTexturePath);
                if (Chest* chest = dynamic_cast<Chest*>(animTile)) {
                    allSprites[i]->hitbox.y = chest->bottom - allSprites[i]->hitbox.height;
                }
                if (animTile->name == "door" && animTile->hasFinishedAnimating) {
                    animTile->isAnimating = false;
                    animTile->frame = 0;
                    if (animTile->state == "closing") {
                        soundManager.play(animTile->soundPath());
                        animTile->state = "opening";
                        animTile->hasFinishedAnimating = false;
                    } else {
                        exitGame();
                        break;
                    }
                }
            }
        }
    }
}

void Game::animateHearts() {
    int heartHealth = 0;
    for (Heart* h : hearts) heartHealth += h->goalFrame;
    
    std::string side = (heartHealth > player->health * 3) ? "right" : "left";
    
    for (int i = 0; i < 5; i++) {
        int j = (side == "left") ? i : 4 - i;
        Heart* heart = hearts[j];
        if (player->health / 3 < j) {
            heart->changeState(0);
        } else if (player->health / 3 > j) {
            heart->changeState(3);
        } else {
            heart->changeState(player->health % 3);
        }
        if (heart->goalFrame != (int)heart->frame) {
            heart->animate(dt);
            break;
        }
    }
}

void Game::playSounds() {
    for (auto& [sound, state] : player->soundStates) {
        if (state == SOUND_WILL_BE_PLAYED) {
            player->soundStates[sound] = SOUND_IS_PLAYING;
            soundManager.play(player->soundPath(sound));
        } else if (state == SOUND_NOT_PLAYING) {
            soundManager.stop(player->soundPath(sound));
        }
    }
}

void Game::drawScene() {
    for (Entity* sprite : allSprites) {
        sprite->draw(UI_BAR_WIDTH);
        if (showHitboxes && dynamic_cast<Entity*>(sprite)) {
            Entity* e = dynamic_cast<Entity*>(sprite);
            Rect hb = e->hitbox;
            Rect ahb = e->attackHitbox;
            hb.x += UI_BAR_WIDTH;
            ahb.x += UI_BAR_WIDTH;
            DrawRectangleLinesEx(hb.toRaylib(), 2, GREEN);
            DrawRectangleLinesEx(ahb.toRaylib(), 2, RED);
        }
    }
    player->draw(UI_BAR_WIDTH);
    if (showHitboxes) {
        Rect hb = player->hitbox;
        Rect ahb = player->attackHitbox;
        hb.x += UI_BAR_WIDTH;
        ahb.x += UI_BAR_WIDTH;
        DrawRectangleLinesEx(hb.toRaylib(), 2, GREEN);
        DrawRectangleLinesEx(ahb.toRaylib(), 2, RED);
    }
}

void Game::drawUI() {
    if (!gameEnded) {
        DrawRectangleRec(uiBarLeft.toRaylib(), UI_BAR_COLOR);
        DrawRectangleRec(uiBarRight.toRaylib(), UI_BAR_COLOR);
        
        if (chestIcon && chestIcon->currentTexture) {
            Rectangle src = {0, 0, (float)chestIcon->currentTexture->width, (float)chestIcon->currentTexture->height};
            Rectangle dst = {chestIcon->hitbox.x, chestIcon->hitbox.y, (float)chestIcon->currentTexture->width, (float)chestIcon->currentTexture->height};
            DrawTexturePro(*chestIcon->currentTexture, src, dst, {0, 0}, 0.0f, WHITE);
        }
        
        for (Heart* heart : hearts) {
            heart->draw(0);
        }
        
        for (Entity* icon : lootIcons) {
            if (icon->currentTexture) {
                Rectangle src = {0, 0, (float)icon->currentTexture->width, (float)icon->currentTexture->height};
                Rectangle dst = {icon->hitbox.x, icon->hitbox.y, (float)icon->currentTexture->width, (float)icon->currentTexture->height};
                DrawTexturePro(*icon->currentTexture, src, dst, {0, 0}, 0.0f, WHITE);
                
                std::string count = std::to_string(lootCollected[icon->name]);
                Vector2 textPos = {icon->hitbox.x + icon->hitbox.width/2, (float)icon->hitbox.y};
                DrawText(count.c_str(), (int)textPos.x, (int)textPos.y, 20, WHITE);
            }
        }
        
        std::string chestText = std::to_string(numberOfFoundChests) + "/" + std::to_string(numberOfChests);
        if (chestIcon) {
            DrawText(chestText.c_str(), 
                (int)chestIcon->hitbox.x, (int)chestIcon->hitbox.y, 20, YELLOW);
        }
    }
    
    for (Button* btn : allButtons) {
        if (btn->isVisible) btn->draw();
    }
    
    if (showHitboxes) {
        DrawRectangleLinesEx(mouseHitbox.toRaylib(), 2, BLUE);
    }
}

void Game::onKeyDown(int key) {
    if (player->canMove) {
        if (key == KEY_UP || key == KEY_W) goUp = true;
        else if (key == KEY_DOWN || key == KEY_S) goDown = true;
    }
    
    if (player->state == STATE_IDLE || player->state == STATE_RUNNING) {
        if (key == KEY_LEFT_SHIFT && jumpButton->canInteract) {
            player->changeState(STATE_JUMP);
        } else if (key == KEY_LEFT_CONTROL && duckButton->canInteract) {
            player->changeState(STATE_DUCK);
        } else if (key == KEY_SPACE && attackButton->canInteract) {
            player->changeState(STATE_ATTACK1);
        }
    }
    
    if (key == KEY_SPACE && playButton->canInteract) {
        setUpGame();
    }
    
    if (key == KEY_M) {
        toggleMap();
    }
}

void Game::onKeyUp(int key) {
    if (key == KEY_LEFT || key == KEY_A) goLeft = false;
    if (key == KEY_RIGHT || key == KEY_D) goRight = false;
}

void Game::onMouseDown(int button) {
    if (button == MOUSE_BUTTON_LEFT) {
        buttonsOn("clicked", mouseHitbox);
    }
}

void Game::onMouseUp(int button) {
    if (button == MOUSE_BUTTON_LEFT) {
        buttonsOn("released", mouseHitbox);
    }
}

void Game::onMouseMove(Vector2 pos) {
    mouseHitbox.setTopleft({pos.x - 1, pos.y - 1});
}

void Game::update() {
    dt = GetFrameTime() * 1000.0f; // Convert to milliseconds for consistency
    
    updateButtons();
    
    if (!gameEnded) {
        pressingUpOrDown();
        changePlayerSpeed();
        entityMove();
        animateTiles();
        animateEntities();
        animateLoot();
        animateHearts();
        playSounds();
        
        if (player->isDead) {
            titleScreen();
        }
    }
}

void Game::draw() {
    BeginDrawing();
    ClearBackground(BLACK);
    
    if (background && background->currentTexture) {
        Rectangle src = {0, 0, (float)background->currentTexture->width, (float)background->currentTexture->height};
        Rectangle dst = {0, 0, (float)WIDTH, (float)HEIGHT};
        DrawTexturePro(*background->currentTexture, src, dst, {0, 0}, 0.0f, WHITE);
    }
    
    if (!gameEnded) {
        drawScene();
        if (mapOpen && mazeMap) {
            mazeMap->draw(maze, roomNumber);
        }
        if (shop && shop->currentTexture) {
            Rectangle src = {0, 0, (float)shop->currentTexture->width, (float)shop->currentTexture->height};
            Rectangle dst = {shop->hitbox.x, shop->hitbox.y, (float)shop->currentTexture->width, (float)shop->currentTexture->height};
            DrawTexturePro(*shop->currentTexture, src, dst, {0, 0}, 0.0f, WHITE);
        }
    } else {
        if (titleText && titleText->currentTexture) {
            Rectangle src = {0, 0, (float)titleText->currentTexture->width, (float)titleText->currentTexture->height};
            Rectangle dst = {(float)WIDTH/2 - titleText->currentTexture->width/2, (float)HEIGHT/3 - titleText->currentTexture->height/2, (float)titleText->currentTexture->width, (float)titleText->currentTexture->height};
            DrawTexturePro(*titleText->currentTexture, src, dst, {0, 0}, 0.0f, WHITE);
        }
    }
    
    drawUI();
    
    EndDrawing();
}

void Game::run() {
    while (!WindowShouldClose()) {
        // Handle input
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) onMouseDown(MOUSE_BUTTON_LEFT);
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) onMouseUp(MOUSE_BUTTON_LEFT);
        onMouseMove(GetMousePosition());
        
        // Keyboard input
        int key = GetKeyPressed();
        while (key > 0) {
            onKeyDown(key);
            key = GetKeyPressed();
        }
        
        // Key up events (simplified - just check for release)
        if (IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_A)) onKeyUp(KEY_LEFT);
        if (IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_D)) onKeyUp(KEY_RIGHT);
        
        update();
        draw();
    }
    
    CloseWindow();
}
