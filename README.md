# 🏃 Stickman Temple Run - Endless Runner 3D

Stickman Temple Run is a polished, lightweight, 3D perspective endless runner game built from scratch using HTML5 Canvas, modern CSS glassmorphism styling, and custom synthesized real-time audio. Escape the ancient temple spirits by jumping, sliding, and shifting lanes to dodge obstacle barricades, deep pit fissures, and low-clearance stone arches!

## 🕹️ How to Play & Controls

The game is playable on both desktop and mobile/touch browsers:

### Keyboard Controls (Desktop)
* **Left Arrow** / **A**: Switch Lane Left (bank lean shift)
* **Right Arrow** / **D**: Switch Lane Right (bank lean shift)
* **Up Arrow** / **W** / **SPACE**: Jump (leaps over barricades and pit fissures)
* **Down Arrow** / **S**: Slide / Duck (slides under high stone arches)

### Touch / Click Controls (Mobile & Desktop Overlay)
* **◀ Button**: Switch Lane Left
* **▶ Button**: Switch Lane Right
* **JUMP Button**: Jump
* **SLIDE Button**: Slide / Duck
* **🔊/🔇 Floating Button (Bottom Right)**: Toggle sound effects and background music loop.

---

## 🛡️ Power-Ups & Collectibles
* **🪙 Gold Coins**: Collect coins to increase your high score multipliers. Each coin adds 12 points to your total run score.
* **🛡️ Shield**: Absorbs a single crash against barrier boards or stone arches. *Note: Shields do not protect against falling into deep ground pits.*
* **🧲 Magnet**: Magnetizes nearby gold coins, pulling them towards you automatically.
* **⚡ Speed Boost**: Grants temporary invincibility, breaks obstacles, and boosts running speed.

---

## 🛠️ Local Development & Setup

This is a **zero-dependency, single-file production-ready project**. Everything (HTML layout, CSS styling, Web Audio API synthesis, and 3D Canvas rendering logic) is self-contained inside `index.html`.

### Run Locally:
1. Double-click `index.html` to open it in any modern web browser.
2. Click **RUN NOW** to start playing!

---

## 🐙 Git Deployment: Transforming your Repository

To rename `test.txt` into `index.html` directly inside your GitHub repository and deploy the game, follow these instructions.

### Option A: Via Command Line (Git Terminal)

Run the following commands in your local project terminal:

```bash
# 1. Initialize git (if not already initialized)
git init

# 2. Add the remote URL
git remote add origin https://github.com/loharbijoy2005-a11y/Game.git

# 3. Pull the remote files (gets the test.txt)
git pull origin main

# 4. Rename test.txt to index.html using Git
git mv test.txt index.html

# 5. Add index.html and README.md
git add index.html README.md

# 6. Commit the changes
git commit -m "Transform repository into polished 3D endless runner web game"

# 7. Push to GitHub
git push -u origin main
```

### Option B: Directly on the GitHub Web Interface

If you do not have Git installed locally, you can rename the file directly on GitHub:
1. Go to your repository on GitHub: `https://github.com/loharbijoy2005-a11y/Game.git`
2. Click on the `test.txt` file.
3. Click the **pencil icon** ✏️ (Edit this file) in the top-right corner.
4. Delete the name `test.txt` in the file name input box and type `index.html`.
5. Replace the contents of the text editor box with the entire contents of your new `index.html` file.
6. Click **Commit changes...** at the top right to save.
7. To add the `README.md`, click **Add file** -> **Create new file** on the repository main page, name it `README.md`, paste this README content, and click **Commit changes...**.
