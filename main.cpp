#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <time.h>

#define PI 3.1415926535f

// Game Settings
const int CANVAS_WIDTH = 960;
const int CANVAS_HEIGHT = 540;
const float BASE_SPEED = 16.0f;      // Base Z speed in C++ (balanced for 3D physics scale)
const float SPEED_ACCEL = 0.0003f;
const float GRAVITY = 22.0f;
const float MAX_Z = 180.0f;          // Horizon spawn point
const float PLAYER_Z_POSITION = 0.0f; // Player is fixed at Z=0; road/objects move to -Z

// Enum Definitions
enum ObstacleType { OBSTACLE_BARRIER, OBSTACLE_ARCH, OBSTACLE_PIT };
enum PowerupType { POWERUP_SHIELD, POWERUP_MAGNET, POWERUP_BOOST };

// Struct Definitions
struct Obstacle {
    int lane;
    ObstacleType type;
    float z;
    bool passed;
    bool active;
};

struct Coin {
    int lane;
    float y;
    float z;
    bool collected;
    bool active;
};

struct Powerup {
    int lane;
    PowerupType type;
    float z;
    bool collected;
    bool active;
};

struct SidePillar {
    float x;
    float z;
};

struct Particle {
    Vector3 position;
    Vector3 velocity;
    Color color;
    float size;
    float alpha;
    float life;
    float maxLife;
    bool active;
};

// Procedural Sound Wave Synthesizers
Sound GenerateCoinSound() {
    float duration = 0.25f;
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    
    Wave wave = { 0 };
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(frameCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    for (int i = 0; i < frameCount; i++) {
        float t = (float)i / sampleRate;
        float freq = (t < 0.08f) ? 880.0f : 1200.0f;
        float val = sinf(2.0f * PI * freq * t);
        float env = 1.0f - (t / duration); // decay
        samples[i] = (short)(val * env * env * 4000.0f);
    }
    
    Sound sound = LoadSoundFromWave(wave);
    free(wave.data);
    return sound;
}

Sound GenerateJumpSound() {
    float duration = 0.15f;
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    
    Wave wave = { 0 };
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(frameCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    for (int i = 0; i < frameCount; i++) {
        float t = (float)i / sampleRate;
        float freq = 180.0f + (700.0f - 180.0f) * (t / duration); // sweep
        float val = sinf(2.0f * PI * freq * t);
        float env = 1.0f - (t / duration);
        samples[i] = (short)(val * env * 6000.0f);
    }
    
    Sound sound = LoadSoundFromWave(wave);
    free(wave.data);
    return sound;
}

Sound GenerateCrashSound() {
    float duration = 0.6f;
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    
    Wave wave = { 0 };
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(frameCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    for (int i = 0; i < frameCount; i++) {
        float t = (float)i / sampleRate;
        float rumble = sinf(2.0f * PI * (100.0f - 90.0f * (t / duration)) * t);
        float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float val = rumble * 0.6f + noise * 0.4f;
        float env = 1.0f - (t / duration);
        samples[i] = (short)(val * env * env * 12000.0f);
    }
    
    Sound sound = LoadSoundFromWave(wave);
    free(wave.data);
    return sound;
}

Sound GenerateSlideSound() {
    float duration = 0.15f;
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    
    Wave wave = { 0 };
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(frameCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    for (int i = 0; i < frameCount; i++) {
        float t = (float)i / sampleRate;
        float noise = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        float env = 1.0f - (t / duration);
        samples[i] = (short)(noise * env * 3000.0f);
    }
    
    Sound sound = LoadSoundFromWave(wave);
    free(wave.data);
    return sound;
}

Sound GenerateLaneSound() {
    float duration = 0.08f;
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    
    Wave wave = { 0 };
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(frameCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    for (int i = 0; i < frameCount; i++) {
        float t = (float)i / sampleRate;
        float freq = 300.0f + (450.0f - 300.0f) * (t / duration);
        float val = sinf(2.0f * PI * freq * t);
        float env = 1.0f - (t / duration);
        samples[i] = (short)(val * env * 4000.0f);
    }
    
    Sound sound = LoadSoundFromWave(wave);
    free(wave.data);
    return sound;
}

Sound GenerateShieldPopSound() {
    float duration = 0.12f;
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    
    Wave wave = { 0 };
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(frameCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    for (int i = 0; i < frameCount; i++) {
        float t = (float)i / sampleRate;
        float freq = 900.0f - 500.0f * (t / duration);
        float val = sinf(2.0f * PI * freq * t);
        float env = 1.0f - (t / duration);
        samples[i] = (short)(val * env * 6000.0f);
    }
    
    Sound sound = LoadSoundFromWave(wave);
    free(wave.data);
    return sound;
}

Sound GeneratePowerupSound() {
    float duration = 0.35f;
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    
    Wave wave = { 0 };
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = malloc(frameCount * sizeof(short));
    
    short *samples = (short *)wave.data;
    for (int i = 0; i < frameCount; i++) {
        float t = (float)i / sampleRate;
        // Generate arpeggio (notes: 293.66, 349.23, 440.00, 587.33)
        float freq = 293.66f;
        if (t > 0.08f) freq = 349.23f;
        if (t > 0.16f) freq = 440.00f;
        if (t > 0.24f) freq = 587.33f;
        
        float val = sinf(2.0f * PI * freq * t);
        float env = 1.0f - (t / duration);
        samples[i] = (short)(val * env * 4000.0f);
    }
    
    Sound sound = LoadSoundFromWave(wave);
    free(wave.data);
    return sound;
}

// Particle System Variables & Functions
const int MAX_PARTICLES = 150;
Particle particles[MAX_PARTICLES] = { 0 };

void SpawnExplosion(Vector3 pos, Color col, int count) {
    int spawned = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = true;
            particles[i].position = pos;
            
            float angle = ((float)rand() / RAND_MAX) * 2.0f * PI;
            float speed = 1.0f + ((float)rand() / RAND_MAX) * 4.0f;
            
            particles[i].velocity = { cosf(angle) * speed, ((float)rand() / RAND_MAX) * 3.0f - 0.5f, sinf(angle) * speed };
            particles[i].color = col;
            particles[i].size = 0.04f + ((float)rand() / RAND_MAX) * 0.08f;
            particles[i].life = 0.0f;
            particles[i].maxLife = 0.3f + ((float)rand() / RAND_MAX) * 0.4f;
            particles[i].alpha = 1.0f;
            
            spawned++;
            if (spawned >= count) break;
        }
    }
}

void UpdateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;
            particles[i].position.z += particles[i].velocity.z * dt;
            
            particles[i].life += dt;
            particles[i].alpha = 1.0f - (particles[i].life / particles[i].maxLife);
            
            if (particles[i].life >= particles[i].maxLife) {
                particles[i].active = false;
            }
        }
    }
}

void DrawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            DrawSphere(particles[i].position, particles[i].size, ColorAlpha(particles[i].color, particles[i].alpha));
        }
    }
}

// Behind-the-Back Player Class
class Player {
public:
    int lane;         // -1, 0, 1
    float x;          // Lerping visual position X
    float y;          // Jump height Y
    float z;          // Fixed Z position
    float vy;         // Vertical velocity
    bool isJumping;
    bool isSliding;
    float slideTimer;
    bool isDead;
    float animTimer;

    // Power-up durations
    float shieldTime;
    float magnetTime;
    float boostTime;

    Player() {
        lane = 0;
        x = 0.0f;
        y = 0.0f;
        z = PLAYER_Z_POSITION;
        vy = 0.0f;
        isJumping = false;
        isSliding = false;
        slideTimer = 0.0f;
        isDead = false;
        animTimer = 0.0f;
        shieldTime = 0.0f;
        magnetTime = 0.0f;
        boostTime = 0.0f;
    }

    void ShiftLeft(Sound laneSound) {
        if (isDead || boostTime > 0.0f) return;
        if (lane > -1) {
            lane--;
            PlaySound(laneSound);
            SpawnExplosion({ x, y + 0.5f, z }, Color{ 20, 184, 166, 255 }, 4);
        }
    }

    void ShiftRight(Sound laneSound) {
        if (isDead || boostTime > 0.0f) return;
        if (lane < 1) {
            lane++;
            PlaySound(laneSound);
            SpawnExplosion({ x, y + 0.5f, z }, Color{ 20, 184, 166, 255 }, 4);
        }
    }

    void Jump(Sound jumpSound) {
        if (isDead || boostTime > 0.0f || isJumping) return;
        isJumping = true;
        isSliding = false;
        vy = 10.0f;
        PlaySound(jumpSound);
        SpawnExplosion({ x, 0.0f, z }, Color{ 255, 255, 255, 100 }, 6);
    }

    void Slide(Sound slideSound) {
        if (isDead || boostTime > 0.0f || isJumping) return;
        isSliding = true;
        slideTimer = 0.5f; // 0.5 seconds
        PlaySound(slideSound);
    }

    void Update(float dt) {
        if (isDead) {
            vy -= GRAVITY * dt;
            y += vy * dt;
            if (y < 0.0f) {
                y = 0.0f;
                vy = 0.0f;
            }
            animTimer += 4.0f * dt;
            return;
        }

        // Frame-rate independent lateral lane interpolation
        float targetX = lane * 3.5f;
        x += (targetX - x) * (1.0f - expf(-12.0f * dt));

        // Powerup Timers
        if (shieldTime > 0.0f) shieldTime = fmaxf(0.0f, shieldTime - dt);
        if (magnetTime > 0.0f) magnetTime = fmaxf(0.0f, magnetTime - dt);
        if (boostTime > 0.0f) {
            boostTime = fmaxf(0.0f, boostTime - dt);
            lane = 0; // Lock to middle
            // Spawn boost sparks
            if (rand() % 4 == 0) {
                SpawnExplosion({ x, y + 0.5f, z - 0.5f }, RED, 1);
            }
        }

        // Slide logic
        if (isSliding) {
            slideTimer -= dt;
            if (slideTimer <= 0.0f) {
                isSliding = false;
            } else {
                if (rand() % 5 == 0) {
                    SpawnExplosion({ x, 0.0f, z - 0.2f }, YELLOW, 1);
                }
            }
        }

        // Jump logic
        if (isJumping) {
            vy -= GRAVITY * dt;
            y += vy * dt;
            if (y <= 0.0f) {
                y = 0.0f;
                isJumping = false;
                vy = 0.0f;
                SpawnExplosion({ x, 0.0f, z }, Color{ 255, 255, 255, 80 }, 4);
            }
        }

        animTimer += dt;
    }
};

// 3D Skeletal Stickman Drawing Algorithm
void DrawStickman(Vector3 pos, float animTimer, bool isJumping, bool isSliding, bool isDead, float boostTime) {
    Color stickColor = (boostTime > 0.0f) ? RED : WHITE;
    float headRadius = 0.35f;
    float limbThickness = 0.07f;
    
    Vector3 head, neck, hip;
    Vector3 leftHand, leftElbow, leftShoulder;
    Vector3 rightHand, rightElbow, rightShoulder;
    Vector3 leftFoot, leftKnee;
    Vector3 rightFoot, rightKnee;

    if (isDead) {
        // Rotated flailing body
        head = { pos.x, pos.y + 1.6f, pos.z };
        neck = { pos.x, pos.y + 1.2f, pos.z };
        hip = { pos.x, pos.y + 0.5f, pos.z };
        
        DrawSphere(head, headRadius, stickColor);
        DrawCylinderEx(neck, hip, limbThickness, limbThickness, 6, stickColor);
        
        DrawCylinderEx(neck, { pos.x - 0.6f, pos.y + 1.4f, pos.z }, limbThickness, limbThickness, 6, stickColor);
        DrawCylinderEx(neck, { pos.x + 0.6f, pos.y + 1.0f, pos.z }, limbThickness, limbThickness, 6, stickColor);
        DrawCylinderEx(hip, { pos.x - 0.4f, pos.y + 0.2f, pos.z - 0.3f }, limbThickness, limbThickness, 6, stickColor);
        DrawCylinderEx(hip, { pos.x + 0.4f, pos.y + 0.1f, pos.z + 0.3f }, limbThickness, limbThickness, 6, stickColor);
        return;
    }

    if (isSliding) {
        hip = { pos.x, pos.y + 0.3f, pos.z };
        neck = { pos.x, pos.y + 0.8f, pos.z };
        head = { pos.x, pos.y + 1.15f, pos.z };
        
        leftFoot = { pos.x - 0.4f, pos.y, pos.z - 0.3f };
        rightFoot = { pos.x + 0.4f, pos.y, pos.z - 0.3f };
        leftKnee = { pos.x - 0.3f, pos.y + 0.15f, pos.z - 0.15f };
        rightKnee = { pos.x + 0.3f, pos.y + 0.15f, pos.z - 0.15f };

        leftHand = { pos.x - 0.5f, pos.y + 0.4f, pos.z + 0.1f };
        rightHand = { pos.x + 0.5f, pos.y + 0.4f, pos.z + 0.1f };
    }
    else if (isJumping) {
        hip = { pos.x, pos.y + 0.8f, pos.z };
        neck = { pos.x, pos.y + 1.6f, pos.z };
        head = { pos.x, pos.y + 1.95f, pos.z };

        leftFoot = { pos.x - 0.2f, pos.y + 0.1f, pos.z - 0.1f };
        rightFoot = { pos.x + 0.2f, pos.y + 0.1f, pos.z - 0.1f };
        leftKnee = { pos.x - 0.25f, pos.y + 0.45f, pos.z };
        rightKnee = { pos.x + 0.25f, pos.y + 0.45f, pos.z };

        leftHand = { pos.x - 0.5f, pos.y + 2.1f, pos.z };
        rightHand = { pos.x + 0.5f, pos.y + 2.1f, pos.z };
    }
    else {
        // Run cycle
        float bob = sinf(animTimer * 14.0f) * 0.06f;
        hip = { pos.x, pos.y + 0.75f + bob, pos.z };
        neck = { pos.x, pos.y + 1.55f + bob, pos.z };
        head = { pos.x, pos.y + 1.9f + bob, pos.z };

        float swingL = sinf(animTimer * 14.0f);
        float swingR = sinf(animTimer * 14.0f + PI);

        leftKnee = { pos.x - 0.2f, pos.y + 0.4f, pos.z + swingL * 0.25f };
        leftFoot = { pos.x - 0.2f, pos.y + fmaxf(0.0f, -swingL * 0.15f), pos.z + swingL * 0.5f };
        
        rightKnee = { pos.x + 0.2f, pos.y + 0.4f, pos.z + swingR * 0.25f };
        rightFoot = { pos.x + 0.2f, pos.y + fmaxf(0.0f, -swingR * 0.15f), pos.z + swingR * 0.5f };

        leftHand = { pos.x - 0.4f, pos.y + 1.0f, pos.z - swingL * 0.35f };
        rightHand = { pos.x + 0.4f, pos.y + 1.0f, pos.z - swingR * 0.35f };
    }

    // Render skeleton
    DrawSphere(head, headRadius, stickColor);
    DrawCylinderEx(neck, hip, limbThickness, limbThickness, 6, stickColor);
    DrawCylinderEx(hip, leftKnee, limbThickness, limbThickness, 6, stickColor);
    DrawCylinderEx(leftKnee, leftFoot, limbThickness, limbThickness, 6, stickColor);
    DrawCylinderEx(hip, rightKnee, limbThickness, limbThickness, 6, stickColor);
    DrawCylinderEx(rightKnee, rightFoot, limbThickness, limbThickness, 6, stickColor);
    DrawCylinderEx(neck, leftHand, limbThickness, limbThickness, 6, stickColor);
    DrawCylinderEx(neck, rightHand, limbThickness, limbThickness, 6, stickColor);
}

// 3D Road Rendering function
void DrawRoad(float roadOffset) {
    float segLength = 8.0f;
    float startZ = -15.0f;
    float endZ = 160.0f;
    
    for (float z = endZ; z >= startZ; z -= segLength) {
        float zCurr = z - fmodf(roadOffset, segLength);
        float zNext = zCurr - segLength;
        
        int index = (int)((zCurr + roadOffset) / segLength);
        Color roadColor = (index % 2 == 0) ? Color{ 17, 22, 37, 255 } : Color{ 11, 14, 24, 255 };
        
        // Base road (width 10.0f)
        DrawCube({ 0.0f, -0.05f, (zCurr + zNext) / 2.0f }, 10.0f, 0.1f, segLength, roadColor);
        
        // Temple edge borders
        Color borderGlow = (index % 2 == 0) ? Color{ 20, 184, 166, 255 } : Color{ 13, 148, 136, 255 };
        DrawCube({ -5.0f, 0.0f, (zCurr + zNext) / 2.0f }, 0.2f, 0.1f, segLength, borderGlow);
        DrawCube({ 5.0f, 0.0f, (zCurr + zNext) / 2.0f }, 0.2f, 0.1f, segLength, borderGlow);
        
        // Lane Divider lines
        if (index % 2 == 0) {
            DrawCube({ -1.75f, 0.0f, (zCurr + zNext) / 2.0f }, 0.06f, 0.06f, segLength / 2.0f, Color{ 255, 255, 255, 45 });
            DrawCube({ 1.75f, 0.0f, (zCurr + zNext) / 2.0f }, 0.06f, 0.06f, segLength / 2.0f, Color{ 255, 255, 255, 45 });
        }
    }
}

// 3D Pillar Drawing function
void DrawPillar(Vector3 pos) {
    DrawCube({ pos.x, 2.0f, pos.z }, 0.6f, 4.0f, 0.6f, Color{ 16, 12, 38, 255 });
    DrawCube({ pos.x, 4.1f, pos.z }, 0.9f, 0.2f, 0.9f, Color{ 30, 22, 69, 255 });
    DrawCube({ pos.x, 0.1f, pos.z }, 0.8f, 0.2f, 0.8f, Color{ 30, 22, 69, 255 });
    
    // Runic light line
    float offset = (pos.x > 0) ? -0.31f : 0.31f;
    DrawCube({ pos.x + offset, 2.0f, pos.z }, 0.04f, 3.2f, 0.08f, Color{ 20, 184, 166, 255 });
}

// Obstacle Drawing helper
void DrawObstacle(Obstacle &ob) {
    float xOffset = ob.lane * 3.5f;
    if (ob.type == OBSTACLE_ARCH || ob.type == OBSTACLE_PIT) xOffset = 0.0f;
    
    if (ob.type == OBSTACLE_BARRIER) {
        DrawCube({ xOffset, 0.5f, ob.z }, 2.8f, 0.7f, 0.25f, RED);
        DrawCube({ xOffset - 1.2f, 0.25f, ob.z }, 0.15f, 0.5f, 0.15f, Color{ 69, 10, 10, 255 });
        DrawCube({ xOffset + 1.2f, 0.25f, ob.z }, 0.15f, 0.5f, 0.15f, Color{ 69, 10, 10, 255 });
        
        // Caution marks
        DrawCube({ xOffset - 0.7f, 0.5f, ob.z + 0.13f }, 0.25f, 0.45f, 0.02f, YELLOW);
        DrawCube({ xOffset,        0.5f, ob.z + 0.13f }, 0.25f, 0.45f, 0.02f, YELLOW);
        DrawCube({ xOffset + 0.7f, 0.5f, ob.z + 0.13f }, 0.25f, 0.45f, 0.02f, YELLOW);
    }
    else if (ob.type == OBSTACLE_ARCH) {
        DrawCube({ -4.3f, 1.8f, ob.z }, 0.6f, 3.6f, 0.6f, Color{ 30, 41, 59, 255 });
        DrawCube({ 4.3f, 1.8f, ob.z }, 0.6f, 3.6f, 0.6f, Color{ 30, 41, 59, 255 });
        DrawCube({ 0.0f, 3.8f, ob.z }, 9.2f, 0.6f, 0.6f, Color{ 22, 30, 46, 255 });
        
        // Caution warnings
        DrawCube({ 0.0f, 3.8f, ob.z + 0.31f }, 2.5f, 0.2f, 0.02f, YELLOW);
    }
    else if (ob.type == OBSTACLE_PIT) {
        // Pit Fissure cut in road
        DrawCube({ 0.0f, -0.04f, ob.z }, 9.8f, 0.03f, 5.0f, Color{ 5, 3, 15, 255 });
        DrawCube({ 0.0f, 0.01f, ob.z - 2.5f }, 9.8f, 0.01f, 0.08f, RED);
        DrawCube({ 0.0f, 0.01f, ob.z + 2.5f }, 9.8f, 0.01f, 0.08f, RED);
    }
}

void DrawCoin(Coin &c, float time) {
    float xOffset = c.lane * 3.5f;
    float spin = time * 240.0f;
    
    // Rotating golden cylinder coin
    DrawCylinderEx({ xOffset, c.y, c.z - 0.04f }, { xOffset, c.y, c.z + 0.04f }, 0.32f, 0.32f, 10, GOLD);
}

void DrawPowerup(Powerup &p, float time) {
    float xOffset = p.lane * 3.5f;
    float bob = sinf(time * 6.0f) * 0.12f;
    Color glowColor = BLUE;
    if (p.type == POWERUP_MAGNET) glowColor = YELLOW;
    if (p.type == POWERUP_BOOST) glowColor = RED;
    
    DrawSphere({ xOffset, 1.0f + bob, p.z }, 0.4f, ColorAlpha(glowColor, 0.2f));
    DrawSphereWires({ xOffset, 1.0f + bob, p.z }, 0.4f, 8, 8, glowColor);
}

// Bounding box collision checking
void CheckCollisions(Player &player, std::vector<Obstacle> &obstacles, std::vector<Coin> &coins, std::vector<Powerup> &powerups,
                     Sound coinSound, Sound powerupSound, Sound shieldPopSound, Sound crashSound, int &coinCount, float &shakeTime, bool &isGameOver) {
    
    // 1. Coins collision
    for (auto &c : coins) {
        if (c.collected || !c.active) continue;
        float xOffset = c.lane * 3.5f;
        
        if (player.magnetTime > 0.0f) {
            float dx = player.x - xOffset;
            float dy = (player.y + 0.8f) - c.y;
            float dz = player.z - c.z;
            float dist = sqrtf(dx*dx + dy*dy + dz*dz);
            if (dist < 25.0f) {
                float pull = 30.0f * GetFrameTime();
                c.z += (dz / dist) * pull;
                c.y += (dy / dist) * pull;
                xOffset += (dx / dist) * pull;
                c.lane = (int)roundf(xOffset / 3.5f);
            }
        }
        
        if (fabsf(c.z - player.z) < 1.4f) {
            if (fabsf(xOffset - player.x) < 1.4f && fabsf(c.y - player.y) < 1.6f) {
                c.collected = true;
                coinCount++;
                PlaySound(coinSound);
                SpawnExplosion({ xOffset, c.y, c.z }, GOLD, 5);
            }
        }
    }

    // 2. Powerups collision
    for (auto &p : powerups) {
        if (p.collected || !p.active) continue;
        float xOffset = p.lane * 3.5f;
        if (fabsf(p.z - player.z) < 1.5f) {
            if (fabsf(xOffset - player.x) < 1.5f && fabsf(1.0f - player.y) < 1.5f) {
                p.collected = true;
                PlaySound(powerupSound);
                SpawnExplosion({ xOffset, 1.0f, p.z }, WHITE, 8);
                
                if (p.type == POWERUP_SHIELD) player.shieldTime = 8.0f;
                else if (p.type == POWERUP_MAGNET) player.magnetTime = 10.0f;
                else if (p.type == POWERUP_BOOST) {
                    player.boostTime = 5.0f;
                    shakeTime = 0.6f;
                }
            }
        }
    }

    // 3. Obstacles collision (anti-tunneling range checks)
    for (auto &o : obstacles) {
        if (o.passed || !o.active || isGameOver) continue;
        
        // Z overlap check
        if (o.z <= player.z + 0.8f && o.z >= player.z - 0.8f) {
            bool collides = false;
            float xOffset = o.lane * 3.5f;
            
            if (o.type == OBSTACLE_ARCH) {
                if (!player.isSliding) {
                    collides = true;
                }
            }
            else if (o.type == OBSTACLE_PIT) {
                if (!player.isJumping) {
                    collides = true;
                }
            }
            else {
                // Single lane check visual lateral overlap
                if (fabsf(player.x - xOffset) < 1.5f) {
                    if (!player.isJumping) {
                        collides = true;
                    }
                }
            }
            
            if (collides) {
                if (player.boostTime > 0.0f) {
                    o.passed = true;
                    SpawnExplosion({ xOffset, 0.6f, o.z }, RED, 12);
                    continue;
                }
                
                if (player.shieldTime > 0.0f && o.type != OBSTACLE_PIT) {
                    player.shieldTime = 0.0f;
                    o.passed = true;
                    PlaySound(shieldPopSound);
                    shakeTime = 0.3f;
                    SpawnExplosion({ xOffset, 0.6f, o.z }, BLUE, 12);
                    continue;
                }
                
                // Game Over Trigger
                isGameOver = true;
                player.isDead = true;
                player.vy = 8.0f; // launch up spin
                PlaySound(crashSound);
                shakeTime = 0.8f;
            }
        }
    }
}

// Spawner logic
void SpawnPattern(std::vector<Obstacle> &obstacles, std::vector<Coin> &coins, std::vector<Powerup> &powerups, float distance) {
    int lane = rand() % 3 - 1;
    ObstacleType type = OBSTACLE_BARRIER;
    if (distance > 150.0f) {
        int r = rand() % 3;
        if (r == 0) type = OBSTACLE_BARRIER;
        else if (r == 1) type = OBSTACLE_PIT;
        else type = OBSTACLE_ARCH;
    }
    
    Obstacle ob = { 0 };
    ob.lane = lane;
    ob.type = type;
    ob.z = MAX_Z;
    ob.passed = false;
    ob.active = true;
    obstacles.push_back(ob);
    
    // Coins spawning pattern
    int coinLane = (type == OBSTACLE_ARCH) ? (rand() % 3 - 1) : lane;
    for (int i = 0; i < 4; i++) {
        Coin c = { 0 };
        c.lane = coinLane;
        c.z = MAX_Z + 12.0f + i * 3.5f;
        c.y = (type == OBSTACLE_PIT) ? (1.5f + sinf((PI / 3.0f) * i) * 1.3f) : 0.4f;
        c.collected = false;
        c.active = true;
        coins.push_back(c);
    }
    
    // Rare Powerup Spawning
    if (rand() % 10 == 0) {
        Powerup p = { 0 };
        p.lane = rand() % 3 - 1;
        p.z = MAX_Z + 30.0f;
        int pt = rand() % 3;
        if (pt == 0) p.type = POWERUP_SHIELD;
        else if (pt == 1) p.type = POWERUP_MAGNET;
        else p.type = POWERUP_BOOST;
        p.collected = false;
        p.active = true;
        powerups.push_back(p);
    }
}

// Main Program Entry
int main(void) {
    srand(time(NULL));
    
    // Initialize Window
    InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Stickman Temple Run 3D - Raylib");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Programmatic Synth Audio Initialization
    Sound coinSound = GenerateCoinSound();
    Sound jumpSound = GenerateJumpSound();
    Sound crashSound = GenerateCrashSound();
    Sound slideSound = GenerateSlideSound();
    Sound laneSound = GenerateLaneSound();
    Sound shieldPopSound = GenerateShieldPopSound();
    Sound powerupSound = GeneratePowerupSound();

    // 3D Perspective Camera Setup
    Camera camera = { 0 };
    camera.position = { 0.0f, 3.8f, -7.5f };
    camera.target = { 0.0f, 1.4f, 10.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 55.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Game variables
    Player player;
    std::vector<Obstacle> obstacles;
    std::vector<Coin> coins;
    std::vector<Powerup> powerups;
    std::vector<SidePillar> pillars;
    
    // Init side pillars
    for (float z = 10.0f; z <= MAX_Z; z += 30.0f) {
        pillars.push_back({ -7.0f, z });
        pillars.push_back({ 7.0f, z + 15.0f });
    }

    float gameSpeed = BASE_SPEED;
    float distance = 0.0f;
    int coinCount = 0;
    int highScore = 0;
    
    // Retrieve highscore from storage if possible
    FILE *saveFile = fopen("highscore.dat", "rb");
    if (saveFile != NULL) {
        fread(&highScore, sizeof(int), 1, saveFile);
        fclose(saveFile);
    }

    // Game Screens
    bool showMenu = true;
    bool showHowToPlay = false;
    bool isGameOver = false;
    bool newHighScore = false;
    
    float roadOffset = 0.0f;
    float obstacleTimer = 0.0f;
    float minObstacleSpacing = 35.0f;
    float shakeTime = 0.0f;
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f; // cap lag spike leap
        
        // Updates logic
        if (showMenu) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                showMenu = false;
                player = Player();
                obstacles.clear();
                coins.clear();
                powerups.clear();
                gameSpeed = BASE_SPEED;
                distance = 0.0f;
                coinCount = 0;
                isGameOver = false;
                newHighScore = false;
                roadOffset = 0.0f;
                obstacleTimer = 0.0f;
            }
            if (IsKeyPressed(KEY_H)) {
                showHowToPlay = true;
                showMenu = false;
            }
        }
        else if (showHowToPlay) {
            if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_ESCAPE)) {
                showHowToPlay = false;
                showMenu = true;
            }
        }
        else if (isGameOver) {
            player.Update(dt);
            UpdateParticles(dt);
            
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                player = Player();
                obstacles.clear();
                coins.clear();
                powerups.clear();
                gameSpeed = BASE_SPEED;
                distance = 0.0f;
                coinCount = 0;
                isGameOver = false;
                newHighScore = false;
                roadOffset = 0.0f;
                obstacleTimer = 0.0f;
            }
            if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_ESCAPE)) {
                isGameOver = false;
                showMenu = true;
            }
        }
        else {
            // Running loop
            if (shakeTime > 0.0f) shakeTime -= dt;

            // Speed Acceleration
            float activeSpeed = gameSpeed;
            if (player.boostTime > 0.0f) {
                activeSpeed = gameSpeed * 2.2f;
            } else {
                gameSpeed += SPEED_ACCEL * dt * 60.0f;
            }
            
            distance += activeSpeed * 0.015f * dt * 60.0f;
            roadOffset += activeSpeed * dt;
            
            // Move Side Pillars
            for (auto &p : pillars) {
                p.z -= activeSpeed * dt;
                if (p.z < -10.0f) p.z += MAX_Z;
            }
            
            // Spawner
            obstacleTimer += activeSpeed * dt;
            if (obstacleTimer > minObstacleSpacing) {
                if (rand() % 100 < 6) { // ~6% chance
                    SpawnPattern(obstacles, coins, powerups, distance);
                    obstacleTimer = 0.0f;
                }
            }

            // Move elements
            for (auto &o : obstacles) o.z -= activeSpeed * dt;
            for (auto &c : coins) c.update(activeSpeed * dt, dt, player);
            for (auto &p : powerups) p.update(activeSpeed * dt);
            
            player.Update(dt);
            UpdateParticles(dt);
            
            // Collision check
            CheckCollisions(player, obstacles, coins, powerups, coinSound, powerupSound, shieldPopSound, crashSound, coinCount, shakeTime, isGameOver);
            
            if (isGameOver) {
                int totalScore = (int)distance + coinCount * 12;
                if (totalScore > highScore) {
                    highScore = totalScore;
                    newHighScore = true;
                    // Write to file
                    FILE *saveFile = fopen("highscore.dat", "wb");
                    if (saveFile != NULL) {
                        fwrite(&highScore, sizeof(int), 1, saveFile);
                        fclose(saveFile);
                    }
                }
            }

            // Cleanup off-screen items
            for (auto it = obstacles.begin(); it != obstacles.end();) {
                if (it->z < -10.0f || it->passed) it = obstacles.erase(it);
                else ++it;
            }
            for (auto it = coins.begin(); it != coins.end();) {
                if (it->z < -10.0f || it->collected) it = coins.erase(it);
                else ++it;
            }
            for (auto it = powerups.begin(); it != powerups.end();) {
                if (it->z < -10.0f || it->collected) it = powerups.erase(it);
                else ++it;
            }

            // Player keyboard input
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) player.ShiftLeft(laneSound);
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) player.ShiftRight(laneSound);
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) player.Jump(jumpSound);
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) player.Slide(slideSound);
        }

        // Render Graphics Drawing
        BeginDrawing();
        ClearBackground(GetColor(0x030712FF));

        // 3D Rendering Pass
        if (!showHowToPlay) {
            // Apply camera shake translation
            Camera cameraShake = camera;
            if (shakeTime > 0.0f) {
                cameraShake.position.x += ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
                cameraShake.position.y += ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
            }
            
            BeginMode3D(cameraShake);
                
                // Draw 3D road
                DrawRoad(roadOffset);
                
                // Draw Side Pillars
                for (auto &p : pillars) {
                    DrawPillar({ p.x, 0.0f, p.z });
                }
                
                // Draw Obstacles
                for (auto &o : obstacles) {
                    DrawObstacle(o);
                }
                
                // Draw Coins
                for (auto &c : coins) {
                    DrawCoin(c, (float)GetTime());
                }
                
                // Draw Powerups
                for (auto &p : powerups) {
                    DrawPowerup(p, (float)GetTime());
                }
                
                // Draw Particles
                DrawParticles();
                
                // Draw Player stickman
                DrawStickman({ player.x, player.y, player.z }, player.animTimer, player.isJumping, player.isSliding, player.isDead, player.boostTime);

                // Draw powerup auras
                if (player.boostTime > 0.0f) {
                    DrawSphere({ player.x, player.y + 0.8f, player.z }, 1.0f, ColorAlpha(RED, 0.15f));
                } else if (player.shieldTime > 0.0f) {
                    DrawSphereWires({ player.x, player.y + 0.8f, player.z }, 0.9f, 8, 8, ColorAlpha(BLUE, 0.4f));
                }

            EndMode3D();
        }

        // 2D Interface Overlay Pass
        if (showMenu) {
            // Darken background
            DrawRectangle(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, ColorAlpha(BLACK, 0.5f));
            
            // Glass panel effect menu
            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 240, CANVAS_HEIGHT/2.0f - 210, 480, 420 }, 0.08f, 4, Color{ 10, 15, 30, 220 });
            DrawRectangleRoundedLines({ CANVAS_WIDTH/2.0f - 240, CANVAS_HEIGHT/2.0f - 210, 480, 420 }, 0.08f, 4, 1.5f, Color{ 20, 184, 166, 120 });
            
            // Text Header
            DrawText("TEMPLE RUN", CANVAS_WIDTH/2 - MeasureText("TEMPLE RUN", 40)/2, CANVAS_HEIGHT/2 - 160, 40, Color{ 20, 184, 166, 255 });
            DrawText("STICKMAN RUNNER 3D", CANVAS_WIDTH/2 - MeasureText("STICKMAN RUNNER 3D", 16)/2, CANVAS_HEIGHT/2 - 110, 16, GOLD);
            
            // Character visual box preview
            DrawRectangle(CANVAS_WIDTH/2 - 40, CANVAS_HEIGHT/2 - 70, 80, 80, Color{ 5, 8, 16, 255 });
            DrawRectangleLines(CANVAS_WIDTH/2 - 40, CANVAS_HEIGHT/2 - 70, 80, 80, Color{ 20, 184, 166, 60 });
            
            float bob = sinf(GetTime() * 4.0f) * 4.0f;
            DrawCircle(CANVAS_WIDTH/2, CANVAS_HEIGHT/2 - 30 + bob, 15, ColorAlpha(Color{ 45, 212, 191, 255 }, 0.6f + bob*0.05f));
            
            // Buttons UI guidance
            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 140, CANVAS_HEIGHT/2.0f + 30, 280, 45 }, 0.2f, 4, Color{ 20, 184, 166, 255 });
            DrawText("PRESS ENTER / SPACE TO RUN", CANVAS_WIDTH/2 - MeasureText("PRESS ENTER / SPACE TO RUN", 15)/2, CANVAS_HEIGHT/2 + 45, 15, WHITE);
            
            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 140, CANVAS_HEIGHT/2.0f + 90, 280, 45 }, 0.2f, 4, Color{ 255, 255, 255, 15 });
            DrawRectangleRoundedLines({ CANVAS_WIDTH/2.0f - 140, CANVAS_HEIGHT/2.0f + 90, 280, 45 }, 0.2f, 4, 1.0f, Color{ 255, 255, 255, 40 });
            DrawText("PRESS 'H' FOR HOW TO PLAY", CANVAS_WIDTH/2 - MeasureText("PRESS 'H' FOR HOW TO PLAY", 15)/2, CANVAS_HEIGHT/2 + 105, 15, Color{ 229, 231, 235, 255 });
            
            std::string hsStr = "BEST RUN: " + std::to_string(highScore) + " PTS";
            DrawText(hsStr.c_str(), CANVAS_WIDTH/2 - MeasureText(hsStr.c_str(), 14)/2, CANVAS_HEIGHT/2 + 170, 14, GRAY);
        }
        else if (showHowToPlay) {
            DrawRectangle(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, ColorAlpha(BLACK, 0.6f));
            
            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 260, CANVAS_HEIGHT/2.0f - 220, 520, 440 }, 0.06f, 4, Color{ 10, 15, 30, 230 });
            DrawRectangleRoundedLines({ CANVAS_WIDTH/2.0f - 260, CANVAS_HEIGHT/2.0f - 220, 520, 440 }, 0.06f, 4, 1.5f, GOLD);
            
            DrawText("HOW TO ESCAPE", CANVAS_WIDTH/2 - MeasureText("HOW TO ESCAPE", 28)/2, CANVAS_HEIGHT/2 - 180, 28, GOLD);
            
            // Instruction items
            DrawRectangle(CANVAS_WIDTH/2 - 230, CANVAS_HEIGHT/2 - 120, 60, 30, Color{ 20, 184, 166, 40 });
            DrawRectangleLines(CANVAS_WIDTH/2 - 230, CANVAS_HEIGHT/2 - 120, 60, 30, Color{ 20, 184, 166, 120 });
            DrawText("A / D", CANVAS_WIDTH/2 - 215, CANVAS_HEIGHT/2 - 113, 14, Color{ 45, 212, 191, 255 });
            DrawText("Switch lanes left/right to dodge barriers", CANVAS_WIDTH/2 - 150, CANVAS_HEIGHT/2 - 113, 14, WHITE);

            DrawRectangle(CANVAS_WIDTH/2 - 230, CANVAS_HEIGHT/2 - 70, 60, 30, Color{ 20, 184, 166, 40 });
            DrawRectangleLines(CANVAS_WIDTH/2 - 230, CANVAS_HEIGHT/2 - 70, 60, 30, Color{ 20, 184, 166, 120 });
            DrawText("SPACE/W", CANVAS_WIDTH/2 - 228, CANVAS_HEIGHT/2 - 63, 12, Color{ 45, 212, 191, 255 });
            DrawText("Jump over ground barriers and deep pit gaps", CANVAS_WIDTH/2 - 150, CANVAS_HEIGHT/2 - 63, 14, WHITE);

            DrawRectangle(CANVAS_WIDTH/2 - 230, CANVAS_HEIGHT/2 - 20, 60, 30, Color{ 20, 184, 166, 40 });
            DrawRectangleLines(CANVAS_WIDTH/2 - 230, CANVAS_HEIGHT/2 - 20, 60, 30, Color{ 20, 184, 166, 120 });
            DrawText("S", CANVAS_WIDTH/2 - 205, CANVAS_HEIGHT/2 - 13, 14, Color{ 45, 212, 191, 255 });
            DrawText("Slide and duck beneath high stone arches", CANVAS_WIDTH/2 - 150, CANVAS_HEIGHT/2 - 13, 14, WHITE);

            // Power-up descriptions
            DrawText("TEMPLE POWERUPS", CANVAS_WIDTH/2 - MeasureText("TEMPLE POWERUPS", 16)/2, CANVAS_HEIGHT/2 + 40, 16, GOLD);
            DrawText("🛡️ Shield: Absorbs 1 crash   🧲 Magnet: Pulls gold coins", CANVAS_WIDTH/2 - MeasureText("🛡️ Shield: Absorbs 1 crash   🧲 Magnet: Pulls gold coins", 13)/2, CANVAS_HEIGHT/2 + 75, 13, Color{ 209, 213, 219, 255 });
            DrawText("⚡ Boost: Temporary hyper-speed invincibility run", CANVAS_WIDTH/2 - MeasureText("⚡ Boost: Temporary hyper-speed invincibility run", 13)/2, CANVAS_HEIGHT/2 + 105, 13, Color{ 209, 213, 219, 255 });

            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 140, CANVAS_HEIGHT/2.0f + 145, 280, 45 }, 0.2f, 4, Color{ 255, 255, 255, 15 });
            DrawRectangleRoundedLines({ CANVAS_WIDTH/2.0f - 140, CANVAS_HEIGHT/2.0f + 145, 280, 45 }, 0.2f, 4, 1.0f, Color{ 255, 255, 255, 40 });
            DrawText("PRESS 'ESC' OR 'B' FOR MENU", CANVAS_WIDTH/2 - MeasureText("PRESS 'ESC' OR 'B' FOR MENU", 14)/2, CANVAS_HEIGHT/2 + 160, 14, WHITE);
        }
        else if (isGameOver) {
            DrawRectangle(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, ColorAlpha(Color{ 3, 7, 18, 255 }, 0.5f));
            
            // Game Over panel
            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 240, CANVAS_HEIGHT/2.0f - 210, 480, 420 }, 0.08f, 4, Color{ 10, 15, 30, 220 });
            DrawRectangleRoundedLines({ CANVAS_WIDTH/2.0f - 240, CANVAS_HEIGHT/2.0f - 210, 480, 420 }, 0.08f, 4, 1.5f, Color{ 244, 63, 94, 150 });
            
            DrawText("CRASHED!", CANVAS_WIDTH/2 - MeasureText("CRASHED!", 36)/2, CANVAS_HEIGHT/2 - 160, 36, Color{ 244, 63, 94, 255 });
            DrawText("The ancient temple spirits caught you...", CANVAS_WIDTH/2 - MeasureText("The ancient temple spirits caught you...", 14)/2, CANVAS_HEIGHT/2 - 110, 14, GRAY);
            
            // Results box
            DrawRectangle(CANVAS_WIDTH/2 - 200, CANVAS_HEIGHT/2 - 70, 400, 120, Color{ 5, 8, 16, 255 });
            DrawRectangleLines(CANVAS_WIDTH/2 - 200, CANVAS_HEIGHT/2 - 70, 400, 120, Color{ 255, 255, 255, 10 });
            
            std::string dStr = "Distance Run:       " + std::to_string((int)distance) + "m";
            DrawText(dStr.c_str(), CANVAS_WIDTH/2 - 170, CANVAS_HEIGHT/2 - 50, 16, WHITE);
            
            std::string cStr = "Coins Collected:    " + std::to_string(coinCount);
            DrawText(cStr.c_str(), CANVAS_WIDTH/2 - 170, CANVAS_HEIGHT/2 - 15, 16, WHITE);
            
            std::string tStr = "TOTAL SCORE:        " + std::to_string((int)distance + coinCount * 12) + " PTS";
            DrawText(tStr.c_str(), CANVAS_WIDTH/2 - 170, CANVAS_HEIGHT/2 + 20, 16, GOLD);
            
            if (newHighScore) {
                DrawText("✨ NEW HIGH SCORE! ✨", CANVAS_WIDTH/2 - MeasureText("✨ NEW HIGH SCORE! ✨", 16)/2, CANVAS_HEIGHT/2 + 70, 16, YELLOW);
            }
            
            // Restart button UI
            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 160, CANVAS_HEIGHT/2.0f + 105, 320, 40 }, 0.2f, 4, Color{ 20, 184, 166, 255 });
            DrawText("PRESS ENTER / SPACE TO PLAY AGAIN", CANVAS_WIDTH/2 - MeasureText("PRESS ENTER / SPACE TO PLAY AGAIN", 13)/2, CANVAS_HEIGHT/2 + 118, 13, WHITE);
            
            DrawRectangleRounded({ CANVAS_WIDTH/2.0f - 160, CANVAS_HEIGHT/2.0f + 155, 320, 40 }, 0.2f, 4, Color{ 255, 255, 255, 15 });
            DrawRectangleRoundedLines({ CANVAS_WIDTH/2.0f - 160, CANVAS_HEIGHT/2.0f + 155, 320, 40 }, 0.2f, 4, 1.0f, Color{ 255, 255, 255, 40 });
            DrawText("PRESS 'M' OR 'ESC' FOR MAIN MENU", CANVAS_WIDTH/2 - MeasureText("PRESS 'M' OR 'ESC' FOR MAIN MENU", 13)/2, CANVAS_HEIGHT/2 + 168, 13, Color{ 229, 231, 235, 255 });
        }
        else {
            // In-Game HUD overlay
            // Top Left Distance Box
            DrawRectangleRounded({ 20, 20, 160, 50 }, 0.2f, 4, Color{ 10, 15, 30, 180 });
            DrawRectangleRoundedLines({ 20, 20, 160, 50 }, 0.2f, 4, 1.0f, Color{ 20, 184, 166, 120 });
            DrawText("DISTANCE", 30, 26, 10, Color{ 156, 163, 175, 255 });
            std::string dStr = std::to_string((int)distance) + "m";
            DrawText(dStr.c_str(), 30, 38, 20, Color{ 45, 212, 191, 255 });

            // Top Left Coins Box
            DrawRectangleRounded({ 195, 20, 140, 50 }, 0.2f, 4, Color{ 10, 15, 30, 180 });
            DrawRectangleRoundedLines({ 195, 20, 140, 50 }, 0.2f, 4, 1.0f, Color{ 251, 191, 36, 120 });
            DrawText("COINS", 205, 26, 10, Color{ 156, 163, 175, 255 });
            std::string cStr = "🪙 " + std::to_string(coinCount);
            DrawText(cStr.c_str(), 205, 38, 20, GOLD);

            // Top Right Speed multiplier Box
            DrawRectangleRounded({ CANVAS_WIDTH - 150, 20, 130, 50 }, 0.2f, 4, Color{ 10, 15, 30, 180 });
            DrawRectangleRoundedLines({ CANVAS_WIDTH - 150, 20, 130, 50 }, 0.2f, 4, 1.0f, Color{ 244, 63, 94, 120 });
            DrawText("SPEED", CANVAS_WIDTH - 140, 26, 10, Color{ 156, 163, 175, 255 });
            float mult = gameSpeed / BASE_SPEED;
            if (player.boostTime > 0.0f) mult *= 2.2f;
            std::string mStr = std::to_string(mult).substr(0, 3) + "x";
            DrawText(mStr.c_str(), CANVAS_WIDTH - 140, 38, 20, Color{ 251, 113, 133, 255 });

            // Powerups HUD bottom left bar
            float startY = CANVAS_HEIGHT - 35;
            if (player.shieldTime > 0.0f) {
                DrawRectangleRounded({ 20, startY, 160, 20 }, 0.2f, 4, Color{ 10, 15, 30, 180 });
                DrawText("🛡️", 26, startY + 3, 12, WHITE);
                DrawRectangle(45, startY + 7, (int)((player.shieldTime / 8.0f) * 125), 6, BLUE);
                startY -= 28;
            }
            if (player.magnetTime > 0.0f) {
                DrawRectangleRounded({ 20, startY, 160, 20 }, 0.2f, 4, Color{ 10, 15, 30, 180 });
                DrawText("🧲", 26, startY + 3, 12, WHITE);
                DrawRectangle(45, startY + 7, (int)((player.magnetTime / 10.0f) * 125), 6, YELLOW);
                startY -= 28;
            }
            if (player.boostTime > 0.0f) {
                DrawRectangleRounded({ 20, startY, 160, 20 }, 0.2f, 4, Color{ 10, 15, 30, 180 });
                DrawText("⚡", 26, startY + 3, 12, WHITE);
                DrawRectangle(45, startY + 7, (int)((player.boostTime / 5.0f) * 125), 6, RED);
            }
        }

        EndDrawing();
    }

    // Unload program resources
    UnloadSound(coinSound);
    UnloadSound(jumpSound);
    UnloadSound(crashSound);
    UnloadSound(slideSound);
    UnloadSound(laneSound);
    UnloadSound(shieldPopSound);
    UnloadSound(powerupSound);

    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
