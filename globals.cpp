#include "globals_extern.h"
#include "constants_config.h"

// ========== ROCKET VARIABLES ==========
float rocketX = INITIAL_ROCKET_X;
float rocketY = INITIAL_ROCKET_Y;
float rocketVelocity = 0.0f;
float rocketAngle = 0.0f;
bool rocketLaunched = false;
bool rocketLaunching = false;
bool rocketExploded = false;
float rocketFuel = INITIAL_ROCKET_FUEL;
int rocketStage = INITIAL_ROCKET_STAGE;
int countdown = INITIAL_COUNTDOWN;
float altitude = 0.0f;
float velocity = 0.0f;
float acceleration = 0.0f;

// Stage separation variables
RocketStage stage1;
RocketStage stage2;
RocketStage stage3;
bool stage1Separated = false;
bool stage2Separated = false;
bool stage1Falling = false;
bool stage2Falling = false;
float stage1FallTime = 0.0f;
float stage2FallTime = 0.0f;

// ========== CAMERA VARIABLES ==========
float cameraZoom = INITIAL_CAMERA_ZOOM;
float cameraX = INITIAL_CAMERA_X;
float cameraY = INITIAL_CAMERA_Y;
bool cameraFollowRocket = false;
bool cameraShake = false;
float shakeIntensity = 0.0f;
CameraMode currentCameraMode = MODE_LAUNCH_PAD;
float cameraTargetX = INITIAL_CAMERA_X;
float cameraTargetY = INITIAL_CAMERA_Y;
float cameraTargetZoom = INITIAL_CAMERA_ZOOM;

// ========== ANIMATION VARIABLES ==========
bool lightOn = true;
float flameSize = 30.0f;
float flameIntensity = 1.0f;
int lightTimer = 0;
float dayTime = INITIAL_DAY_TIME;
bool dayNightCycle = true;
float windSpeed = INITIAL_WIND_SPEED;
float windDirection = INITIAL_WIND_DIRECTION;

// ========== UI VARIABLES ==========
bool showStats = true;
bool showGrid = false;
bool showTrajectory = false;
std::vector<std::pair<float, float>> trajectoryPoints;

// ========== GAME STATE ==========
GameState currentState = PRELAUNCH;

// ========== COLLECTIONS ==========
std::vector<Star> stars;
std::vector<SmokeParticle> smokeParticles;
std::vector<Cloud> clouds;
std::vector<ExplosionParticle> explosionParticles;
std::vector<Satellite> satellites;
std::vector<Meteor> meteors;

// ========== MISSION PARAMETERS ==========
float targetAltitude = INITIAL_TARGET_ALTITUDE;
float targetVelocity = INITIAL_TARGET_VELOCITY;
bool missionSuccess = false;

// ========== KEYBOARD TRACKING ==========
bool keys[256] = {false};