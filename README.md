# VectorStrike

VectorStrike is a top-down arena shooter built with C++17, SFML 3, and CMake. Move through the arena, aim with the mouse, defeat chasing enemies, and watch the spatial-hash collision benchmark scale from normal gameplay to 1,000 entities.

## Features

- WASD player movement with diagonal normalization
- Mouse aiming and held left-click shooting
- Visible projectiles with bullet/enemy collision detection
- Enemy health, damage, death, score, and attack cooldowns
- Chase and attack AI with separation behavior
- Player and enemy health bars
- Hit effects, crosshair, colored entities, HUD, and restart screen
- Spatial-hash broad phase for enemy and bullet collision queries
- Benchmark modes for 200, 500, and 1,000 entities

## Controls

| Input | Action |
| --- | --- |
| `W` `A` `S` `D` | Move the player |
| Left mouse button | Fire toward the cursor |
| `F1` | Return to normal gameplay |
| `F2` | Run the 200-enemy benchmark |
| `F3` | Run the 500-enemy benchmark |
| `F4` | Run the 1,000-enemy benchmark |
| `R` | Restart after game over |

## Requirements

- CMake 3.20 or newer
- A C++17 compiler
- SFML 3 with Graphics, Window, and System components
- Windows users can use MinGW/MSYS2 with SFML installed through the same toolchain

## Build

From the project directory:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Run the game:

```powershell
.\build\VectorStrike.exe
```

For a Visual Studio generator, specify the configuration when building:

```powershell
cmake -S . -B build-vs
cmake --build build-vs --config Debug
.\build-vs\Debug\VectorStrike.exe
```

## Gameplay HUD

Normal mode displays score, health, FPS, frame time, enemy count, bullet count, and collision checks. Benchmark modes freeze enemy movement and spawning while preserving the spatial-hash workload, so the collision cost can be compared at each entity count.

## Project Layout

```text
include/   Public C++ headers
src/       Game and entity implementations
assets/    Runtime assets
build/     Generated build output, ignored by Git
```

## License

No license has been specified for this project yet.
