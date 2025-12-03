#include "main_includes.h"
#include "graphics_algorithms.h"

// Drawing functions
void drawSeparatedStage(const RocketStage& stage, int stageNum) {
    if (!stage.falling) return;
    
    glPushMatrix();
    glTranslatef(stage.x, stage.y, 0);
    glRotatef(stage.angle, 0, 0, 1);
    
    if (stageNum == 1) {
        // Draw falling first stage - make it BRIGHT and different color
        glColor3f(1.0f, 0.3f, 0.3f); // Bright red
        drawFilledQuadDDASafe(-20, 0, 20, 0, 20, 150, -20, 150);
        
        // Add some details to make it visible
        glColor3f(0.9f, 0.9f, 0.9f);
        drawFilledQuadDDASafe(-18, 30, -10, 30, -10, 40, -18, 40);
        
        drawFilledQuadDDASafe(10, 30, 18, 30, 18, 40, 10, 40);
        
        // Draw engine details
        glColor3f(0.3f, 0.3f, 0.3f);
        drawFilledQuadDDASafe(-15, 0, 15, 0, 15, -10, -15, -10);
        
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
        drawFilledQuadDDASafe(-15, -2, 15, -2, 15, 68, -15, 68);
        
        // Add some details
        glColor3f(0.9f, 0.9f, 0.9f);
        drawFilledQuadDDASafe(-12, 18, -5, 18, -5, 28, -12, 28);
        
        glBegin(GL_QUADS);
        drawFilledQuadDDASafe(5, 18, 12, 18, 12, 28, 5, 28);
        
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

void drawFirstStage() {
    // First stage (booster) - make it BRIGHT
    glColor3f(1.0f, 0.3f, 0.3f); // Bright red
    drawFilledQuadDDASafe(-20, 0, 20, 0, 20, 150, -20, 150);
    
    // Add white stripes for visibility
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledQuadDDASafe(-18, 30, 18, 30, 18, 40, -18, 40);
    
    drawFilledQuadDDASafe(-18, 90, 18, 90, 18, 100, -18, 100);

    
    // Engine details
    glColor3f(0.1f, 0.1f, 0.1f);
    drawFilledQuadDDASafe(-15, 0, 15, 0, 15, -10, -15, -10);
    
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
    drawFilledQuadDDASafe(-15, yOffset - 2, 15, yOffset - 2, 15, yOffset + 70, -15, yOffset + 70);  // CHANGED
    
    // Add black stripes for visibility
    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledQuadDDASafe(-14, yOffset + 8, 14, yOffset + 8, 14, yOffset + 13, -14, yOffset + 13);
    
    drawFilledQuadDDASafe(-14, yOffset + 38, 14, yOffset + 38, 14, yOffset + 43, -14, yOffset + 43);
    
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
    drawFilledQuadDDASafe(-15, yOffset - 2, 15, yOffset - 2, 15, yOffset + 60, -13, yOffset + 60);
    
    // Add some metallic shine
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledQuadDDASafe(-12, yOffset + 40, -7, yOffset + 40, -7, yOffset + 50, -12, yOffset + 50);
    
    // Fairing (nose cone) - BRIGHT WHITE and larger
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledTriangleDDASafe(-15, yOffset + 60, 15, yOffset + 60, 0, yOffset + 110);
    
    // Details (windows) - VIVID BLUE and larger
    glColor3f(0.0f, 0.9f, 1.0f); // Brighter blue
    drawFilledQuadDDASafe(-6, yOffset + 30, 6, yOffset + 30, 6, yOffset + 40, -6, yOffset + 40);
    
    // Add another window
    drawFilledQuadDDASafe(-6, yOffset + 15, 6, yOffset + 15, 6, yOffset + 25, -6, yOffset + 25);
    
    // Engine nozzle at bottom - larger
    glColor3f(0.3f, 0.3f, 0.3f);
    drawFilledQuadDDASafe(-10, yOffset - 12, 10, yOffset - 12, 10, yOffset - 2, -10, yOffset - 2);
    
    // Add a highlight on the nozzle
    glColor3f(0.5f, 0.5f, 0.5f);
    drawFilledQuadDDASafe(-8, yOffset - 10, -4, yOffset - 10, -4, yOffset - 4, -8, yOffset - 4);
    
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

void drawText(float x, float y, const std::string& text) {
    glColor3f(1.0f, 1.0f, 1.0f); // Ensure white text
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
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
        drawText(10, 560, "BOOSTER DESTROYED - ROCKET LANUCHED!");
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
        rocketStatus = "STATUS: PAYLOAD SUCCESSFULL";
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