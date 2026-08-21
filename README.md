# 🏃 Stickman Temple Run 3D - C++ Raylib Version

Stickman Temple Run 3D is a native desktop 3D endless runner game built from scratch in **C++** using the **Raylib** graphics library. Escape the ancient temple spirits by jumping, sliding, and shifting lanes to dodge obstacle barricades, deep pit fissures, and low-clearance stone arches!

This version features **true 3D perspective camera projections**, a **3D skeletal stickman drawing algorithm** rendering running/jumping/sliding animations using mathematical primitives, and a **real-time procedural sound synthesizer** generating all audio effects in memory.

---

## 🕹️ Controls & Menus

### Menu Controls:
* **ENTER** / **SPACE** / **UP** / **W**: Start/Restart Run
* **H**: Open How to Play instructions
* **ESC** / **B**: Back to Main Menu from Instructions
* **ESC** / **M**: Exit Game Over screen back to Main Menu

### Running Controls:
* **Left Arrow** / **A**: Switch Lane Left (leans and banks left)
* **Right Arrow** / **D**: Switch Lane Right (leans and banks right)
* **Up Arrow** / **W** / **SPACE**: Jump (clears barricades and pit fissures)
* **Down Arrow** / **S**: Slide / Duck (slides beneath high stone arches)

---

## 🛡️ Power-Ups & Collectibles
* **🪙 Gold Coins**: Collect coins to increase your score multiplier. Each coin adds 12 points to your total run score.
* **🛡️ Shield (Blue Aura)**: Absorbs a single crash against barricades or stone arches. *Note: Shields do not protect against falling into ground pits.*
* **🧲 Magnet (Yellow Aura)**: Attracts nearby gold coins, pulling them towards you automatically.
* **⚡ Speed Boost (Red Aura)**: Grants temporary invincibility, breaks obstacles, and boosts running speed.

---

## 🛠️ Compilation & Local Build Guide

To build and run the game, you will need a C++ compiler (`g++` / MinGW-w64) and the Raylib library installed.

### Windows (MSYS2 / MinGW-w64):
1. Install MSYS2 from [msys2.org](https://www.msys2.org/).
2. Open the MSYS2 MinGW-w64 terminal and install GCC and Raylib:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib
   ```
3. Add MinGW-w64 binary path (typically `C:\msys64\mingw64\bin`) to your system **PATH** environment variable.
4. Double-click the **[`compile.bat`](file:///d:/game/compile.bat)** script inside the project folder, or compile it manually in your terminal:
   ```bash
   g++ main.cpp -o game.exe -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
   ```
5. Run the generated **`game.exe`** to play!

### macOS / Linux:
1. Install Raylib:
   * **macOS** (Homebrew): `brew install raylib`
   * **Ubuntu/Debian**: `sudo apt install libraylib-dev`
2. Compile via Terminal:
   ```bash
   # macOS
   g++ main.cpp -o game -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
   
   # Linux
   g++ main.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
   ```
3. Run with `./game`!

---

## 📦 Project Architecture
* **[`main.cpp`](file:///d:/game/main.cpp)**: Unified game logic, including:
  * **3D Stickman Animator**: Computes skeletal coordinates for nodes (head, hip, knees, elbows, feet, hands) and draws them dynamically using Raylib cylinders and spheres.
  * **Procedural Synth**: Generates custom sound waves (using math formula envelopes) in system RAM and streams them to the sound card.
  * **Physics Loop**: Adjusts speed, gravity, and animations using delta time (`GetFrameTime()`) for frame-rate independence.
  * **Bounding Collision**: High-accuracy spatial collision checking to prevent objects clipping through player coordinates.
* **`highscore.dat`**: Locally saves your best runs.
