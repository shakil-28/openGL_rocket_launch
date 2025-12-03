#include "main_includes.h"

// GLUT callback implementations
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