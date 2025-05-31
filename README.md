# Radon [WORK IN PROGRESS]

A voxel-based game inspired by the S.T.A.L.K.E.R. universe, featuring a haunting atmosphere and survival mechanics.

## Project Overview

**Radon** is a multiplayer (not yet) game that combines Minecraft-style voxel graphics with the atmosphere and
mechanics of the S.T.A.L.K.E.R. series. The project aims to deliver a unique blend of voxel visuals and deep gameplay
systems, including exploration, survival, and combat.

### Key Features

- 🎮 Voxel world with a fixed, handcrafted map
- 🌍 Atmospheric Zone inspired by S.T.A.L.K.E.R.
- 👥 Multiplayer mode (planned)
- 🧰 Inventory and equipment system
- 🤖 AI-controlled NPCs and mutants
- ⚔️ Combat system (ranged and melee)
- 🧪 Anomalies, radiation, and artifacts
- 🎯 Quest and faction systems

## Tech Stack

- **Programming Language:** C++23
- **Platform:** Windows
- **Graphics Engine:** [Magnum](https://magnum.graphics/)
- **Asynchronous Programming:** [concurrencpp](https://github.com/David-Haim/concurrencpp)
- **Architecture:** ECS (Entity Component System)
- **Build System:** CMake
- **IDE:** CLion

## Project Structure

```
src/
├── core/ # Game core
├── render/ # Rendering and graphics
├── world/ # World and voxel structures
├── utils/ # Utility modules
├── ecs/ # ECS system
├── game/ # Game logic
├── ui/ # User interface
└── input/ # Input handling
```

## System Requirements

- Windows 10/11
- C++23 compatible compiler
- CMake 3.20+
- OpenGL 4.5+
