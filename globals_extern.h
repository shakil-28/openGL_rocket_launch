#ifndef GLOBALS_EXTERN_H
#define GLOBALS_EXTERN_H

#include "types_structures.h"
#include <vector>
#include <utility>

// ========== ROCKET VARIABLES ==========
extern float rocketX, rocketY;
extern float rocketVelocity;
extern float rocketAngle;
extern bool rocketLaunched;
extern bool rocketLaunching;
extern bool rocketExploded;
extern float rocketFuel;
extern int rocketStage;
extern int countdown;
extern float altitude;
extern float velocity;
extern float acceleration;

// Stage separation variables
extern RocketStage stage1;
extern RocketStage stage2;
extern RocketStage stage3;
extern bool stage1Separated;
extern bool stage2Separated;
extern bool stage1Falling;
extern bool stage2Falling;
extern float stage1FallTime;
extern float stage2FallTime;

// ========== CAMERA VARIABLES ==========
extern float cameraZoom;
extern float cameraX, cameraY;
extern bool cameraFollowRocket;
extern bool cameraShake;
extern float shakeIntensity;
extern CameraMode currentCameraMode;
extern float cameraTargetX, cameraTargetY;
extern float cameraTargetZoom;

// ========== ANIMATION VARIABLES ==========
extern bool lightOn;
extern float flameSize;
extern float flameIntensity;
extern int lightTimer;
extern float dayTime;
extern bool dayNightCycle;
extern float windSpeed;
extern float windDirection;

// ========== UI VARIABLES ==========
extern bool showStats;
extern bool showGrid;
extern bool showTrajectory;
extern std::vector<std::pair<float, float>> trajectoryPoints;

// ========== GAME STATE ==========
extern GameState currentState;

// ========== COLLECTIONS ==========
extern std::vector<Star> stars;
extern std::vector<SmokeParticle> smokeParticles;
extern std::vector<Cloud> clouds;
extern std::vector<ExplosionParticle> explosionParticles;
extern std::vector<Satellite> satellites;
extern std::vector<Meteor> meteors;

// ========== MISSION PARAMETERS ==========
extern float targetAltitude;
extern float targetVelocity;
extern bool missionSuccess;

// ========== KEYBOARD TRACKING ==========
extern bool keys[256];

#endif