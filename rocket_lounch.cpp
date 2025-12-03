#include <GLUT/glut.h>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

// ========== GLOBAL VARIABLES ==========
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
    float r, g, b; // Color variations
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
    float angle; // For rotation
};

struct Meteor {
    float x, y;
    float speed;
    float size;
    float trailLength;
    std::vector<float> trailX, trailY;
};

// Rocket stage structure
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

// Rocket properties
const float LAUNCH_PAD_X = 400.0f;
const float LAUNCH_PAD_Y = 150.0f;
float rocketX = LAUNCH_PAD_X;
float rocketY = LAUNCH_PAD_Y;
float rocketVelocity = 0.0f;
float rocketAngle = 0.0f;
bool rocketLaunched = false;
bool rocketLaunching = false;
bool rocketExploded = false;
float rocketFuel = 100.0f;
int rocketStage = 1;
int countdown = 10;
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

// Camera properties
float cameraZoom = 1.0f;
float cameraX = 400.0f;
float cameraY = 300.0f;
bool cameraFollowRocket = false;
bool cameraShake = false;
float shakeIntensity = 0.0f;
enum CameraMode { MODE_FOLLOW_ROCKET, MODE_FOLLOW_STAGE1, MODE_FOLLOW_STAGE2, MODE_LAUNCH_PAD, MODE_WIDE_VIEW };
CameraMode currentCameraMode = MODE_LAUNCH_PAD;
float cameraTargetX = 400.0f;
float cameraTargetY = 300.0f;
float cameraTargetZoom = 1.0f;

// Animation properties
bool lightOn = true;
float flameSize = 30.0f;
float flameIntensity = 1.0f;
int lightTimer = 0;
float dayTime = 0.5f; // 0.0 = night, 1.0 = day
bool dayNightCycle = true;
float windSpeed = 0.1f;
float windDirection = 1.0f;

// UI properties
bool showStats = true;
bool showGrid = false;
bool showTrajectory = false;
std::vector<std::pair<float, float>> trajectoryPoints;

// Game states
enum GameState { PRELAUNCH, COUNTDOWN, LAUNCH, ORBIT, MISSION_COMPLETE };
GameState currentState = PRELAUNCH;

// Collections
std::vector<Star> stars;
std::vector<SmokeParticle> smokeParticles;
std::vector<Cloud> clouds;
std::vector<ExplosionParticle> explosionParticles;
std::vector<Satellite> satellites;
std::vector<Meteor> meteors;

// Mission parameters
float targetAltitude = 800.0f;
float targetVelocity = 5.0f;
bool missionSuccess = false;


// Keyboard tracking
bool keys[256] = {false};

// ========== FUNCTION PROTOTYPES ==========
void drawSeparatedStage(const RocketStage& stage, int stageNum);
void drawFirstStage();
void drawSecondStage(float yOffset);
void drawThirdStage(float yOffset);
void drawText(float x, float y, const std::string& text);
void drawGauge(float x, float y, float value, float maxValue, const std::string& label, float r, float g, float b);
void initializeStages();
void createStageSeparationEffect(float x, float y, int stageNum);
void separateStage(int stageNum);
void updateSeparatedStages();
void drawFlamesForStage(int stageNum);
void switchCameraToRocket(int value);
void testSeparation();

// ========== DRAWING FUNCTIONS ==========
// Add this function to test separation manually
void testSeparation() {
    std::cout << "\n=== MANUAL SEPARATION TEST ===" << std::endl;
    std::cout << "Current Stage: " << rocketStage << std::endl;
    std::cout << "Altitude: " << altitude << "m" << std::endl;
    std::cout << "Fuel: " << rocketFuel << "%" << std::endl;
    std::cout << "Velocity: " << velocity << "m/s" << std::endl;
    std::cout << "Stage1 Separated: " << (stage1Separated ? "YES" : "NO") << std::endl;
    std::cout << "Stage2 Separated: " << (stage2Separated ? "YES" : "NO") << std::endl;
    
    if (rocketLaunching) {
        if (rocketStage == 1 && !stage1Separated) {
            std::cout << "Forcing Stage 1 separation..." << std::endl;
            separateStage(1);
        }
        else if (rocketStage == 2 && !stage2Separated) {
            std::cout << "Forcing Stage 2 separation..." << std::endl;
            separateStage(2);
        }
        else if (rocketStage == 3) {
            std::cout << "Already at final stage (Payload)" << std::endl;
        }
    } else {
        std::cout << "Rocket not launching yet!" << std::endl;
    }
}

// Add a timer callback function
void switchCameraToRocket(int value) {
    if (currentCameraMode == MODE_FOLLOW_STAGE1 || currentCameraMode == MODE_FOLLOW_STAGE2) {
        currentCameraMode = MODE_FOLLOW_ROCKET;
        std::cout << "Auto-switching camera back to rocket view" << std::endl;
    }
}

void drawFlamesForStage(int stageNum) {
    if (!rocketLaunching || rocketFuel <= 0) return;
    
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    
    if (stageNum == 1) {
        // Stage 1 flames - ORANGE/RED (strong)
        glPushMatrix();
        glTranslatef(rocketX, rocketY, 0);
        glRotatef(rocketAngle, 0, 0, 1);
        
        for(int layer = 0; layer < 3; layer++) {
            float layerSize = 40.0f * (1.0f - layer * 0.3f);
            float layerOffset = layer * 5.0f;
            
            if (layer == 0) glColor3f(1.0f, 0.4f, 0.0f); // Orange core
            else if (layer == 1) glColor3f(1.0f, 0.9f, 0.0f); // Yellow middle
            else glColor3f(1.0f, 1.0f, 1.0f); // White outer
            
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, -layerOffset);
            for(int i = 0; i <= 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                float wave = sin(time * 10.0f + angle * 5.0f) * 0.2f + 1.0f;
                float x = sin(angle) * 15 * wave;
                float y = -layerSize - cos(angle) * 15 * wave;
                glVertex2f(x, y);
            }
            glEnd();
        }
        glPopMatrix();
    }
    else if (stageNum == 2) {
        // Stage 2 flames - BLUE/WHITE
        glPushMatrix();
        glTranslatef(rocketX, rocketY, 0);
        glRotatef(rocketAngle, 0, 0, 1);
        
        for(int layer = 0; layer < 2; layer++) {
            float layerSize = 25.0f * (1.0f - layer * 0.3f);
            float layerOffset = layer * 3.0f;
            
            if (layer == 0) glColor3f(0.0f, 0.6f, 1.0f); // Blue core
            else glColor3f(0.8f, 0.9f, 1.0f); // Light blue outer
            
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, -layerOffset);
            for(int i = 0; i <= 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                float wave = sin(time * 12.0f + angle * 6.0f) * 0.2f + 1.0f;
                float x = sin(angle) * 10 * wave;
                float y = -layerSize - cos(angle) * 10 * wave;
                glVertex2f(x, y);
            }
            glEnd();
        }
        glPopMatrix();
    }
    else if (stageNum == 3) {
        // Stage 3 (PAYLOAD) flames - BRIGHT PURPLE (make it more visible!)
        glPushMatrix();
        glTranslatef(rocketX, rocketY, 0);
        glRotatef(rocketAngle, 0, 0, 1);
        
        // Make payload flame BRIGHTER and larger
        for(int layer = 0; layer < 2; layer++) {
            float layerSize = 18.0f * (1.0f - layer * 0.4f);
            float layerOffset = layer * 2.0f;
            
            if (layer == 0) {
                glColor3f(1.0f, 0.0f, 1.0f); // Bright magenta core
            } else {
                glColor3f(0.8f, 0.0f, 0.8f); // Purple outer
            }
            
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, -layerOffset);
            for(int i = 0; i <= 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                float wave = sin(time * 15.0f + angle * 7.0f) * 0.3f + 1.0f;
                float x = sin(angle) * 8 * wave;
                float y = -layerSize - cos(angle) * 8 * wave;
                glVertex2f(x, y);
            }
            glEnd();
        }
        
        // Add a glow effect around payload flame
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.0f, 1.0f, 0.3f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for(int i = 0; i <= 360; i += 30) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(12 * sin(angle), -25 - 12 * cos(angle));
        }
        glEnd();
        glDisable(GL_BLEND);
        
        glPopMatrix();
    }
}

void drawStars() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for(const auto& star : stars) {
        float brightness = star.brightness * (1.0f - dayTime * 0.8f);
        glColor3f(brightness, brightness, brightness);
        glVertex2f(star.x, star.y);
    }
    glEnd();
    
    // Draw constellations
    glColor3f(0.8f, 0.8f, 1.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // Big Dipper
    glVertex2f(100, 550); glVertex2f(120, 530);
    glVertex2f(120, 530); glVertex2f(140, 540);
    glVertex2f(140, 540); glVertex2f(160, 520);
    glVertex2f(160, 520); glVertex2f(180, 510);
    // Orion's Belt
    glVertex2f(600, 500); glVertex2f(620, 490);
    glVertex2f(620, 490); glVertex2f(640, 480);
    glEnd();
}

void drawSky() {
    glBegin(GL_QUADS);
    if (dayTime < 0.25f) { // Night - DARKER for better contrast
        glColor3f(0.0f, 0.02f, 0.1f);
        glVertex2f(0, 600);
        glVertex2f(800, 600);
        glColor3f(0.0f, 0.05f, 0.2f);
        glVertex2f(800, 0);
        glVertex2f(0, 0);
    }
    else if (dayTime < 0.5f) { // Dawn
        glColor3f(0.3f, 0.2f, 0.5f);
        glVertex2f(0, 600);
        glVertex2f(800, 600);
        glColor3f(0.8f, 0.4f, 0.2f);
        glVertex2f(800, 0);
        glVertex2f(0, 0);
    }
    else if (dayTime < 0.75f) { // Day - LIGHTER for better contrast
        glColor3f(0.5f, 0.7f, 1.0f);
        glVertex2f(0, 600);
        glVertex2f(800, 600);
        glColor3f(0.7f, 0.9f, 1.0f);
        glVertex2f(800, 0);
        glVertex2f(0, 0);
    }
    else { // Dusk
        glColor3f(0.8f, 0.3f, 0.2f);
        glVertex2f(0, 600);
        glVertex2f(800, 600);
        glColor3f(0.3f, 0.2f, 0.4f);
        glVertex2f(800, 0);
        glVertex2f(0, 0);
    }
    glEnd();
    
    // Draw sun/moon
    glPushMatrix();
    float sunX = 700;
    float sunY = 500;
    if (dayTime < 0.5f) { // Moon at night
        glColor3f(0.95f, 0.95f, 0.95f); // Brighter moon
        glTranslatef(sunX, sunY, 0);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i += 10) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(20 * cos(angle), 20 * sin(angle));
        }
        glEnd();
        // Moon craters
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i += 10) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(10 + 5 * cos(angle), 5 + 5 * sin(angle));
        }
        glEnd();
    } else { // Sun during day
        glColor3f(1.0f, 0.95f, 0.1f); // Brighter sun
        glTranslatef(sunX, sunY, 0);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i += 10) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(30 * cos(angle), 30 * sin(angle));
        }
        glEnd();
        // Sun rays
        glColor3f(1.0f, 0.9f, 0.2f);
        for(int i = 0; i < 360; i += 45) {
            float angle = i * 3.14159f / 180.0f;
            glBegin(GL_TRIANGLES);
            glVertex2f(25 * cos(angle), 25 * sin(angle));
            glVertex2f(25 * cos(angle + 0.2), 25 * sin(angle + 0.2));
            glVertex2f(50 * cos(angle + 0.1), 50 * sin(angle + 0.1));
            glEnd();
        }
    }
    glPopMatrix();
}

void drawGround() {
    // Terrain with hills
    glColor3f(0.15f, 0.5f, 0.15f); // Brighter green
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(800, 0);
    
    // Create hilly terrain
    for(int x = 0; x <= 800; x += 20) {
        float height = 120 + 30 * sin(x * 0.02f) + 20 * sin(x * 0.05f);
        glVertex2f(x, height);
    }
    glVertex2f(0, 120);
    glEnd();
    
    // Draw runway
    glColor3f(0.4f, 0.4f, 0.4f); // Brighter gray
    glBegin(GL_QUADS);
    glVertex2f(200, 100);
    glVertex2f(600, 100);
    glVertex2f(600, 110);
    glVertex2f(200, 110);
    glEnd();
    
    // Runway markings
    glColor3f(1.0f, 1.0f, 1.0f);
    for(int i = 220; i < 580; i += 40) {
        glBegin(GL_QUADS);
        glVertex2f(i, 103);
        glVertex2f(i + 20, 103);
        glVertex2f(i + 20, 107);
        glVertex2f(i, 107);
        glEnd();
    }
}

void drawGrid() {
    if (!showGrid) return;
    
    glColor4f(0.4f, 0.4f, 0.4f, 0.4f); // Brighter grid
    glLineWidth(1.0f);
    
    // Vertical lines
    for(int x = 0; x <= 800; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, 0);
        glVertex2f(x, 600);
        glEnd();
    }
    
    // Horizontal lines
    for(int y = 0; y <= 600; y += 50) {
        glBegin(GL_LINES);
        glVertex2f(0, y);
        glVertex2f(800, y);
        glEnd();
    }
}

void drawLaunchPad() {
    // FIXED launch pad position - NOT relative to rocket position
    float launchPadX = 400.0f;  // Fixed position
    float launchPadY = 150.0f;  // Fixed position
    
    // ===== MAIN LAUNCH TABLE =====
    // Large octagonal launch table (like SpaceX's OLM)
    glColor3f(0.35f, 0.35f, 0.35f); // Concrete gray
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 45) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(launchPadX + 80 * cos(angle), launchPadY - 20 + 40 * sin(angle));
    }
    glEnd();
    
    // Launch table surface detail
    glColor3f(0.45f, 0.45f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for(int i = 0; i < 360; i += 45) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(launchPadX, launchPadY - 20);
        glVertex2f(launchPadX + 75 * cos(angle), launchPadY - 20 + 35 * sin(angle));
    }
    glEnd();
    
    // ===== FLAME DIVERTER/TRENCH =====
    // Realistic flame trench opening
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(launchPadX - 50, launchPadY - 80);
    glVertex2f(launchPadX + 50, launchPadY - 80);
    glVertex2f(launchPadX + 50, launchPadY - 20);
    glVertex2f(launchPadX - 50, launchPadY - 20);
    glEnd();
    
    // Water deluge system holes
    glColor3f(0.1f, 0.1f, 0.3f);
    for(int i = -4; i <= 4; i++) {
        if(i == 0) continue;
        float holeX = launchPadX + i * 10;
        glBegin(GL_POLYGON);
        for(int j = 0; j < 360; j += 30) {
            float angle = j * 3.14159f / 180.0f;
            glVertex2f(holeX + 3 * cos(angle), launchPadY - 30 + 3 * sin(angle));
        }
        glEnd();
    }
    
    // ===== LAUNCH MOUNT DETAILS =====
    // Cryogenic propellant lines (stainless steel)
    glColor3f(0.7f, 0.7f, 0.75f);
    for(int i = -1; i <= 1; i += 2) {
        // Main feed line
        glBegin(GL_QUADS);
        glVertex2f(launchPadX + i * 60, launchPadY - 80);
        glVertex2f(launchPadX + i * 65, launchPadY - 80);
        glVertex2f(launchPadX + i * 65, launchPadY + 50);
        glVertex2f(launchPadX + i * 60, launchPadY + 50);
        glEnd();
        
        // Support brackets
        for(int j = 0; j < 5; j++) {
            float bracketY = launchPadY - 60 + j * 25;
            glColor3f(0.5f, 0.5f, 0.5f);
            glBegin(GL_QUADS);
            glVertex2f(launchPadX + i * 65, bracketY);
            glVertex2f(launchPadX + i * 70, bracketY);
            glVertex2f(launchPadX + i * 70, bracketY + 3);
            glVertex2f(launchPadX + i * 65, bracketY + 3);
            glEnd();
        }
        
        // Umbilical connection point (quick disconnect)
        glColor3f(0.8f, 0.8f, 0.3f); // Yellow for LOX/Methane connections
        glBegin(GL_POLYGON);
        for(int j = 0; j < 360; j += 30) {
            float angle = j * 3.14159f / 180.0f;
            glVertex2f(launchPadX + i * 40 + 8 * cos(angle), launchPadY + 40 + 8 * sin(angle));
        }
        glEnd();
    }
    
    // ===== FOUNDATION AND SUPPORTS =====
    // Concrete foundation pillars
    glColor3f(0.4f, 0.4f, 0.4f);
    for(int i = -2; i <= 2; i++) {
        if(i == 0) continue;
        float pillarX = launchPadX + i * 35;
        glBegin(GL_QUADS);
        glVertex2f(pillarX - 6, 120);
        glVertex2f(pillarX + 6, 120);
        glVertex2f(pillarX + 6, launchPadY - 80);
        glVertex2f(pillarX - 6, launchPadY - 80);
        glEnd();
        
        // Reinforcement details
        glColor3f(0.3f, 0.3f, 0.3f);
        for(int j = 0; j < 6; j++) {
            float rebarY = 130 + j * 20;
            glBegin(GL_QUADS);
            glVertex2f(pillarX - 8, rebarY);
            glVertex2f(pillarX + 8, rebarY);
            glVertex2f(pillarX + 8, rebarY + 2);
            glVertex2f(pillarX - 8, rebarY + 2);
            glEnd();
        }
    }
    
    // ===== SAFETY SYSTEMS =====
    // Lightning protection towers (in distance)
    glColor3f(0.6f, 0.6f, 0.6f);
    for(int i = -1; i <= 1; i += 2) {
        float towerX = launchPadX + i * 150;
        glBegin(GL_QUADS);
        glVertex2f(towerX - 3, 120);
        glVertex2f(towerX + 3, 120);
        glVertex2f(towerX + 3, 350);
        glVertex2f(towerX - 3, 350);
        glEnd();
        
        // Lightning rod
        glColor3f(0.9f, 0.9f, 0.1f);
        glBegin(GL_TRIANGLES);
        glVertex2f(towerX - 5, 350);
        glVertex2f(towerX + 5, 350);
        glVertex2f(towerX, 380);
        glEnd();
    }
    
    // ===== SURFACE DETAILS =====
    // Concrete expansion joints
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for(int i = -3; i <= 3; i++) {
        if(i == 0) continue;
        float jointX = launchPadX + i * 25;
        glVertex2f(jointX, 120);
        glVertex2f(jointX, launchPadY - 20);
    }
    glEnd();
    
    // Warning stripes around launch area
    glColor3f(1.0f, 0.5f, 0.0f); // Orange
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(launchPadX - 120, 140);
    glVertex2f(launchPadX + 120, 140);
    glVertex2f(launchPadX + 120, launchPadY - 60);
    glVertex2f(launchPadX - 120, launchPadY - 60);
    glEnd();
}

void drawLaunchComplex() {
    // Draw main launch pad
    drawLaunchPad();
    
    // FIXED positions for infrastructure
    float launchPadX = 400.0f;
    float launchPadY = 150.0f;
    
    // ===== LIQUID PROPELLANT STORAGE TANKS =====
    // LOX Tank (light blue)
    float loxX = launchPadX - 180;
    glColor3f(0.7f, 0.9f, 1.0f); // Light blue for LOX
    // Tank body
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(loxX + 25 * cos(angle), 180 + 50 * sin(angle));
    }
    glEnd();
    
    // Tank dome
    glColor3f(0.8f, 0.95f, 1.0f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 180; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(loxX + 25 * cos(angle), 230 + 15 * sin(angle));
    }
    glEnd();
    
    // Tank label
    glColor3f(0.0f, 0.2f, 0.6f);
    glRasterPos2f(loxX - 15, 150);
    std::string loxText = "LOX";
    for(char c : loxText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    
    // Methane/CH4 Tank (gray with yellow stripe)
    float ch4X = launchPadX + 180;
    glColor3f(0.8f, 0.8f, 0.8f); // Gray for methane
    // Tank body
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(ch4X + 25 * cos(angle), 180 + 50 * sin(angle));
    }
    glEnd();
    
    // Safety stripe
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glBegin(GL_QUADS);
    glVertex2f(ch4X - 25, 200);
    glVertex2f(ch4X + 25, 200);
    glVertex2f(ch4X + 25, 210);
    glVertex2f(ch4X - 25, 210);
    glEnd();
    
    // Tank dome
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 180; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(ch4X + 25 * cos(angle), 230 + 15 * sin(angle));
    }
    glEnd();
    
    // Tank label
    glColor3f(0.3f, 0.3f, 0.3f);
    glRasterPos2f(ch4X - 20, 150);
    std::string ch4Text = "CH4";
    for(char c : ch4Text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    
    // ===== PIPING SYSTEMS =====
    // Draw pipelines from tanks to launch pad
    glColor3f(0.7f, 0.7f, 0.7f); // Stainless steel
    glLineWidth(6.0f);
    
    // LOX pipeline
    glBegin(GL_LINES);
    glVertex2f(loxX, 180);
    glVertex2f(loxX - 40, 180);
    glVertex2f(loxX - 40, 180);
    glVertex2f(loxX - 40, 140);
    glVertex2f(loxX - 40, 140);
    glVertex2f(launchPadX - 60, 140);
    glVertex2f(launchPadX - 60, 140);
    glVertex2f(launchPadX - 60, launchPadY - 40);
    glEnd();
    
    // CH4 pipeline
    glBegin(GL_LINES);
    glVertex2f(ch4X, 180);
    glVertex2f(ch4X + 40, 180);
    glVertex2f(ch4X + 40, 180);
    glVertex2f(ch4X + 40, 140);
    glVertex2f(ch4X + 40, 140);
    glVertex2f(launchPadX + 60, 140);
    glVertex2f(launchPadX + 60, 140);
    glVertex2f(launchPadX + 60, launchPadY - 40);
    glEnd();
    
    // Pipeline supports
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(3.0f);
    float supportPositions[] = {loxX - 20, loxX - 40, ch4X + 20, ch4X + 40, launchPadX - 60, launchPadX + 60};
    for(float pos : supportPositions) {
        glBegin(GL_LINES);
        glVertex2f(pos - 3, 130);
        glVertex2f(pos + 3, 130);
        glVertex2f(pos, 130);
        glVertex2f(pos, 120);
        glEnd();
    }
    
    // ===== WATER DELUGE TOWER =====
    float waterTowerX = launchPadX - 280;
    // Tower structure
    glColor3f(0.6f, 0.6f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(waterTowerX - 10, 120);
    glVertex2f(waterTowerX + 10, 120);
    glVertex2f(waterTowerX + 10, 280);
    glVertex2f(waterTowerX - 10, 280);
    glEnd();
    
    // Water tank
    glColor3f(0.3f, 0.5f, 0.9f); // Blue for water
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(waterTowerX + 20 * cos(angle), 280 + 25 * sin(angle));
    }
    glEnd();
    
    // Water tower label
    glColor3f(0.1f, 0.3f, 0.8f);
    glRasterPos2f(waterTowerX - 25, 130);
    std::string waterText = "WATER DELUGE";
    for(char c : waterText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
    
    // ===== ELECTRICAL SUBSTATION =====
    float substationX = launchPadX + 280;
    // Substation building
    glColor3f(0.7f, 0.7f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(substationX - 30, 120);
    glVertex2f(substationX + 30, 120);
    glVertex2f(substationX + 30, 180);
    glVertex2f(substationX - 30, 180);
    glEnd();
    
    // Transformer
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(substationX - 20, 180);
    glVertex2f(substationX + 20, 180);
    glVertex2f(substationX + 20, 220);
    glVertex2f(substationX - 20, 220);
    glEnd();
    
    // Danger sign
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 30) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(substationX + 10 * cos(angle), 240 + 10 * sin(angle));
    }
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(substationX - 5, 238);
    std::string dangerText = "!";
    for(char c : dangerText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    // ===== ACCESS ROADS =====
    glColor3f(0.4f, 0.4f, 0.3f);
    glLineWidth(8.0f);
    glBegin(GL_LINES);
    // Main access road
    glVertex2f(launchPadX - 200, 110);
    glVertex2f(launchPadX + 200, 110);
    // Road to LOX tank
    glVertex2f(launchPadX - 120, 110);
    glVertex2f(loxX, 110);
    glVertex2f(loxX, 110);
    glVertex2f(loxX, 130);
    // Road to CH4 tank
    glVertex2f(launchPadX + 120, 110);
    glVertex2f(ch4X, 110);
    glVertex2f(ch4X, 110);
    glVertex2f(ch4X, 130);
    glEnd();
    
    // Road markings
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glLineStipple(1, 0xF0F0);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glVertex2f(launchPadX - 200, 110);
    glVertex2f(launchPadX + 200, 110);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    
    // ===== VEHICLE PARKING =====
    // Crew transport vehicle
    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(launchPadX - 220, 115);
    glVertex2f(launchPadX - 190, 115);
    glVertex2f(launchPadX - 190, 130);
    glVertex2f(launchPadX - 220, 130);
    glEnd();
    
    // Fire truck
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(launchPadX + 190, 115);
    glVertex2f(launchPadX + 220, 115);
    glVertex2f(launchPadX + 220, 130);
    glVertex2f(launchPadX + 190, 130);
    glEnd();
}

void drawFirstStage() {
    // First stage (booster) - make it BRIGHT
    glColor3f(1.0f, 0.3f, 0.3f); // Bright red
    glBegin(GL_QUADS);
    glVertex2f(-20, 0);
    glVertex2f(20, 0);
    glVertex2f(20, 150);
    glVertex2f(-20, 150);
    glEnd();
    
    // Add white stripes for visibility
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-18, 30);
    glVertex2f(18, 30);
    glVertex2f(18, 40);
    glVertex2f(-18, 40);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex2f(-18, 90);
    glVertex2f(18, 90);
    glVertex2f(18, 100);
    glVertex2f(-18, 100);
    glEnd();
    
    // Engine details
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(-15, 0);
    glVertex2f(15, 0);
    glVertex2f(15, -10);
    glVertex2f(-15, -10);
    glEnd();
    
    // Stage label - make it larger
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-20, 75);
    std::string stageText = "STAGE 1";
    for(char c : stageText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void drawSecondStage(float yOffset) {
    // Second stage - make it BRIGHT and clearly different from stage 1
    // IMPORTANT: Remove the gap by adjusting yOffset
    glColor3f(0.2f, 0.6f, 1.0f); // Bright BLUE to distinguish from stage 1
    glBegin(GL_QUADS);
    glVertex2f(-15, yOffset - 2); // Move down 2 pixels to close gap
    glVertex2f(15, yOffset - 2);  // Move down 2 pixels to close gap
    glVertex2f(15, yOffset + 70);
    glVertex2f(-15, yOffset + 70);
    glEnd();
    
    // Add black stripes for visibility
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-14, yOffset + 8); // Adjusted for new position
    glVertex2f(14, yOffset + 8);
    glVertex2f(14, yOffset + 13);
    glVertex2f(-14, yOffset + 13);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex2f(-14, yOffset + 38); // Adjusted for new position
    glVertex2f(14, yOffset + 38);
    glVertex2f(14, yOffset + 43);
    glVertex2f(-14, yOffset + 43);
    glEnd();
    
    // Stage separation line (dashed) - move it down slightly
    glColor3f(1.0f, 1.0f, 0.0f); // Bright yellow line
    glLineWidth(3.0f);
    glLineStipple(1, 0xF0F0); // Dashed pattern
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    glVertex2f(-20, yOffset - 2); // Move separation line down
    glVertex2f(20, yOffset - 2);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    
    // Stage label - make it larger
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-18, yOffset + 33); // Adjusted position
    std::string stageText = "STAGE 2";
    for(char c : stageText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void drawThirdStage(float yOffset) {
    // Third stage (PAYLOAD) - make it MUCH larger and more visible
    // Main body - BRIGHT SILVER
    glColor3f(0.95f, 0.95f, 0.95f); // Even brighter silver
    glBegin(GL_QUADS);
    glVertex2f(-15, yOffset - 2);  // Wider (from -12 to -15)
    glVertex2f(15, yOffset - 2);   // Wider (from 12 to 15)
    glVertex2f(15, yOffset + 60);  // Taller (from 50 to 60)
    glVertex2f(-13, yOffset + 60); // Taller and adjusted
    glEnd();
    
    // Add some metallic shine
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-12, yOffset + 40); // Adjusted
    glVertex2f(-7, yOffset + 40);
    glVertex2f(-7, yOffset + 50);
    glVertex2f(-12, yOffset + 50);
    glEnd();
    
    // Fairing (nose cone) - BRIGHT WHITE and larger
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-15, yOffset + 60); // Wider base
    glVertex2f(15, yOffset + 60);  // Wider base
    glVertex2f(0, yOffset + 110);  // Taller nose cone (from 90 to 110)
    glEnd();
    
    // Details (windows) - VIVID BLUE and larger
    glColor3f(0.0f, 0.9f, 1.0f); // Brighter blue
    glBegin(GL_QUADS);
    glVertex2f(-6, yOffset + 30); // Larger window
    glVertex2f(6, yOffset + 30);
    glVertex2f(6, yOffset + 40);
    glVertex2f(-6, yOffset + 40);
    glEnd();
    
    // Add another window
    glBegin(GL_QUADS);
    glVertex2f(-6, yOffset + 15); // Larger window
    glVertex2f(6, yOffset + 15);
    glVertex2f(6, yOffset + 25);
    glVertex2f(-6, yOffset + 25);
    glEnd();
    
    // Engine nozzle at bottom - larger
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(-10, yOffset - 12);  // Larger nozzle
    glVertex2f(10, yOffset - 12);
    glVertex2f(10, yOffset - 2);    // Connect to main body
    glVertex2f(-10, yOffset - 2);
    glEnd();
    
    // Add a highlight on the nozzle
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-8, yOffset - 10);
    glVertex2f(-4, yOffset - 10);
    glVertex2f(-4, yOffset - 4);
    glVertex2f(-8, yOffset - 4);
    glEnd();
    
    // Payload label - make it LARGE and BOLD
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(-20, yOffset + 85); // Adjusted position
    std::string stageText = "PAYLOAD";
    for(char c : stageText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    // Add "STAGE 3" label
    glColor3f(1.0f, 0.0f, 1.0f); // Purple to match flames
    glRasterPos2f(-17, yOffset + 70); // Adjusted position
    std::string stage3Text = "STAGE 3";
    for(char c : stage3Text) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
}

void drawSeparatedStage(const RocketStage& stage, int stageNum) {
    if (!stage.falling) return;
    
    glPushMatrix();
    glTranslatef(stage.x, stage.y, 0);
    glRotatef(stage.angle, 0, 0, 1);
    
    if (stageNum == 1) {
        // Draw falling first stage - make it BRIGHT and different color
        glColor3f(1.0f, 0.3f, 0.3f); // Bright red
        glBegin(GL_QUADS);
        glVertex2f(-20, 0);
        glVertex2f(20, 0);
        glVertex2f(20, 150);
        glVertex2f(-20, 150);
        glEnd();
        
        // Add some details to make it visible
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_QUADS);
        glVertex2f(-18, 30);
        glVertex2f(-10, 30);
        glVertex2f(-10, 40);
        glVertex2f(-18, 40);
        glEnd();
        
        glBegin(GL_QUADS);
        glVertex2f(10, 30);
        glVertex2f(18, 30);
        glVertex2f(18, 40);
        glVertex2f(10, 40);
        glEnd();
        
        // Draw engine details
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(-15, 0);
        glVertex2f(15, 0);
        glVertex2f(15, -10);
        glVertex2f(-15, -10);
        glEnd();
        
        // Draw parachute if falling
        if (stage.y < 400 && stage.velocity < -1.0f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            // Parachute canopy - make it LARGE and BRIGHT
            glColor4f(1.0f, 1.0f, 1.0f, 0.95f);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 170);
            for(int i = 0; i <= 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(40 * cos(angle), 170 + 25 * sin(angle));
            }
            glEnd();
            
            // Parachute lines
            glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
            glLineWidth(3.0f);
            glBegin(GL_LINES);
            glVertex2f(0, 170); glVertex2f(-20, 150);
            glVertex2f(0, 170); glVertex2f(20, 150);
            glVertex2f(0, 170); glVertex2f(0, 155);
            glVertex2f(0, 170); glVertex2f(-15, 140);
            glVertex2f(0, 170); glVertex2f(15, 140);
            glEnd();
            
            glDisable(GL_BLEND);
        }
        
        // Stage label - make it LARGE
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(-25, 75);
        std::string stageText = "STAGE 1";
        for(char c : stageText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    } 
    else if (stageNum == 2) {
        // Draw falling second stage - make it BRIGHT and different color
        glColor3f(1.0f, 0.5f, 0.3f); // Bright orange-red
        glBegin(GL_QUADS);
        glVertex2f(-15, -2); // Move down 2 pixels to match adjustment
        glVertex2f(15, -2);
        glVertex2f(15, 68);  // Adjusted height (70 - 2)
        glVertex2f(-15, 68);
        glEnd();
        
        // Add some details
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_QUADS);
        glVertex2f(-12, 18); // Adjusted
        glVertex2f(-5, 18);
        glVertex2f(-5, 28);
        glVertex2f(-12, 28);
        glEnd();
        
        glBegin(GL_QUADS);
        glVertex2f(5, 18); // Adjusted
        glVertex2f(12, 18);
        glVertex2f(12, 28);
        glVertex2f(5, 28);
        glEnd();
        
        // Stage label - make it LARGE
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(-20, 33); // Adjusted
        std::string stageText = "STAGE 2";
        for(char c : stageText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
        }
    }
    
    glPopMatrix();
}

void drawRocket() {
    if (rocketExploded) return;
    
    // Draw separated stages if they exist
    if (stage1Separated && stage1.falling) {
        drawSeparatedStage(stage1, 1);
    }
    if (stage2Separated && stage2.falling) {
        drawSeparatedStage(stage2, 2);
    }
    
    // Draw current active rocket
    glPushMatrix();
    glTranslatef(rocketX, rocketY, 0);
    glRotatef(rocketAngle, 0, 0, 1);
    
    // IMPORTANT: Only draw the CURRENT active stage
    if (rocketStage == 1) {
        // Draw all three stages together when stage 1 is active
        // Adjust positions to eliminate gaps
        drawFirstStage();
        drawSecondStage(148.0f); // Changed from 150.0f to 148.0f (2 pixels up)
        drawThirdStage(218.0f);  // Changed from 220.0f to 218.0f (2 pixels up)
        
        // Draw stage 1 flames (ORANGE)
        if (rocketLaunching && rocketFuel > 0) {
            drawFlamesForStage(1);
        }
    } 
    else if (rocketStage == 2) {
        // IMPORTANT: When stage 2 is active, we should NOT draw stage 1
        // Draw stage 2 (at position 0) and stage 3 (at offset 68)
        drawSecondStage(0.0f);
        drawThirdStage(68.0f); // Changed from 70.0f to 68.0f (2 pixels up)
        
        // Draw stage 2 flames (BLUE)
        if (rocketLaunching && rocketFuel > 0) {
            drawFlamesForStage(2);
        }
    }
    else if (rocketStage == 3) {
        // IMPORTANT: When stage 3 (PAYLOAD) is active, draw ONLY the payload
        // Payload should be at position 0 (it's now the main rocket)
        drawThirdStage(0.0f);
        
        // Draw payload flames (PURPLE) - make it BRIGHTER
        if (rocketLaunching && rocketFuel > 0) {
            drawFlamesForStage(3);
        }
    }
    
    glPopMatrix();
}

void drawFlames() {
    if (!rocketLaunching && !rocketLaunched) return;
    if (rocketFuel <= 0) return;
    
    // Only draw flames for active rocket, not separated stages
    glPushMatrix();
    glTranslatef(rocketX, rocketY, 0);
    glRotatef(rocketAngle, 0, 0, 1);
    
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    
    // Only draw main engine if we have fuel
    if (rocketFuel > 0) {
        // Main engine flame - Brighter
        for(int layer = 0; layer < 3; layer++) {
            float layerSize = flameSize * (1.0f - layer * 0.3f);
            float layerOffset = layer * 5.0f;
            
            if (layer == 0) glColor3f(1.0f, 0.4f, 0.0f); // Brighter orange
            else if (layer == 1) glColor3f(1.0f, 0.9f, 0.0f); // Brighter yellow
            else glColor3f(1.0f, 1.0f, 1.0f); // White
            
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, -layerOffset); // Center at engine
            
            // Create wavy flame effect
            for(int i = 0; i <= 360; i += 20) {
                float angle = i * 3.14159f / 180.0f;
                float wave = sin(time * 10.0f + angle * 5.0f) * 0.2f + 1.0f;
                float x = sin(angle) * 15 * wave;
                float y = -layerSize - cos(angle) * 15 * wave;
                glVertex2f(x, y);
            }
            glEnd();
        }
    }
    
    // Side boosters (only for stage 1 and if it hasn't separated)
    if (rocketStage == 1 && !stage1Separated && rocketFuel > 0) {
        for(int i = -1; i <= 1; i += 2) {
            glPushMatrix();
            glTranslatef(i * 25, 0, 0);
            
            glColor3f(1.0f, 0.6f, 0.0f); // Brighter
            glBegin(GL_TRIANGLES);
            glVertex2f(-5, 0);
            glVertex2f(5, 0);
            glVertex2f(0, -flameSize * 0.7f);
            glEnd();
            
            glPopMatrix();
        }
    }
    
    glPopMatrix();
}

void drawSmoke() {
    if (smokeParticles.empty()) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    
    for(const auto& smoke : smokeParticles) {
        // Calculate smoke transparency based on life
        float alpha = smoke.alpha * (smoke.life / 0.5f); // Fade out as life decreases
        
        // Realistic smoke colors - white/gray for clean smoke
        float r = smoke.r * 0.8f + 0.2f; // Mix with white
        float g = smoke.g * 0.8f + 0.2f;
        float b = smoke.b * 0.8f + 0.2f;
        
        glColor4f(r, g, b, alpha * 0.7f); // More transparent
        
        // Draw smoke as soft, fuzzy particles (not solid shapes)
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(smoke.x, smoke.y); // Center
        
        // Create wavy, organic smoke shape
        for(int i = 0; i <= 360; i += 15) {
            float angle = i * 3.14159f / 180.0f;
            
            // Make smoke wavy and irregular
            float wave1 = sin(time * 2.0f + angle * 3.0f) * 0.3f;
            float wave2 = cos(time * 1.5f + angle * 2.0f) * 0.2f;
            float wave3 = sin(smoke.x * 0.01f + smoke.y * 0.01f + time) * 0.2f;
            
            float radius = smoke.size * (1.0f + wave1 + wave2 + wave3);
            
            // Make smoke puffier by varying radius more
            if (i % 45 == 0) {
                radius *= 1.3f; // Create puffier sections
            }
            
            float x = smoke.x + radius * cos(angle);
            float y = smoke.y + radius * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
        
        // Add a second, more transparent layer for depth
        glColor4f(r * 1.2f, g * 1.2f, b * 1.2f, alpha * 0.3f); // Brighter, more transparent
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(smoke.x, smoke.y);
        for(int i = 0; i <= 360; i += 30) {
            float angle = i * 3.14159f / 180.0f;
            float wave = sin(time * 3.0f + angle * 4.0f) * 0.4f;
            float radius = smoke.size * 1.5f * (1.0f + wave);
            glVertex2f(smoke.x + radius * cos(angle), smoke.y + radius * sin(angle));
        }
        glEnd();
    }
    
    // Draw smoke trails for falling stages
    if (stage1Separated && stage1Falling && stage1.falling) {
        // Smoke trail for falling stage 1
        glColor4f(0.3f, 0.3f, 0.3f, 0.4f);
        glBegin(GL_QUAD_STRIP);
        for(int i = 0; i < 10; i++) {
            float ratio = i / 10.0f;
            float trailX = stage1.x - sin(stage1.angle * 3.14159f / 180.0f) * ratio * 50.0f;
            float trailY = stage1.y - cos(stage1.angle * 3.14159f / 180.0f) * ratio * 50.0f;
            float width = 10.0f * (1.0f - ratio);
            glVertex2f(trailX - width, trailY);
            glVertex2f(trailX + width, trailY);
        }
        glEnd();
    }
    
    if (stage2Separated && stage2Falling && stage2.falling) {
        // Smoke trail for falling stage 2
        glColor4f(0.25f, 0.25f, 0.25f, 0.3f);
        glBegin(GL_QUAD_STRIP);
        for(int i = 0; i < 8; i++) {
            float ratio = i / 8.0f;
            float trailX = stage2.x - sin(stage2.angle * 3.14159f / 180.0f) * ratio * 40.0f;
            float trailY = stage2.y - cos(stage2.angle * 3.14159f / 180.0f) * ratio * 40.0f;
            float width = 8.0f * (1.0f - ratio);
            glVertex2f(trailX - width, trailY);
            glVertex2f(trailX + width, trailY);
        }
        glEnd();
    }
    
    glDisable(GL_BLEND);
}

void drawExplosion() {
    if (!rocketExploded) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for(const auto& particle : explosionParticles) {
        glColor4f(particle.r, particle.g, particle.b, particle.life);
        glBegin(GL_TRIANGLE_FAN);
        for(int i = 0; i < 360; i += 30) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(particle.x + particle.size * cos(angle), 
                      particle.y + particle.size * sin(angle));
        }
        glEnd();
    }
    
    glDisable(GL_BLEND);
}

void drawControlTower() {
    float towerX = 650.0f;  // Move to far right
    float towerY = 150.0f;
    
    // ===== MAIN CONTROL BUILDING =====
    // Modern glass and steel control center
    glColor3f(0.3f, 0.3f, 0.35f); // Dark glass
    glBegin(GL_QUADS);
    glVertex2f(towerX - 60, towerY);
    glVertex2f(towerX + 60, towerY);
    glVertex2f(towerX + 60, towerY + 200);
    glVertex2f(towerX - 60, towerY + 200);
    glEnd();
    
    // Steel frame
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    // Vertical columns
    for(int i = -2; i <= 2; i++) {
        float columnX = towerX + i * 30;
        glVertex2f(columnX, towerY);
        glVertex2f(columnX, towerY + 200);
    }
    // Horizontal beams
    for(int i = 0; i < 4; i++) {
        float beamY = towerY + i * 50;
        glVertex2f(towerX - 60, beamY);
        glVertex2f(towerX + 60, beamY);
    }
    glEnd();
    
    // ===== GLASS WINDOWS =====
    for(int floor = 0; floor < 4; floor++) {
        for(int col = 0; col < 3; col++) {
            float winX = towerX - 40 + col * 40;
            float winY = towerY + 150 - floor * 40;
            
            // Window glass with blue tint
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.1f, 0.2f, 0.4f, 0.7f);
            glBegin(GL_QUADS);
            glVertex2f(winX - 15, winY - 15);
            glVertex2f(winX + 15, winY - 15);
            glVertex2f(winX + 15, winY + 15);
            glVertex2f(winX - 15, winY + 15);
            glEnd();
            
            // Interior lights (randomly on)
            if (rand() % 100 < 70) { // 70% chance light is on
                glColor4f(1.0f, 1.0f, 0.8f, 0.9f);
                glBegin(GL_QUADS);
                glVertex2f(winX - 12, winY - 12);
                glVertex2f(winX + 12, winY - 12);
                glVertex2f(winX + 12, winY + 12);
                glVertex2f(winX - 12, winY + 12);
                glEnd();
                
                // Computer monitors (blue glow)
                glColor4f(0.0f, 0.5f, 1.0f, 0.6f);
                glBegin(GL_QUADS);
                glVertex2f(winX - 8, winY);
                glVertex2f(winX + 8, winY);
                glVertex2f(winX + 8, winY + 8);
                glVertex2f(winX - 8, winY + 8);
                glEnd();
            }
            glDisable(GL_BLEND);
        }
    }
    
    // ===== COMMUNICATIONS ARRAY =====
    // Microwave dish
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glTranslatef(towerX, towerY + 220, 0);
    glRotatef(glutGet(GLUT_ELAPSED_TIME) * 0.02f, 0, 0, 1);
    
    // Parabolic dish
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 15) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(20 * cos(angle), 20 * sin(angle));
    }
    glEnd();
    
    // Support arm
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-2, 0);
    glVertex2f(2, 0);
    glVertex2f(2, -40);
    glVertex2f(-2, -40);
    glEnd();
    
    glPopMatrix();
    
    // ===== ANTENNA MAST =====
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(towerX + 40, towerY + 200);
    glVertex2f(towerX + 44, towerY + 200);
    glVertex2f(towerX + 44, towerY + 300);
    glVertex2f(towerX + 40, towerY + 300);
    glEnd();
    
    // Antenna elements
    glColor3f(0.6f, 0.6f, 0.6f);
    for(int i = 0; i < 3; i++) {
        float antennaY = towerY + 220 + i * 25;
        glBegin(GL_QUADS);
        glVertex2f(towerX + 44, antennaY - 2);
        glVertex2f(towerX + 60, antennaY - 2);
        glVertex2f(towerX + 60, antennaY + 2);
        glVertex2f(towerX + 44, antennaY + 2);
        glEnd();
    }
    
    // ===== SECURITY AND SAFETY =====
    // Perimeter fence
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for(int i = 0; i < 10; i++) {
        float fenceX = towerX - 80 + i * 16;
        glVertex2f(fenceX, towerY - 20);
        glVertex2f(fenceX, towerY - 5);
    }
    glVertex2f(towerX - 80, towerY - 12);
    glVertex2f(towerX + 80, towerY - 12);
    glEnd();
    
    // Security camera
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 30) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(towerX - 50 + 5 * cos(angle), towerY + 180 + 5 * sin(angle));
    }
    glEnd();
    
    // Camera lens
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for(int i = 0; i < 360; i += 30) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(towerX - 50 + 2 * cos(angle), towerY + 180 + 2 * sin(angle));
    }
    glEnd();
    
    // Camera moving periodically
    float cameraAngle = sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f) * 30.0f;
    glPushMatrix();
    glTranslatef(towerX - 50, towerY + 180, 0);
    glRotatef(cameraAngle, 0, 0, 1);
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0);
    glVertex2f(-3, -10);
    glVertex2f(3, -10);
    glEnd();
    glPopMatrix();
    
    // ===== BUILDING SIGN =====
    glColor3f(0.9f, 0.9f, 0.9f);
    glRasterPos2f(towerX - 45, towerY - 30);
    std::string signText = "MISSION CONTROL";
    for(char c : signText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void drawClouds() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for(const auto& cloud : clouds) {
        glColor4f(1.0f, 1.0f, 1.0f, cloud.opacity * (0.4f + 0.6f * dayTime)); // More opaque
        
        // Draw cloud as multiple overlapping circles
        for(int i = 0; i < 3; i++) {
            float offsetX = (i - 1) * cloud.size * 0.6f;
            float offsetY = (i % 2 == 0) ? cloud.size * 0.2f : -cloud.size * 0.2f;
            
            glBegin(GL_TRIANGLE_FAN);
            for(int j = 0; j <= 360; j += 15) {
                float angle = j * 3.14159f / 180.0f;
                float radius = cloud.size * (0.6f + 0.4f * sin(j * 0.1f));
                glVertex2f(cloud.x + offsetX + radius * cos(angle), 
                          cloud.y + offsetY + radius * sin(angle));
            }
            glEnd();
        }
    }
    
    glDisable(GL_BLEND);
}

void drawSatellites() {
    for(const auto& satellite : satellites) {
        glPushMatrix();
        glTranslatef(satellite.x, satellite.y, 0);
        glRotatef(satellite.angle, 0, 0, 1);
        
        // Main body - Brighter
        glColor3f(0.7f, 0.7f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(-satellite.size, -satellite.size/2);
        glVertex2f(satellite.size, -satellite.size/2);
        glVertex2f(satellite.size, satellite.size/2);
        glVertex2f(-satellite.size, satellite.size/2);
        glEnd();
        
        // Solar panels - Brighter
        glColor3f(0.3f, 0.5f, 0.9f);
        glBegin(GL_QUADS);
        glVertex2f(satellite.size, -satellite.size);
        glVertex2f(satellite.size + satellite.size/2, -satellite.size);
        glVertex2f(satellite.size + satellite.size/2, satellite.size);
        glVertex2f(satellite.size, satellite.size);
        glEnd();
        
        // Antenna
        glColor3f(0.9f, 0.9f, 0.9f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(0, satellite.size/2);
        glVertex2f(0, satellite.size);
        glEnd();
        
        glPopMatrix();
    }
}

void drawMeteors() {
    for(const auto& meteor : meteors) {
        // Meteor body - Brighter
        glColor3f(0.9f, 0.5f, 0.2f);
        glBegin(GL_POLYGON);
        for(int i = 0; i < 360; i += 30) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(meteor.x + meteor.size * cos(angle), 
                      meteor.y + meteor.size * sin(angle));
        }
        glEnd();
        
        // Glow effect
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.7f, 0.3f, 0.6f); // Brighter
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(meteor.x, meteor.y);
        for(int i = 0; i <= 360; i += 30) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(meteor.x + meteor.size * 2 * cos(angle), 
                      meteor.y + meteor.size * 2 * sin(angle));
        }
        glEnd();
        glDisable(GL_BLEND);
        
        // Trail
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.9f, 0.5f, 0.2f, 0.4f); // Brighter
        glBegin(GL_QUAD_STRIP);
        for(size_t i = 0; i < meteor.trailX.size(); i++) {
            float ratio = float(i) / meteor.trailX.size();
            float trailWidth = meteor.size * (1.0f - ratio);
            glVertex2f(meteor.trailX[i] - trailWidth, meteor.trailY[i]);
            glVertex2f(meteor.trailX[i] + trailWidth, meteor.trailY[i]);
        }
        glEnd();
        glDisable(GL_BLEND);
    }
}

void drawTrajectory() {
    if (!showTrajectory) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw recorded trajectory line if we have points
    if (trajectoryPoints.size() >= 2) {
        glColor4f(0.0f, 1.0f, 0.0f, 0.8f); // Brighter green
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for(const auto& point : trajectoryPoints) {
            glVertex2f(point.first, point.second);
        }
        // Add current position if rocket exists
        if (!rocketExploded) {
            glVertex2f(rocketX, rocketY);
        }
        glEnd();
    } else if (!rocketLaunching && !rocketLaunched) {
        // Draw starting point
        glColor4f(0.0f, 1.0f, 0.0f, 0.8f);
        glPointSize(6.0f); // Larger
        glBegin(GL_POINTS);
        glVertex2f(rocketX, rocketY);
        glEnd();
    }
    
    // Always draw predicted trajectory when trajectory is enabled
    glColor4f(1.0f, 1.0f, 0.0f, 0.6f); // Brighter yellow
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    
    float predX = rocketX;
    float predY = rocketY;
    float predVx = 0;
    float predVy = rocketVelocity;
    
    // If not launching, show trajectory with thrust
    if (!rocketLaunching && !rocketLaunched && !rocketExploded) {
        predVy = 0.5f; // Initial thrust
    }
    
    for(int i = 0; i < 100; i++) {
        if (rocketLaunching && rocketFuel > 0 && i < 20) {
            predVy += 0.08f; // Thrust while fuel lasts
        } else {
            predVy -= 0.05f; // Gravity
        }
        predX += predVx + windSpeed * windDirection * 0.1f; // Wind effect
        predY += predVy;
        glVertex2f(predX, predY);
        
        if (predY < 0 || predY > 800) break;
    }
    glEnd();
    
    glDisable(GL_BLEND);
}

void drawGauge(float x, float y, float value, float maxValue, const std::string& label, float r, float g, float b) {
    // Gauge background - Brighter
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(x - 25, y - 50);
    glVertex2f(x + 25, y - 50);
    glVertex2f(x + 25, y);
    glVertex2f(x - 25, y);
    glEnd();
    
    // Gauge fill
    float fillHeight = (value / maxValue) * 50.0f;
    if (fillHeight > 50.0f) fillHeight = 50.0f;
    
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x - 20, y - 50);
    glVertex2f(x + 20, y - 50);
    glVertex2f(x + 20, y - 50 + fillHeight);
    glVertex2f(x - 20, y - 50 + fillHeight);
    glEnd();
    
    // Gauge border
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 25, y - 50);
    glVertex2f(x + 25, y - 50);
    glVertex2f(x + 25, y);
    glVertex2f(x - 25, y);
    glEnd();
}

// Simple text drawing function
void drawText(float x, float y, const std::string& text) {
    glColor3f(1.0f, 1.0f, 1.0f); // Ensure white text
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}

void drawUI() {
    // Draw HUD background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Top bar (Mission status)
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f); // More opaque
    glBegin(GL_QUADS);
    glVertex2f(0, 550);
    glVertex2f(800, 550);
    glVertex2f(800, 600);
    glVertex2f(0, 600);
    glEnd();
    
    // Bottom bar (Controls info)
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(800, 0);
    glVertex2f(800, 40);
    glVertex2f(0, 40);
    glEnd();
    
    // Right panel (Telemetry)
    glBegin(GL_QUADS);
    glVertex2f(600, 40);
    glVertex2f(800, 40);
    glVertex2f(800, 200);
    glVertex2f(600, 200);
    glEnd();
    
    // Left panel (Stage info)
    glBegin(GL_QUADS);
    glVertex2f(0, 200);
    glVertex2f(250, 200);
    glVertex2f(250, 350);
    glVertex2f(0, 350);
    glEnd();
    
    glDisable(GL_BLEND);
    
    // ===== MISSION STATUS =====
    std::string status;
    switch(currentState) {
        case PRELAUNCH: 
            status = "PRELAUNCH CHECKLIST - PRESS SPACE TO START"; 
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow
            break;
        case COUNTDOWN: 
            status = "COUNTDOWN: T-" + std::to_string(countdown);
            if (countdown <= 3) glColor3f(1.0f, 0.0f, 0.0f); // Red for last 3 seconds
            else glColor3f(1.0f, 1.0f, 0.0f); // Yellow
            break;
        case LAUNCH: 
            status = "LAUNCH IN PROGRESS"; 
            glColor3f(0.0f, 1.0f, 0.0f); // Green
            break;
        case ORBIT: 
            status = "ORBIT ACHIEVED!"; 
            glColor3f(0.0f, 1.0f, 1.0f); // Cyan
            break;
        case MISSION_COMPLETE: 
            status = "MISSION SUCCESS!"; 
            glColor3f(0.0f, 1.0f, 0.0f); // Green
            break;
    }
    drawText(10, 580, status);
    
    // Draw mission success/failure message
    if (rocketExploded) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        //drawText(10, 560, "MISSION FAILED - ROCKET EXPLODED!");
    } else if (missionSuccess) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green
        drawText(10, 560, "MISSION ACCOMPLISHED!");
    }
    
    // ===== TELEMETRY PANEL =====
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(610, 180, "TELEMETRY:");
    
    if (showStats) {
        // Draw gauges with labels
        drawGauge(650, 150, rocketFuel, 100.0f, "FUEL", 0.0f, 1.0f, 0.0f);
        drawGauge(700, 150, altitude, 1000.0f, "ALT", 1.0f, 0.0f, 0.0f);
        drawGauge(750, 150, velocity, 10.0f, "VEL", 0.0f, 0.0f, 1.0f);
        
        // Draw gauge values
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(645, 160, std::to_string((int)rocketFuel) + "%");
        drawText(695, 160, std::to_string((int)altitude) + "m");
        drawText(745, 160, std::to_string((int)velocity) + "m/s");
        
        // Additional telemetry data
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(610, 120, "STAGE: " + std::to_string(rocketStage));
        
        // Stage status in telemetry
        if (stage1Separated) {
            drawText(610, 105, "STAGE 1: SEPARATED");
        } else {
            drawText(610, 105, "STAGE 1: ATTACHED");
        }
        
        if (stage2Separated) {
            drawText(610, 90, "STAGE 2: SEPARATED");
        } else {
            drawText(610, 90, "STAGE 2: ATTACHED");
        }
        
        // Trajectory status
        drawText(610, 75, "TRAJECTORY: " + std::string(showTrajectory ? "ON" : "OFF"));
        
        // Camera status
        drawText(610, 60, "CAMERA: " + std::string(cameraFollowRocket ? "FOLLOW" : "FIXED"));
    }
    
    // ===== CONTROLS INFO (Bottom bar) =====
    // In drawUI(), update the controls line:
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(10, 25, "CONTROLS: SPACE=Start  R=Reset  G=Grid  T=Trajectory/TEST  C=Camera  D=Day/Night");
    drawText(10, 10, "          +/-=Zoom  1/2/3/4/5=Views  S=Stats  F=Fuel  P=Separate  I=Info  ESC=Exit");
    
    // ===== WARNINGS/ALERTS =====
    if (rocketFuel < 20.0f && rocketLaunching && !rocketExploded) {
        glColor3f(1.0f, 0.0f, 0.0f); // Red
        drawText(300, 25, "LOW FUEL!");
    }
    
    if (rocketAngle > 15.0f || rocketAngle < -15.0f) {
        glColor3f(1.0f, 0.5f, 0.0f); // Orange
        drawText(300, 10, "HIGH ANGLE DETECTED!");
    }
    
    // ===== MISSION TARGETS =====
    if (currentState == PRELAUNCH || currentState == COUNTDOWN) {
        glColor3f(0.5f, 0.8f, 1.0f);
        drawText(400, 575, "TARGET ALTITUDE: " + std::to_string((int)targetAltitude) + "m");
        drawText(400, 560, "TARGET VELOCITY: " + std::to_string((int)targetVelocity) + "m/s");
    }
    
    // ===== DAY/NIGHT INDICATOR =====
    std::string timeOfDay;
    if (dayTime < 0.25f) timeOfDay = "NIGHT";
    else if (dayTime < 0.5f) timeOfDay = "DAWN";
    else if (dayTime < 0.75f) timeOfDay = "DAY";
    else timeOfDay = "DUSK";
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(700, 575, "TIME: " + timeOfDay);
    
    // ===== ROCKET STATUS =====
    std::string rocketStatus;
    glColor3f(1.0f, 1.0f, 1.0f);
    if (rocketExploded) {
        rocketStatus = "STATUS: DESTROYED";
        glColor3f(1.0f, 0.0f, 0.0f);
    } else if (rocketLaunched) {
        rocketStatus = "STATUS: IN FLIGHT";
        glColor3f(0.0f, 1.0f, 0.0f);
    } else if (rocketLaunching) {
        rocketStatus = "STATUS: LAUNCHING";
        glColor3f(1.0f, 1.0f, 0.0f);
    } else {
        rocketStatus = "STATUS: READY";
        glColor3f(0.5f, 0.5f, 1.0f);
    }
    drawText(10, 540, rocketStatus);
    
    // ===== CAMERA INFO =====
    std::string cameraModeText;
    switch(currentCameraMode) {
        case MODE_LAUNCH_PAD: cameraModeText = "LAUNCH PAD"; break;
        case MODE_WIDE_VIEW: cameraModeText = "WIDE VIEW"; break;
        case MODE_FOLLOW_ROCKET: cameraModeText = "FOLLOW ROCKET"; break;
        case MODE_FOLLOW_STAGE1: cameraModeText = "FOLLOW STAGE 1"; break;
        case MODE_FOLLOW_STAGE2: cameraModeText = "FOLLOW STAGE 2"; break;
    }

    glColor3f(0.5f, 1.0f, 0.5f);
    drawText(10, 520, "CAMERA: " + cameraModeText + 
            " Zoom=" + std::to_string(cameraZoom).substr(0, 4));
    
    // ===== ACTIVE STAGE INFO =====
    glColor3f(1.0f, 1.0f, 0.5f);
    std::string activeStageInfo;
    std::string flameColorInfo;
    
    if (rocketStage == 1) {
        activeStageInfo = "ACTIVE: STAGE 1 (BOOSTER)";
        flameColorInfo = "FLAMES: ORANGE";
    } else if (rocketStage == 2) {
        activeStageInfo = "ACTIVE: STAGE 2 (UPPER STAGE)";
        flameColorInfo = "FLAMES: BLUE";
    } else if (rocketStage == 3) {
        activeStageInfo = "ACTIVE: PAYLOAD";
        flameColorInfo = "FLAMES: PURPLE";
    }
    
    drawText(10, 500, activeStageInfo);
    drawText(10, 485, flameColorInfo);
    
    // ===== ROCKET POSITION =====
    glColor3f(0.8f, 0.8f, 1.0f);
    std::string rocketPos = "POSITION: X=" + std::to_string((int)rocketX) + 
                           " Y=" + std::to_string((int)rocketY) + 
                           " Alt=" + std::to_string((int)altitude) + "m";
    drawText(10, 470, rocketPos);
    
    // ===== STAGE INFO PANEL (Left side) =====
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(10, 340, "=== STAGE STATUS ===");
    
    // Stage 1 status
    glColor3f(1.0f, 0.5f, 0.0f); // Orange
    std::string stage1Status = "STAGE 1: ";
    if (stage1Separated) {
        if (stage1.falling) {
            stage1Status += "FALLING ";
            if (stage1.y < 400 && stage1.velocity < -1.0f) {
                stage1Status += "(PARACHUTE)";
            }
        } else {
            stage1Status += "CRASHED";
        }
    } else {
        stage1Status += "ATTACHED";
    }
    drawText(15, 320, stage1Status);
    
    // Stage 1 position if separated
    if (stage1Separated && stage1.falling) {
        glColor3f(0.8f, 0.8f, 0.8f);
        std::string stage1Pos = "  X=" + std::to_string((int)stage1.x) + 
                               " Y=" + std::to_string((int)stage1.y) + 
                               " V=" + std::to_string((int)stage1.velocity);
        drawText(15, 305, stage1Pos);
    }
    
    // Stage 2 status
    glColor3f(0.5f, 1.0f, 0.5f); // Green
    std::string stage2Status = "STAGE 2: ";
    if (stage2Separated) {
        if (stage2.falling) {
            stage2Status += "FALLING";
        } else {
            stage2Status += "CRASHED";
        }
    } else {
        stage2Status += "ATTACHED";
    }
    drawText(15, 285, stage2Status);
    
    // Stage 2 position if separated
    if (stage2Separated && stage2.falling) {
        glColor3f(0.8f, 0.8f, 0.8f);
        std::string stage2Pos = "  X=" + std::to_string((int)stage2.x) + 
                               " Y=" + std::to_string((int)stage2.y) + 
                               " V=" + std::to_string((int)stage2.velocity);
        drawText(15, 270, stage2Pos);
    }
    
    // Stage 3 (Payload) status
    glColor3f(0.8f, 0.5f, 1.0f); // Purple
    std::string stage3Status = "PAYLOAD: ";
    if (rocketStage == 3) {
        stage3Status += "ACTIVE";
    } else if (rocketExploded) {
        stage3Status += "DESTROYED";
    } else {
        stage3Status += "ATTACHED";
    }
    drawText(15, 250, stage3Status);
    
    // ===== SEPARATION CONDITIONS =====
    if (rocketLaunching && !rocketExploded) {
        glColor3f(0.8f, 0.8f, 1.0f);
        std::string sepInfo;
        
        if (rocketStage == 1 && !stage1Separated) {
            sepInfo = "STAGE 1 SEP: Alt>350m & Fuel<40% & Vel>4m/s";
        } else if (rocketStage == 2 && !stage2Separated) {
            sepInfo = "STAGE 2 SEP: Alt>650m & Fuel<30% & Vel>5m/s";
        } else if (rocketStage == 3) {
            sepInfo = "FINAL STAGE: CONTINUE TO ORBIT";
        }
        
        if (!sepInfo.empty()) {
            drawText(400, 540, sepInfo);
        }
    }
    
    // ===== MISSION PROGRESS =====
    if (rocketLaunching || rocketLaunched) {
        glColor3f(1.0f, 1.0f, 0.5f);
        float progress = (altitude / targetAltitude) * 100.0f;
        if (progress > 100.0f) progress = 100.0f;
        
        std::string progressText = "MISSION PROGRESS: " + std::to_string((int)progress) + "%";
        drawText(400, 560, progressText);
        
        // Progress bar background
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(400, 545);
        glVertex2f(600, 545);
        glVertex2f(600, 550);
        glVertex2f(400, 550);
        glEnd();
        
        // Progress bar fill
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(400, 545);
        glVertex2f(400 + (200.0f * progress / 100.0f), 545);
        glVertex2f(400 + (200.0f * progress / 100.0f), 550);
        glVertex2f(400, 550);
        glEnd();
    }
}

void drawCountdown() {
    if (countdown <= 0) return;
    
    // Draw large countdown numbers
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(400, 300, 0);
    
    std::string countStr = std::to_string(countdown);
    glRasterPos2f(-countStr.length() * 9, -5);
    for(char c : countStr) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    
    glPopMatrix();
}

// ========== INITIALIZATION ==========

void initializeStars() {
    stars.clear();
    for(int i = 0; i < 300; i++) {
        Star star;
        star.x = rand() % 800;
        star.y = 300 + rand() % 300;
        star.brightness = 0.5f + (rand() % 50) / 100.0f; // Brighter
        star.twinkleSpeed = 0.1f + (rand() % 90) / 100.0f;
        stars.push_back(star);
    }
}

void initializeClouds() {
    clouds.clear();
    for(int i = 0; i < 8; i++) {
        Cloud cloud;
        cloud.x = rand() % 900 - 50;
        cloud.y = 200 + rand() % 150;
        cloud.speed = 0.05f + (rand() % 30) / 100.0f;
        cloud.size = 15 + rand() % 25;
        cloud.opacity = 0.4f + (rand() % 60) / 100.0f; // More opaque
        clouds.push_back(cloud);
    }
}

void initializeSatellites() {
    satellites.clear();
    for(int i = 0; i < 3; i++) {
        Satellite sat;
        sat.x = rand() % 800;
        sat.y = 400 + rand() % 200;
        sat.speed = 0.1f + (rand() % 20) / 100.0f;
        sat.size = 5 + rand() % 10;
        sat.angle = rand() % 360;
        satellites.push_back(sat);
    }
}

void initializeMeteors() {
    meteors.clear();
    for(int i = 0; i < 2; i++) {
        Meteor meteor;
        meteor.x = rand() % 800;
        meteor.y = 500 + rand() % 100;
        meteor.speed = 0.3f + (rand() % 20) / 100.0f;
        meteor.size = 3 + rand() % 7;
        meteor.trailLength = 10;
        for(int j = 0; j < meteor.trailLength; j++) {
            meteor.trailX.push_back(meteor.x - j * 2);
            meteor.trailY.push_back(meteor.y - j * 2);
        }
        meteors.push_back(meteor);
    }
}

void initializeStages() {
    // Initialize stage 1
    stage1.x = rocketX;
    stage1.y = rocketY;
    stage1.velocity = rocketVelocity;
    stage1.angle = rocketAngle;
    stage1.fuel = 100.0f;
    stage1.active = true;
    stage1.separated = false;
    stage1.falling = false;
    stage1.separationTime = 0.0f;
    stage1.width = 40.0f;  // 20 units each side
    stage1.height = 150.0f;
    
    // Initialize stage 2 - adjust offset to match drawing
    stage2.x = rocketX;
    stage2.y = rocketY + 148.0f; // Changed from 150.0f to 148.0f
    stage2.velocity = rocketVelocity;
    stage2.angle = rocketAngle;
    stage2.fuel = 100.0f;
    stage2.active = false;
    stage2.separated = false;
    stage2.falling = false;
    stage2.separationTime = 0.0f;
    stage2.width = 30.0f;  // 15 units each side
    stage2.height = 70.0f;
    
    // Initialize stage 3 - adjust offset to match drawing
    stage3.x = rocketX;
    stage3.y = rocketY + 218.0f; // Changed from 220.0f to 218.0f
    stage3.velocity = rocketVelocity;
    stage3.angle = rocketAngle;
    stage3.fuel = 100.0f;
    stage3.active = false;
    stage3.separated = false;
    stage3.falling = false;
    stage3.separationTime = 0.0f;
    stage3.width = 20.0f;  // 10 units each side
    stage3.height = 40.0f;
}

void createExplosion(float x, float y) {
    rocketExploded = true;
    
    for(int i = 0; i < 100; i++) {
        ExplosionParticle particle;
        particle.x = x;
        particle.y = y;
        particle.vx = (rand() % 200 - 100) / 10.0f;
        particle.vy = (rand() % 200 - 100) / 10.0f;
        particle.size = 2 + rand() % 8;
        particle.life = 1.0f;
        
        // Color based on explosion type
        float colorRand = rand() % 100 / 100.0f;
        if (colorRand < 0.3f) {
            particle.r = 1.0f; particle.g = 0.6f; particle.b = 0.0f; // Brighter orange
        } else if (colorRand < 0.6f) {
            particle.r = 1.0f; particle.g = 1.0f; particle.b = 0.0f; // Yellow
        } else if (colorRand < 0.8f) {
            particle.r = 1.0f; particle.g = 0.0f; particle.b = 0.0f; // Red
        } else {
            particle.r = 0.6f; particle.g = 0.6f; particle.b = 0.6f; // Brighter gray
        }
        
        explosionParticles.push_back(particle);
    }
}

void createStageSeparationEffect(float x, float y, int stageNum) {
    // Create separation smoke/particles
    for(int i = 0; i < 30; i++) {
        SmokeParticle debris;
        debris.x = x + (rand() % 60 - 30);
        debris.y = y + (rand() % 60 - 30);
        debris.size = 2 + rand() % 6;
        debris.speed = 0.05f + (rand() % 15) / 30.0f;
        debris.life = 0.6f + (rand() % 40) / 100.0f;
        debris.alpha = 0.9f;
        
        // Different smoke colors for different stages
        if (stageNum == 1) {
            // Orange/brown smoke for stage 1 separation
            debris.r = 0.8f + (rand() % 20) / 100.0f;
            debris.g = 0.4f + (rand() % 30) / 100.0f;
            debris.b = 0.2f + (rand() % 20) / 100.0f;
        } else {
            // Blue/gray smoke for stage 2 separation
            debris.r = 0.6f + (rand() % 20) / 100.0f;
            debris.g = 0.7f + (rand() % 20) / 100.0f;
            debris.b = 0.9f + (rand() % 10) / 100.0f;
        }
        
        smokeParticles.push_back(debris);
    }
    
    // Also add explosion particles for more dramatic effect
    for(int i = 0; i < 15; i++) {
        ExplosionParticle particle;
        particle.x = x;
        particle.y = y;
        particle.vx = (rand() % 200 - 100) / 25.0f;
        particle.vy = (rand() % 200 - 100) / 25.0f;
        particle.size = 1 + rand() % 3;
        particle.life = 0.9f;
        
        if (stageNum == 1) {
            particle.r = 1.0f; particle.g = 0.6f; particle.b = 0.0f; // Orange
        } else {
            particle.r = 0.4f; particle.g = 0.6f; particle.b = 1.0f; // Blue
        }
        
        explosionParticles.push_back(particle);
    }
    
    std::cout << "STAGE " << stageNum << " SEPARATED!" << std::endl;
}

// ========== ANIMATION FUNCTIONS ==========

void updateStars() {
    for(auto& star : stars) {
        star.brightness = 0.5f + 0.5f * fabs(sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f * star.twinkleSpeed));
    }
}

void updateSmoke() {
    // Add new smoke particles during launch
    if (rocketLaunching && rocketFuel > 0 && rocketStage == 1) {
        // Stage 1 smoke - white/gray
        for(int i = 0; i < 4; i++) {
            SmokeParticle smoke;
            smoke.x = rocketX + (rand() % 40 - 20) * 0.5f;
            smoke.y = rocketY - 20 - (rand() % 15);
            smoke.size = 5 + rand() % 10;
            smoke.speed = 0.1f + (rand() % 10) / 20.0f;
            smoke.life = 0.8f + (rand() % 30) / 100.0f;
            smoke.alpha = 0.9f;
            
            // White/gray smoke for stage 1
            float gray = 0.7f + (rand() % 30) / 100.0f;
            smoke.r = gray;
            smoke.g = gray;
            smoke.b = gray;
            
            smokeParticles.push_back(smoke);
        }
    } 
    else if (rocketLaunching && rocketFuel > 0 && rocketStage == 2) {
        // Stage 2 smoke - cleaner, whiter smoke
        for(int i = 0; i < 2; i++) {
            SmokeParticle smoke;
            smoke.x = rocketX + (rand() % 30 - 15) * 0.3f;
            smoke.y = rocketY - 15 - (rand() % 10);
            smoke.size = 4 + rand() % 6;
            smoke.speed = 0.15f + (rand() % 10) / 25.0f;
            smoke.life = 0.6f + (rand() % 20) / 100.0f;
            smoke.alpha = 0.8f;
            
            // Clean white smoke for stage 2
            smoke.r = 0.95f;
            smoke.g = 0.95f;
            smoke.b = 0.95f;
            
            smokeParticles.push_back(smoke);
        }
    }
    else if (rocketLaunching && rocketFuel > 0 && rocketStage == 3) {
        // Payload smoke - very light, almost invisible
        if (rand() % 5 == 0) {
            SmokeParticle smoke;
            smoke.x = rocketX + (rand() % 20 - 10) * 0.2f;
            smoke.y = rocketY - 10 - (rand() % 8);
            smoke.size = 2 + rand() % 4;
            smoke.speed = 0.2f;
            smoke.life = 0.4f + (rand() % 10) / 100.0f;
            smoke.alpha = 0.6f;
            
            // Very light smoke for payload
            smoke.r = 1.0f;
            smoke.g = 1.0f;
            smoke.b = 1.0f;
            
            smokeParticles.push_back(smoke);
        }
    }
    
    // Add smoke during stage separation explosions
    for(auto& particle : explosionParticles) {
        if (particle.life > 0.5f) {
            SmokeParticle smoke;
            smoke.x = particle.x;
            smoke.y = particle.y;
            smoke.size = particle.size * 2.0f;
            smoke.speed = 0.05f;
            smoke.life = particle.life * 0.8f;
            smoke.alpha = 0.7f;
            
            // Dark smoke for explosions
            float dark = 0.3f + (rand() % 20) / 100.0f;
            smoke.r = dark;
            smoke.g = dark * 0.8f;
            smoke.b = dark * 0.6f;
            
            smokeParticles.push_back(smoke);
        }
    }
    
    // Update existing smoke particles
    for(auto& smoke : smokeParticles) {
        // Drift upward with some randomness
        smoke.y += smoke.speed;
        smoke.x += (rand() % 20 - 10) * 0.03f * windDirection;
        
        // Slowly fade out
        smoke.life -= 0.005f;
        smoke.alpha = smoke.life;
        
        // Smoke expands as it rises
        smoke.size += 0.15f;
        
        // Add some organic movement
        float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
        smoke.x += sin(time * 0.5f + smoke.y * 0.01f) * 0.1f;
        smoke.y += cos(time * 0.3f + smoke.x * 0.01f) * 0.05f;
    }
    
    // Remove dead particles
    smokeParticles.erase(
        std::remove_if(smokeParticles.begin(), smokeParticles.end(),
            [](const SmokeParticle& s) { return s.life <= 0.0f || s.alpha <= 0.0f; }),
        smokeParticles.end()
    );
}

void updateClouds() {
    for(auto& cloud : clouds) {
        cloud.x += cloud.speed * windSpeed * windDirection;
        if (cloud.x > 850) {
            cloud.x = -50;
            cloud.y = 200 + rand() % 150;
            cloud.opacity = 0.4f + (rand() % 60) / 100.0f;
        } else if (cloud.x < -50) {
            cloud.x = 850;
            cloud.y = 200 + rand() % 150;
            cloud.opacity = 0.4f + (rand() % 60) / 100.0f;
        }
        
        // Float up and down slightly
        cloud.y += 0.05f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f + cloud.x * 0.01f);
    }
}

void updateSatellites() {
    for(auto& sat : satellites) {
        sat.x += sat.speed;
        sat.angle += 1.0f;
        
        if (sat.x > 850) {
            sat.x = -50;
            sat.y = 400 + rand() % 200;
        }
    }
}

void updateMeteors() {
    for(auto& meteor : meteors) {
        meteor.x += meteor.speed;
        meteor.y -= meteor.speed * 0.5f;
        
        // Update trail
        meteor.trailX.insert(meteor.trailX.begin(), meteor.x);
        meteor.trailY.insert(meteor.trailY.begin(), meteor.y);
        if (meteor.trailX.size() > meteor.trailLength) {
            meteor.trailX.pop_back();
            meteor.trailY.pop_back();
        }
        
        if (meteor.x > 850 || meteor.y < -50) {
            meteor.x = rand() % 800;
            meteor.y = 500 + rand() % 100;
            meteor.trailX.clear();
            meteor.trailY.clear();
            for(int j = 0; j < meteor.trailLength; j++) {
                meteor.trailX.push_back(meteor.x - j * 2);
                meteor.trailY.push_back(meteor.y - j * 2);
            }
        }
    }
}

void updateExplosion() {
    for(auto& particle : explosionParticles) {
        particle.x += particle.vx;
        particle.y += particle.vy;
        particle.vy -= 0.05f; // Gravity
        particle.life -= 0.01f;
        particle.size += 0.1f;
    }
    
    explosionParticles.erase(
        std::remove_if(explosionParticles.begin(), explosionParticles.end(),
            [](const ExplosionParticle& p) { return p.life <= 0.0f; }),
        explosionParticles.end()
    );
}

void updateSeparatedStages() {
    // Update stage 1 if separated
    if (stage1Separated && stage1Falling && stage1.falling) {
        // Stage 1 should fall DOWN (negative Y direction)
        stage1.velocity -= 0.08f; // Stronger gravity for stage 1
        stage1.y += stage1.velocity;
        
        // Add some horizontal drift - make it visible
        stage1.x += (windSpeed * windDirection * 2.0f) + (sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f) * 0.5f);
        
        // Add rotation while falling - make it spin
        stage1.angle += 5.0f + sin(glutGet(GLUT_ELAPSED_TIME) * 0.002f) * 2.0f;
        
        // Deploy parachute at certain altitude
        if (stage1.y < 400 && stage1.velocity < -1.5f) {
            stage1.velocity *= 0.97f; // Parachute slows descent more
        }
        
        // Add smoke trail from falling stage - make it more visible
        if (rand() % 5 == 0) {
            SmokeParticle smoke;
            smoke.x = stage1.x + (rand() % 20 - 10);
            smoke.y = stage1.y + (rand() % 20 - 10);
            smoke.size = 3 + rand() % 6;
            smoke.speed = 0.2f;
            smoke.life = 0.5f;
            smoke.alpha = 0.6f;
            smoke.r = 0.6f;
            smoke.g = 0.6f;
            smoke.b = 0.6f;
            smokeParticles.push_back(smoke);
        }
        
        // Check if hit ground
        if (stage1.y <= 150.0f) {
            stage1Falling = false;
            stage1.falling = false;
            createExplosion(stage1.x, stage1.y);
            std::cout << "Stage 1 CRASHED at X=" << stage1.x << " Y=" << stage1.y << std::endl;
        }
    }
    
    // Update stage 2 if separated
    if (stage2Separated && stage2Falling && stage2.falling) {
        // Stage 2 should also fall DOWN
        stage2.velocity -= 0.07f; // Slightly less gravity
        stage2.y += stage2.velocity;
        
        // Add some horizontal drift
        stage2.x += (windSpeed * windDirection * 1.5f) + (cos(glutGet(GLUT_ELAPSED_TIME) * 0.001f) * 0.3f);
        
        // Add rotation while falling
        stage2.angle += 6.0f + cos(glutGet(GLUT_ELAPSED_TIME) * 0.003f) * 3.0f;
        
        // Add smoke trail
        if (rand() % 8 == 0) {
            SmokeParticle smoke;
            smoke.x = stage2.x + (rand() % 15 - 7);
            smoke.y = stage2.y + (rand() % 15 - 7);
            smoke.size = 2 + rand() % 4;
            smoke.speed = 0.15f;
            smoke.life = 0.4f;
            smoke.alpha = 0.5f;
            smoke.r = 0.7f;
            smoke.g = 0.7f;
            smoke.b = 0.7f;
            smokeParticles.push_back(smoke);
        }
        
        // Check if hit ground
        if (stage2.y <= 150.0f) {
            stage2Falling = false;
            stage2.falling = false;
            createExplosion(stage2.x, stage2.y);
            std::cout << "Stage 2 CRASHED at X=" << stage2.x << " Y=" << stage2.y << std::endl;
        }
    }
}

void separateStage(int stageNum) {
    if (stageNum == 1 && !stage1Separated) {
        stage1Separated = true;
        stage1Falling = true;
        stage1.falling = true;
        stage1.x = rocketX;
        stage1.y = rocketY;
        stage1.velocity = rocketVelocity * 0.7f; // Keep some momentum
        stage1.angle = rocketAngle + (rand() % 20 - 10); // Random tilt
        createStageSeparationEffect(rocketX, rocketY, 1);
        
        // Stage 2 becomes active
        rocketStage = 2;
        rocketFuel = 100.0f; // Full fuel for stage 2
        flameSize = 25.0f; // Medium flame for stage 2
        
        // Small velocity loss during separation (realistic)
        rocketVelocity *= 0.95f;
        
        // Update stage 2 position
        stage2.active = true;
        stage2.x = rocketX;
        stage2.y = rocketY;
        stage2.velocity = rocketVelocity;
        stage2.angle = rocketAngle;
        
        std::cout << "\n=== STAGE 1 SEPARATION SUCCESS ===" << std::endl;
        std::cout << "Stage 2 now ACTIVE with BLUE flames" << std::endl;
        std::cout << "Stage 2 position: Y=" << (int)rocketY << "m" << std::endl;
        
        // Auto-switch camera to follow stage 1
        if (cameraFollowRocket) {
            currentCameraMode = MODE_FOLLOW_STAGE1;
            std::cout << "Camera: Following Stage 1 (falling)" << std::endl;
            
            // Set timer to switch camera back to rocket after 3 seconds
            glutTimerFunc(3000, [](int value) {
                if (!rocketExploded && currentCameraMode == MODE_FOLLOW_STAGE1) {
                    currentCameraMode = MODE_FOLLOW_ROCKET;
                    // ZOOM IN when switching to stage 2
                    cameraTargetZoom = 0.8f; // Zoom in to see stage 2 clearly
                    std::cout << "\nCamera: Switched to Stage 2 (BLUE flames)" << std::endl;
                    std::cout << "Stage 2 altitude: " << (int)altitude << "m" << std::endl;
                    std::cout << "Stage 2 fuel: " << (int)rocketFuel << "%" << std::endl;
                }
            }, 0);
        }
    }
    else if (stageNum == 2 && !stage2Separated) {
        stage2Separated = true;
        stage2Falling = true;
        stage2.falling = true;
        stage2.x = rocketX;
        stage2.y = rocketY;
        stage2.velocity = rocketVelocity * 0.7f; // Keep some momentum
        stage2.angle = rocketAngle + (rand() % 20 - 10); // Random tilt
        createStageSeparationEffect(rocketX, rocketY, 2);
        
        // IMPORTANT: Move to PAYLOAD (Stage 3)
        rocketStage = 3;
        rocketFuel = 50.0f; // Limited fuel for payload
        flameSize = 15.0f; // Small PURPLE flame for payload
        
        // Very small velocity loss for payload separation
        rocketVelocity *= 0.98f;
        
        // IMPORTANT: Payload continues from the SAME position
        // (rocketX, rocketY) stays the same - only the stage number changes
        
        // Update payload (stage 3) position
        stage3.active = true;
        stage3.x = rocketX;
        stage3.y = rocketY; // Payload continues from same Y position
        stage3.velocity = rocketVelocity;
        stage3.angle = rocketAngle;
        
        std::cout << "\n=== STAGE 2 SEPARATION SUCCESS ===" << std::endl;
        std::cout << "PAYLOAD now ACTIVE with PURPLE flames" << std::endl;
        std::cout << "Payload position: Y=" << (int)rocketY << "m" << std::endl;
        std::cout << "Payload fuel: 50%" << std::endl;
        std::cout << "Look for the SILVER payload with PURPLE flame!" << std::endl;
        std::cout << "ORBIT ACHIEVED!" << std::endl;
        
        // Auto-switch camera to follow stage 2
        if (cameraFollowRocket) {
            currentCameraMode = MODE_FOLLOW_STAGE2;
            std::cout << "Camera: Following Stage 2 (falling)" << std::endl;
            
            // Set timer to switch camera back to PAYLOAD after 3 seconds
            glutTimerFunc(3000, [](int value) {
                if (!rocketExploded && currentCameraMode == MODE_FOLLOW_STAGE2) {
                    currentCameraMode = MODE_FOLLOW_ROCKET;
                    // IMPORTANT: ZOOM IN CLOSER for payload
                    cameraTargetZoom = 1.2f; // Zoom in more for small payload
                    cameraX = rocketX; // Center on rocket
                    cameraY = rocketY + 80; // Adjust Y offset to see payload better
                    
                    std::cout << "\nCamera: Switched to PAYLOAD (PURPLE flames)" << std::endl;
                    std::cout << "Camera ZOOMED IN to 1.2x to see payload clearly" << std::endl;
                    std::cout << "Payload altitude: " << (int)altitude << "m" << std::endl;
                    std::cout << "Payload fuel: " << (int)rocketFuel << "%" << std::endl;
                    std::cout << "Look for small PURPLE flame below silver payload!" << std::endl;
                }
            }, 0);
        }
        
        // Update game state
        currentState = ORBIT;
    }
}

void updateRocket() {
    if (rocketExploded) return;
    
    // Update separated stages
    updateSeparatedStages();
    
    // Update trajectory history
    if (showTrajectory || rocketLaunching) {
        trajectoryPoints.push_back({rocketX, rocketY});
        if (trajectoryPoints.size() > 200) {
            trajectoryPoints.erase(trajectoryPoints.begin());
        }
    }
    
    if (rocketLaunching && rocketFuel > 0) {
        // Update physics
        float thrust = 0.0f;
        
        // Different thrust for different stages
        if (rocketStage == 1) {
            thrust = 0.12f; // Strong thrust for stage 1
        } else if (rocketStage == 2) {
            thrust = 0.09f; // Medium thrust for stage 2
        } else {
            thrust = 0.06f; // Weak thrust for payload
        }
        
        rocketVelocity += thrust;
        rocketFuel -= 0.4f; // Fuel consumption
        
        // Apply wind effect (less effect at higher altitudes)
        float windEffect = windSpeed * windDirection * 0.1f * (1.0f - altitude / 1000.0f);
        rocketX += windEffect;
        
        // Slight rotation due to wind
        rocketAngle += windEffect * 0.5f;
        
        // Limit angle
        if (rocketAngle > 10.0f) rocketAngle = 10.0f;
        if (rocketAngle < -10.0f) rocketAngle = -10.0f;
        
        rocketY += rocketVelocity;
        
        // Update telemetry
        altitude = rocketY - 150.0f;
        velocity = rocketVelocity;
        acceleration = thrust * 100; // Convert to m/s²
        
        // DEBUG: Print current state for troubleshooting
        static int debugCounter = 0;
        debugCounter++;
        if (debugCounter >= 30) { // Print every 30 frames
            std::cout << "DEBUG - Stage: " << rocketStage 
                      << " | Alt: " << (int)altitude << "m" 
                      << " | Fuel: " << (int)rocketFuel << "%"
                      << " | Vel: " << std::fixed << std::setprecision(1) << velocity << "m/s"
                      << " | Stage1Sep: " << (stage1Separated ? "Y" : "N")
                      << " | Stage2Sep: " << (stage2Separated ? "Y" : "N") << std::endl;
            debugCounter = 0;
        }
        
        // ===== STAGE SEPARATION LOGIC =====
        // Stage 1 separation conditions
        if (rocketStage == 1 && !stage1Separated) {
            bool shouldSeparate = false;
            std::string reason = "";
            
            // Condition 1: Altitude + Fuel + Velocity
            if (altitude > 250.0f && rocketFuel < 50.0f && velocity > 3.0f) {
                shouldSeparate = true;
                reason = "Normal separation (Alt>250m, Fuel<50%, Vel>3m/s)";
            }
            // Condition 2: Very low fuel
            else if (rocketFuel < 10.0f) {
                shouldSeparate = true;
                reason = "Emergency separation (Fuel<10%)";
            }
            // Condition 3: Very high altitude
            else if (altitude > 400.0f) {
                shouldSeparate = true;
                reason = "Altitude separation (Alt>400m)";
            }
            
            if (shouldSeparate) {
                std::cout << "\n=== STAGE 1 SEPARATION TRIGGERED ===" << std::endl;
                std::cout << "Reason: " << reason << std::endl;
                std::cout << "Current: Alt=" << (int)altitude << "m, Fuel=" << (int)rocketFuel 
                          << "%, Vel=" << std::fixed << std::setprecision(1) << velocity << "m/s" << std::endl;
                separateStage(1);
            }
        }
        // Stage 2 separation conditions
        else if (rocketStage == 2 && !stage2Separated) {
            bool shouldSeparate = false;
            std::string reason = "";
            
            // Condition 1: Altitude + Fuel + Velocity
            if (altitude > 500.0f && rocketFuel < 40.0f && velocity > 4.0f) {
                shouldSeparate = true;
                reason = "Normal separation (Alt>500m, Fuel<40%, Vel>4m/s)";
            }
            // Condition 2: Very low fuel
            else if (rocketFuel < 15.0f) {
                shouldSeparate = true;
                reason = "Emergency separation (Fuel<15%)";
            }
            // Condition 3: Very high altitude
            else if (altitude > 700.0f) {
                shouldSeparate = true;
                reason = "Altitude separation (Alt>700m)";
            }
            
            if (shouldSeparate) {
                std::cout << "\n=== STAGE 2 SEPARATION TRIGGERED ===" << std::endl;
                std::cout << "Reason: " << reason << std::endl;
                std::cout << "Current: Alt=" << (int)altitude << "m, Fuel=" << (int)rocketFuel 
                          << "%, Vel=" << std::fixed << std::setprecision(1) << velocity << "m/s" << std::endl;
                separateStage(2);
                currentState = ORBIT;
            }
        }
        
        // Animate flames - different sizes for different stages
        float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
        if (rocketStage == 1) {
            flameSize = 40.0f + 15.0f * sin(time * 0.01f);
        } else if (rocketStage == 2) {
            flameSize = 25.0f + 8.0f * sin(time * 0.015f);
        } else {
            flameSize = 15.0f + 5.0f * sin(time * 0.02f);
        }
        flameIntensity = 0.5f + 0.5f * sin(time * 0.02f);
        
        // Random chance of failure (for excitement!)
        if (rand() % 10000 < 2 && !rocketExploded) { // 0.02% chance per frame
            createExplosion(rocketX, rocketY);
        }
        
        // Check mission success
        if (rocketY > targetAltitude && rocketVelocity > targetVelocity && !rocketExploded) {
            missionSuccess = true;
            currentState = MISSION_COMPLETE;
            std::cout << "\n=== MISSION SUCCESS ===" << std::endl;
            std::cout << "Target altitude reached: " << (int)altitude << "m" << std::endl;
            std::cout << "Target velocity reached: " << std::fixed << std::setprecision(1) << velocity << "m/s" << std::endl;
        }
        
        if (rocketY > 1000) {
            rocketLaunched = true;
            rocketLaunching = false;
            std::cout << "\n=== MAXIMUM ALTITUDE REACHED ===" << std::endl;
        }
    }
    else if (rocketLaunching && rocketFuel <= 0) {
        // Out of fuel but still launching (coasting)
        rocketVelocity -= 0.02f; // Reduced gravity at high altitude
        rocketY += rocketVelocity;
        
        // Update telemetry
        altitude = rocketY - 150.0f;
        
        // Emergency separation if out of fuel
        if (rocketStage == 1 && !stage1Separated && altitude > 200.0f) {
            std::cout << "\n=== EMERGENCY: STAGE 1 SEPARATION (OUT OF FUEL) ===" << std::endl;
            separateStage(1);
        }
        else if (rocketStage == 2 && !stage2Separated && altitude > 450.0f) {
            std::cout << "\n=== EMERGENCY: STAGE 2 SEPARATION (OUT OF FUEL) ===" << std::endl;
            separateStage(2);
            currentState = ORBIT;
        }
    }
    
    // Apply gravity if not launching (falling back)
    if (!rocketLaunching && rocketY > 150.0f && !rocketExploded) {
        rocketVelocity -= 0.05f; // Gravity
        rocketY += rocketVelocity;
        
        // Update altitude even when falling
        altitude = rocketY - 150.0f;
        
        // Crash detection
        if (rocketY <= 150.0f && rocketVelocity < -2.0f) {
            createExplosion(rocketX, rocketY);
            std::cout << "\n=== ROCKET CRASHED ===" << std::endl;
        }
        else if (rocketY <= 150.0f) {
            // Soft landing
            rocketY = 150.0f;
            rocketVelocity = 0.0f;
            rocketLaunching = false;
            rocketLaunched = false;
            std::cout << "\n=== ROCKET LANDED ===" << std::endl;
        }
    }
    
    // Update stage positions relative to rocket
    if (!stage1Separated && rocketStage == 1) {
        // Stage 1 is still attached
        stage1.x = rocketX;
        stage1.y = rocketY;
        stage1.velocity = rocketVelocity;
        stage1.angle = rocketAngle;
    }
    
    if (!stage2Separated && rocketStage <= 2) {
        // Stage 2 is still attached (for stage 1 or 2)
        stage2.x = rocketX;
        stage2.y = rocketY + (rocketStage == 1 ? 150.0f : 0.0f);
        stage2.velocity = rocketVelocity;
        stage2.angle = rocketAngle;
    }
    
    if (!stage3.separated) {
        // Stage 3 (payload) is still attached
        stage3.x = rocketX;
        stage3.y = rocketY + (rocketStage == 1 ? 220.0f : (rocketStage == 2 ? 70.0f : 0.0f));
        stage3.velocity = rocketVelocity;
        stage3.angle = rocketAngle;
    }
}

void updateCamera() {
    // Set camera targets based on mode
    switch(currentCameraMode) {
        case MODE_LAUNCH_PAD:
            cameraTargetX = LAUNCH_PAD_X;
            cameraTargetY = LAUNCH_PAD_Y + 150;
            cameraTargetZoom = 1.0f;
            break;
            
        case MODE_WIDE_VIEW:
            cameraTargetX = 400.0f;
            cameraTargetY = 300.0f;
            cameraTargetZoom = 0.4f;
            break;
            
        case MODE_FOLLOW_ROCKET:
            if (rocketExploded) {
                cameraTargetX = rocketX;
                cameraTargetY = rocketY;
                cameraTargetZoom = 1.0f;
            } else if (rocketLaunching || rocketLaunched) {
                cameraTargetX = rocketX;
                
                // Adjust Y offset and zoom based on rocket stage
                if (rocketStage == 1) {
                    // Stage 1 - standard view
                    cameraTargetY = rocketY + 100;
                    cameraTargetZoom = 1.0f - (rocketY / 2000.0f);
                } else if (rocketStage == 2) {
                    // Stage 2 - zoom in a bit more
                    cameraTargetY = rocketY + 80;
                    cameraTargetZoom = 0.8f - (rocketY / 2500.0f);
                } else if (rocketStage == 3) {
                    // PAYLOAD - zoom in MUCH more to see small payload
                    cameraTargetY = rocketY + 60;
                    cameraTargetZoom = 1.2f - (rocketY / 3000.0f);
                }
                
                // Clamp zoom values
                if (cameraTargetZoom < 0.3f) cameraTargetZoom = 0.3f;
                if (cameraTargetZoom > 2.0f) cameraTargetZoom = 2.0f;
                
                // Special case: if payload is very high, don't zoom out too much
                if (rocketStage == 3 && rocketY > 1000.0f) {
                    cameraTargetZoom = 0.8f; // Keep payload visible
                }
            } else {
                // Pre-launch view
                cameraTargetX = rocketX;
                cameraTargetY = rocketY + 50;
                cameraTargetZoom = 1.0f;
            }
            break;
            
        case MODE_FOLLOW_STAGE1:
            if (stage1Separated && stage1.falling) {
                cameraTargetX = stage1.x;
                cameraTargetY = stage1.y + 100;
                cameraTargetZoom = 0.8f;
            } else {
                // Stage 1 has crashed, switch back to rocket
                currentCameraMode = MODE_FOLLOW_ROCKET;
                cameraTargetZoom = 0.8f; // Zoom in for stage 2
                std::cout << "Stage 1 lost, camera switching to current rocket stage" << std::endl;
                return;
            }
            break;
            
        case MODE_FOLLOW_STAGE2:
            if (stage2Separated && stage2.falling) {
                cameraTargetX = stage2.x;
                cameraTargetY = stage2.y + 100;
                cameraTargetZoom = 0.9f;
            } else {
                // Stage 2 has crashed, switch back to rocket
                currentCameraMode = MODE_FOLLOW_ROCKET;
                cameraTargetZoom = 1.2f; // Zoom in for payload
                std::cout << "Stage 2 lost, camera switching to PAYLOAD" << std::endl;
                return;
            }
            break;
    }
    
    // Smooth camera movement
    float smoothFactor = 0.05f;
    cameraX += (cameraTargetX - cameraX) * smoothFactor;
    cameraY += (cameraTargetY - cameraY) * smoothFactor;
    cameraZoom += (cameraTargetZoom - cameraZoom) * smoothFactor;
    
    // Camera shake effect
    if (cameraShake && shakeIntensity > 0.0f) {
        cameraX += (rand() % 100 - 50) * 0.01f * shakeIntensity;
        cameraY += (rand() % 100 - 50) * 0.01f * shakeIntensity;
        shakeIntensity *= 0.9f;
        if (shakeIntensity < 0.01f) {
            cameraShake = false;
            shakeIntensity = 0.0f;
        }
    }
}

void updateDayNight() {
    if (dayNightCycle) {
        dayTime += 0.0001f;
        if (dayTime > 1.0f) dayTime = 0.0f;
    }
}

// ========== GLUT CALLBACKS ==========

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Apply camera transformations
    glPushMatrix();
    glTranslatef(400, 300, 0); // Center of screen
    glScalef(cameraZoom, cameraZoom, 1.0f);
    glTranslatef(-cameraX, -cameraY, 0);
    
    // Draw scene
    drawSky();
    drawStars();
    drawGrid();
    drawGround();
    drawLaunchComplex();
    drawClouds();
    drawSatellites();
    drawMeteors();
    drawControlTower();
    drawTrajectory();
    drawRocket();
    drawFlames();
    drawSmoke();
    drawExplosion();
    
    glPopMatrix();
    
    // Draw UI elements (not affected by camera)
    drawUI();
    if (countdown > 0 && countdown <= 10) {
        drawCountdown();
    }
    
    glutSwapBuffers();
}

void timer(int value) {
    // Check for continuous key presses
    static int zoomCounter = 0;
    zoomCounter++;
    
    if (zoomCounter >= 2) { // Process zoom every 2 frames for smoother control
        if (keys['+'] || keys['=']) {
            cameraZoom *= 1.02f;
            cameraTargetZoom = cameraZoom; // Keep target zoom in sync
            if (cameraZoom > 3.0f) {
                cameraZoom = 3.0f;
                cameraTargetZoom = 3.0f;
            }
        }
        if (keys['-'] || keys['_']) {
            cameraZoom *= 0.98f;
            cameraTargetZoom = cameraZoom; // Keep target zoom in sync
            if (cameraZoom < 0.1f) {
                cameraZoom = 0.1f;
                cameraTargetZoom = 0.1f;
            }
        }
        zoomCounter = 0;
    }
    
    // Update countdown
    if (countdown > 0 && currentState == COUNTDOWN) {
        countdown--;
        if (countdown == 0) {
            rocketLaunching = true;
            currentState = LAUNCH;
            cameraFollowRocket = true;
            initializeStages(); // Initialize stages at launch
            std::cout << "=== LIFTOFF! ===" << std::endl;
        }
        
        // Camera shake during countdown
        if (countdown <= 3 && countdown > 0) {
            cameraShake = true;
            shakeIntensity = 0.5f * (4 - countdown);
        }
    }
    
    // Update animations
    updateStars();
    updateClouds();
    updateSatellites();
    updateMeteors();
    updateSmoke();
    updateExplosion();
    updateRocket();
    updateCamera();
    updateDayNight();
    
    // Blink control tower light
    lightTimer++;
    if (lightTimer >= 15) {
        lightOn = !lightOn;
        lightTimer = 0;
    }
    
    // Change wind occasionally
    if (rand() % 1000 == 0) {
        windDirection *= -1.0f;
        windSpeed = 0.05f + (rand() % 10) / 100.0f;
    }
    
    // Redraw
    glutPostRedisplay();
    
    // Call timer again
    glutTimerFunc(16, timer, 0);  // ~60 FPS
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true; // Mark key as pressed
    
    switch(key) {
        case ' ': // Space - start countdown
            if (currentState == PRELAUNCH) {
                currentState = COUNTDOWN;
                countdown = 10;
                std::cout << "Countdown started!" << std::endl;
            }
            break;
            
        case 'r':
        case 'R': // Reset
            rocketX = 400.0f;
            rocketY = 150.0f;
            rocketVelocity = 0.0f;
            rocketAngle = 0.0f;
            rocketLaunched = false;
            rocketLaunching = false;
            rocketExploded = false;
            rocketFuel = 100.0f;
            rocketStage = 1;
            countdown = 10;
            altitude = 0.0f;
            velocity = 0.0f;
            acceleration = 0.0f;
            currentState = PRELAUNCH;
            cameraFollowRocket = false;
            cameraX = 400.0f;
            cameraY = 300.0f;
            cameraZoom = 1.0f;
            cameraTargetX = 400.0f;
            cameraTargetY = 300.0f;
            cameraTargetZoom = 1.0f;
            currentCameraMode = MODE_LAUNCH_PAD;
            missionSuccess = false;
            stage1Separated = false;
            stage2Separated = false;
            stage1Falling = false;
            stage2Falling = false;
            smokeParticles.clear();
            explosionParticles.clear();
            trajectoryPoints.clear();
            showTrajectory = false;
            initializeStages(); // Re-initialize stages
            std::cout << "=== SIMULATION RESET ===" << std::endl;
            break;
            
        case 'g':
        case 'G': // Toggle grid
            showGrid = !showGrid;
            std::cout << "Grid: " << (showGrid ? "ON" : "OFF") << std::endl;
            break;
            
        case 't':
        case 'T': // Toggle trajectory
            if (key == 't') {
                // Toggle trajectory
                showTrajectory = !showTrajectory;
                std::cout << "Trajectory: " << (showTrajectory ? "ON" : "OFF") << std::endl;
            } else if (key == 'T') {
                // Test separation (Shift+T)
                testSeparation();
            }
            break;
            
        case 'c':
        case 'C': // Toggle camera follow
            cameraFollowRocket = !cameraFollowRocket;
            if (cameraFollowRocket) {
                currentCameraMode = MODE_FOLLOW_ROCKET;
                std::cout << "Camera: Following Rocket" << std::endl;
            } else {
                currentCameraMode = MODE_LAUNCH_PAD;
                std::cout << "Camera: Fixed on Launch Pad" << std::endl;
            }
            break;
            
        case 'd':
        case 'D': // Toggle day/night cycle
            dayNightCycle = !dayNightCycle;
            std::cout << "Day/Night cycle: " << (dayNightCycle ? "ON" : "OFF") << std::endl;
            break;
            
        case '+':
        case '=': // Zoom in
            cameraZoom *= 1.1f;
            if (cameraZoom > 3.0f) cameraZoom = 3.0f;
            // Update target zoom to maintain position
            cameraTargetZoom = cameraZoom;
            std::cout << "Zoom in: " << cameraZoom << std::endl;
            break;
            
        case '-':
        case '_': // Zoom out
            cameraZoom *= 0.9f;
            if (cameraZoom < 0.1f) cameraZoom = 0.1f;
            // Update target zoom to maintain position
            cameraTargetZoom = cameraZoom;
            std::cout << "Zoom out: " << cameraZoom << std::endl;
            break;
            
        case '1': // View 1: Launch pad
            currentCameraMode = MODE_LAUNCH_PAD;
            cameraFollowRocket = false;
            cameraTargetX = 400.0f;
            cameraTargetY = 300.0f;
            cameraTargetZoom = 1.0f;
            cameraX = 400.0f;
            cameraY = 300.0f;
            cameraZoom = 1.0f;
            std::cout << "View 1: Launch pad (Zoom: 1.0)" << std::endl;
            break;
            
        case '2': // View 2: Wide shot
            currentCameraMode = MODE_WIDE_VIEW;
            cameraFollowRocket = false;
            cameraTargetX = 400.0f;
            cameraTargetY = 300.0f;
            cameraTargetZoom = 0.4f;
            cameraX = 400.0f;
            cameraY = 300.0f;
            cameraZoom = 0.4f;
            std::cout << "View 2: Wide shot (Zoom: 0.4)" << std::endl;
            break;
            
        case '3': // View 3: Rocket follow
            currentCameraMode = MODE_FOLLOW_ROCKET;
            cameraFollowRocket = true;
            std::cout << "View 3: Rocket follow" << std::endl;
            break;
            
        case '4': // View 4: Follow Stage 1
            if (stage1Separated) {
                currentCameraMode = MODE_FOLLOW_STAGE1;
                cameraFollowRocket = true;
                std::cout << "View 4: Following Stage 1" << std::endl;
            } else {
                std::cout << "Stage 1 not separated yet!" << std::endl;
            }
            break;
            
        case '5': // View 5: Follow Stage 2
            if (stage2Separated) {
                currentCameraMode = MODE_FOLLOW_STAGE2;
                cameraFollowRocket = true;
                std::cout << "View 5: Following Stage 2" << std::endl;
            } else {
                std::cout << "Stage 2 not separated yet!" << std::endl;
            }
            break;
            
        case 's':
        case 'S': // Toggle stats
            showStats = !showStats;
            std::cout << "Stats display: " << (showStats ? "ON" : "OFF") << std::endl;
            break;
            
        case 'f':
        case 'F': // Add fuel (cheat)
            rocketFuel += 20.0f;
            if (rocketFuel > 100.0f) rocketFuel = 100.0f;
            std::cout << "Fuel added: " << rocketFuel << "%" << std::endl;
            break;
            
        case 'p':
        case 'P': // Manual stage separation (for testing)
            if (rocketLaunching) {
                if (rocketStage == 1 && !stage1Separated) {
                    std::cout << "\n=== MANUAL STAGE 1 SEPARATION ===" << std::endl;
                    separateStage(1);
                }
                else if (rocketStage == 2 && !stage2Separated) {
                    std::cout << "\n=== MANUAL STAGE 2 SEPARATION ===" << std::endl;
                    separateStage(2);
                }
                else {
                    std::cout << "No stages left to separate!" << std::endl;
                }
            } else {
                std::cout << "Rocket not launching yet!" << std::endl;
            }
            break;
            
        case 'a':
        case 'A': // Auto-switch camera when stages separate
            if (stage1Separated && !stage1Falling) {
                currentCameraMode = MODE_FOLLOW_STAGE1;
                std::cout << "Auto-switched to Stage 1 camera" << std::endl;
            }
            break;

        case 'i':
        case 'I': // Info/debug
            std::cout << "\n=== ROCKET STATUS ===" << std::endl;
            std::cout << "Current Stage: " << rocketStage << std::endl;
            std::cout << "Position: X=" << rocketX << " Y=" << rocketY << std::endl;
            std::cout << "Altitude: " << altitude << "m" << std::endl;
            std::cout << "Fuel: " << rocketFuel << "%" << std::endl;
            std::cout << "Velocity: " << velocity << "m/s" << std::endl;
            std::cout << "Stage1 Separated: " << (stage1Separated ? "YES" : "NO") << std::endl;
            std::cout << "Stage2 Separated: " << (stage2Separated ? "YES" : "NO") << std::endl;
            std::cout << "Camera Mode: " << currentCameraMode << std::endl;
            std::cout << "Flame Size: " << flameSize << std::endl;
            std::cout << "=== END STATUS ===" << std::endl;
            break;

        case 'z':
        case 'Z': // Manual zoom adjustment for payload
            if (rocketStage == 3) {
                // Increase zoom for payload
                cameraTargetZoom *= 1.2f;
                if (cameraTargetZoom > 3.0f) cameraTargetZoom = 3.0f;
                std::cout << "Manual zoom for payload: " << cameraTargetZoom << "x" << std::endl;
            }
            break;
            
        case 'x':
        case 'X': // Manual zoom out
            cameraTargetZoom *= 0.8f;
            if (cameraTargetZoom < 0.1f) cameraTargetZoom = 0.1f;
            std::cout << "Manual zoom out: " << cameraTargetZoom << "x" << std::endl;
            break;
            
        case 27: // ESC - exit
            exit(0);
            break;
            
        default:
            // For debugging: print any key press
            std::cout << "Key pressed: " << key << " (ASCII: " << (int)key << ")" << std::endl;
            break;
    }
    
    // Force redraw
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false; // Mark key as released
}

void specialKeys(int key, int x, int y) {
    // Manual rocket control (for testing)
    if (rocketLaunching && rocketFuel > 0) {
        switch(key) {
            case GLUT_KEY_LEFT:
                rocketAngle -= 0.5f;
                std::cout << "Left arrow: Adjusting rocket angle to " << rocketAngle << " degrees" << std::endl;
                break;
            case GLUT_KEY_RIGHT:
                rocketAngle += 0.5f;
                std::cout << "Right arrow: Adjusting rocket angle to " << rocketAngle << " degrees" << std::endl;
                break;
            case GLUT_KEY_UP:
                rocketVelocity += 0.05f;
                rocketFuel -= 1.0f;
                std::cout << "Up arrow: Increasing thrust, fuel: " << rocketFuel << "%" << std::endl;
                break;
            case GLUT_KEY_DOWN:
                rocketVelocity -= 0.05f;
                std::cout << "Down arrow: Decreasing thrust" << std::endl;
                break;
        }
        
        // Limit angle
        if (rocketAngle > 30.0f) rocketAngle = 30.0f;
        if (rocketAngle < -30.0f) rocketAngle = -30.0f;
    }
}

// ========== MAIN FUNCTION ==========

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Enhanced Space Rocket Launch Station with Stage Separation");
    
    // Set background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable point smoothing
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize game objects
    initializeStars();
    initializeClouds();
    initializeSatellites();
    initializeMeteors();
    
    // Set callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp); // Register key up callback
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, timer, 0);
    
    // Print controls
    // Update the print controls section in main()
    std::cout << "================================================" << std::endl;
    std::cout << "=== SPACE ROCKET LAUNCH STATION CONTROLS ===" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "SPACE: Start countdown" << std::endl;
    std::cout << "R: Reset simulation" << std::endl;
    std::cout << "G: Toggle grid" << std::endl;
    std::cout << "T: Toggle trajectory (shows predicted path)" << std::endl;
    std::cout << "C: Toggle camera follow" << std::endl;
    std::cout << "D: Toggle day/night cycle" << std::endl;
    std::cout << "+ or =: Zoom in (hold for continuous)" << std::endl;
    std::cout << "- or _: Zoom out (hold for continuous)" << std::endl;
    std::cout << "1: View 1 - Launch pad (zoom 1.0)" << std::endl;
    std::cout << "2: View 2 - Wide shot (zoom 0.4)" << std::endl;
    std::cout << "3: View 3 - Rocket follow" << std::endl;
    std::cout << "4: View 4 - Follow Stage 1 (after separation)" << std::endl;
    std::cout << "5: View 5 - Follow Stage 2 (after separation)" << std::endl;
    std::cout << "S: Toggle stats display" << std::endl;
    std::cout << "F: Add fuel (cheat)" << std::endl;
    std::cout << "P: Manual stage separation (for testing)" << std::endl;
    std::cout << "A: Auto-switch camera to falling stage" << std::endl;
    std::cout << "Arrow keys: Manual rocket control during launch" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "FEATURES:" << std::endl;
    std::cout << "- Realistic stage separation with visual effects" << std::endl;
    std::cout << "- Multiple camera views (follow rocket or stages)" << std::endl;
    std::cout << "- Auto-camera switching when stages separate" << std::endl;
    std::cout << "- Falling stages with parachute deployment" << std::endl;
    std::cout << "================================================" << std::endl;
    
    // Start main loop
    glutMainLoop();
    
    return 0;
}