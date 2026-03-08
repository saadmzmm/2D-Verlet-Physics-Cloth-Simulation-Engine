# 2D Verlet Physics Engine

A lightweight, CPU-based 2D physics engine built from scratch in C++. This project demonstrates core game and physics engine development principles, specifically focusing on soft-body dynamics, cloth simulation, and constraint satisfaction.

![Cloth Simulation Demo](link-to-your-gif-here.gif) 

## Core Architecture
Unlike standard game engines that use Euler integration, this engine utilizes **Verlet Integration** to calculate particle trajectories. This approach inherently stabilizes the simulation, making it highly suitable for complex soft-body interactions like cloth and ropes.

### Key Features
* **Custom Physics Solver:** Calculates position, inferred velocity, and acceleration without relying on external physics libraries.
* **Constraint Satisfaction:** Implements a multi-iteration constraint solver to maintain rigid distances between connected point masses, simulating material tension.
* **Real-Time Interactivity:** * Left-click raycasting to grab, lock, and drag particles while the engine dynamically updates tension.
  * Right-click spatial querying to dynamically sever constraints (tearing the cloth).
* **Memory Management:** Pre-allocates vector memory `reserve()` to prevent pointer invalidation during dynamic grid generation.

## Tech Stack
* **Language:** C++
* **Graphics Library:** SFML 3.0.2 (Simple and Fast Multimedia Library)
* **Compiler Environment:** MSYS2 / MinGW-w64 (UCRT64)

## How to Build and Run
This project is self-contained. Assuming you have a standard MSYS2 UCRT64 environment configured on Windows:

1. Clone this repository.
2. Open a UCRT64 terminal and navigate to the project directory.
3. Compile the engine using the included SFML dynamic libraries:
   ```bash
   g++ main.cpp -I"./SFML-3.0.2/include" -L"./SFML-3.0.2/lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -o physics_engine.exe
   
   ./physics_engine.exe
