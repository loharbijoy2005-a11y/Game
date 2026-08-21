# 🔫 ShadowArrow - Free Fire Web 3D

ShadowArrow is a complete, lightweight, standalone 3D Third-Person Shooter (TPS) web prototype inspired by Free Fire. Built from scratch in vanilla HTML5, CSS3, and JavaScript using the **Three.js** WebGL graphics engine, the game runs directly inside Google Chrome without requiring any local app build tools, compilers, or server setups.

---

## 🕹️ Controls & Mechanics

To play, double-click `index.html` or open the live deployment link in a browser, then click **DEPLOY TO ARENA** to lock your mouse pointer.

* **W / A / S / D** (or **Arrow Keys**): Move (Run, Strafe, Back)
* **Left Shift**: Sprint (increases movement speed)
* **Spacebar**: Jump (leaps over crates and low barricades)
* **Mouse Move**: Aim (orbit-based Third-Person camera tracking looking past the player's right shoulder)
* **Left Mouse Click**: Fire Weapon (spawns muzzle flash, recoils camera, and shoots tracers)
* **Key R**: Reload ammo
* **ESC**: Release mouse lock
* **🔊/🔇 Button (Bottom Right)**: Toggle sound effects and synthesizers.

---

## 🛡️ Key Features

* **3D Tactical Arena**: Complete custom-built training ground containing concrete walls, metallic crates for cover, and floating medkits.
* **3D Character Model**: Multi-node skeletal character rigged with running, jumping, and weapon-aiming animations.
* **Smart Bot AI**: Bots patrol the arena, acquire targets when the player is within range, close in, and shoot red tracer lasers.
* **Combat Feedback**:
  * Crosshair reticle with hit-marker reactions (flashes red on hit).
  * Real-time damage numbers: **Red for Headshots** (45-60 dmg), **Yellow for Body shots** (15-25 dmg) projected to screen space.
  * Bullet impact sparks and blood splash effects.
* **HUD Overlay**: Tactical HUD displaying player health, shield points, weapon ammo count, match score, and remaining enemies count.
* **Procedural Sound Synthesizer**: Utilizes the browser's Web Audio API to procedurally synthesize sound effects for gun firing, reload, hits, damage taken, and health pickups.

---

## 🚀 Live Deployment to GitHub Pages

The repository contains a fully automated deployment workflow.

### To Make it Live:
1. Go to your repository settings page:
   👉 **[https://github.com/loharbijoy2005-a11y/Game/settings/pages](https://github.com/loharbijoy2005-a11y/Game/settings/pages)**
2. In the **Build and deployment** section, select **GitHub Actions** as the source.
3. Push any changes to the `main` branch. The system will build and host the game automatically!

### Play Live:
🔗 **[https://loharbijoy2005-a11y.github.io/Game/](https://loharbijoy2005-a11y.github.io/Game/)**
