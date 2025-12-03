#ifndef FUNCTIONS_DECLARATIONS_H
#define FUNCTIONS_DECLARATIONS_H

#include "types_structures.h"
#include <string>

// ========== GRAPHICS FUNCTIONS ==========
// Basic drawing
void drawSeparatedStage(const RocketStage& stage, int stageNum);
void drawFirstStage();
void drawSecondStage(float yOffset);
void drawThirdStage(float yOffset);
void drawRocket();
void drawFlames();
void drawFlamesForStage(int stageNum);

// Environment drawing
void drawStars();
void drawSky();
void drawGround();
void drawLaunchPad();
void drawLaunchComplex();
void drawControlTower();
void drawGrid();
void drawClouds();
void drawSatellites();
void drawMeteors();
void drawTrajectory();
void drawSmoke();
void drawExplosion();

// UI drawing
void drawText(float x, float y, const std::string& text);
void drawGauge(float x, float y, float value, float maxValue, 
               const std::string& label, float r, float g, float b);
void drawUI();
void drawCountdown();

// ========== INITIALIZATION FUNCTIONS ==========
void initializeStars();
void initializeClouds();
void initializeSatellites();
void initializeMeteors();
void initializeStages();

// ========== UPDATE FUNCTIONS ==========
void updateStars();
void updateClouds();
void updateSatellites();
void updateMeteors();
void updateSmoke();
void updateExplosion();
void updateRocket();
void updateCamera();
void updateDayNight();
void updateSeparatedStages();

// ========== SPECIAL EFFECTS ==========
void createExplosion(float x, float y);
void createStageSeparationEffect(float x, float y, int stageNum);
void separateStage(int stageNum);

// ========== UTILITY FUNCTIONS ==========
void testSeparation();
void switchCameraToRocket(int value);

// ========== INPUT HANDLING ==========
// These will be implemented in main.cpp or input.cpp
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);

#endif