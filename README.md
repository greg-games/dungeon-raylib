# Dungeon Raylib

A C++ port of the Pygame Zero dungeon crawler game using Raylib.

## Features

- Procedural maze generation with rooms, doors, ladders
- Player with idle/run/jump/duck/attack/hit/die states
- Skeleton enemies with AI (pathfinding, attacking)
- Animated chests with loot tables per floor
- Decorative addons (bones, candles, cracks, moss, spikes)
- Heart-based health UI, loot collection counters
- Touch/mouse button controls + keyboard
- Minimap (M key)
- Full game loop: title → play → game over → restart

## Building

### Desktop (Linux/Windows/macOS)

**Prerequisites:**
- CMake 3.16+
- C++17 compiler
- Raylib 5.5+

**Linux:**
```bash
sudo apt-get install cmake g++ libraylib-dev
cd dungeon_raylib
mkdir build && cd build
cmake ..
cmake --build . --config Release
./DungeonRaylib
```

**Windows (with Raylib installed):**
```cmd
set RAYLIB_PATH=C:\raylib
mkdir build && cd build
cmake ..
cmake --build . --config Release
DungeonRaylib.exe
```

### Web (WebAssembly)

**Using Emscripten:**
```bash
# Install emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# Build
cd dungeon_raylib
mkdir build-web && cd build-web
emcmake cmake -G Ninja ..
ninja
```

The web build outputs `index.html`, `index.js`, `index.wasm`, and `index.data` in the build directory.

**GitHub Actions:**
The repository includes a workflow (`.github/workflows/build-web.yml`) that automatically builds for WebAssembly on push and deploys to GitHub Pages.

To enable GitHub Pages deployment:
1. Go to Settings → Pages
2. Set Source to "GitHub Actions"
3. Push to main branch

The game will be available at `https://<username>.github.io/<repo>/web/`

## Controls

### Keyboard
- **Arrow Keys / WASD**: Move
- **Shift**: Jump
- **Ctrl**: Duck
- **Space**: Attack
- **M**: Toggle minimap

### Touch/Mouse
- On-screen buttons for movement, jump, duck, attack
- Click minimap button to toggle map

## Project Structure

```
dungeon_raylib/
├── CMakeLists.txt
├── README.md
├── assets/
│   ├── images/     (sprites, tiles, UI)
│   ├── sounds/     (.ogg sound files)
│   └── labirynty.txt (maze definitions)
├── include/        (header files)
├── src/            (source files)
├── web/
│   └── shell.html  (Emscripten HTML template)
└── .github/
    └── workflows/
        └── build-web.yml
```

## License

Original game by greg-games. This port maintains the same license.