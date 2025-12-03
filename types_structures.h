#ifndef TYPES_STRUCTURES_H
#define TYPES_STRUCTURES_H

#include <vector>
#include <utility>

// Basic structures
struct Star {
    float x, y;
    float brightness;
    float twinkleSpeed;
};

struct SmokeParticle {
    float x, y;
    float size;
    float speed;
    float life;
    float alpha;
    float r, g, b;
};

struct Cloud {
    float x, y;
    float speed;
    float size;
    float opacity;
};

struct ExplosionParticle {
    float x, y;
    float vx, vy;
    float size;
    float life;
    float r, g, b;
};

struct Satellite {
    float x, y;
    float speed;
    float size;
    float angle;
};

struct Meteor {
    float x, y;
    float speed;
    float size;
    float trailLength;
    std::vector<float> trailX, trailY;
};

struct RocketStage {
    float x, y;
    float velocity;
    float angle;
    float fuel;
    bool active;
    bool separated;
    bool falling;
    float separationTime;
    std::vector<SmokeParticle> debris;
    float width;
    float height;
};

// Enumerations
enum CameraMode { 
    MODE_FOLLOW_ROCKET, 
    MODE_FOLLOW_STAGE1, 
    MODE_FOLLOW_STAGE2, 
    MODE_LAUNCH_PAD, 
    MODE_WIDE_VIEW 
};

enum GameState { 
    PRELAUNCH, 
    COUNTDOWN, 
    LAUNCH, 
    ORBIT, 
    MISSION_COMPLETE 
};

#endif