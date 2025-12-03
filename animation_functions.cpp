#include "main_includes.h"

// Initialization functions
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

// Update functions
void updateStars() {
    for(auto& star : stars) {
        star.brightness = 0.5f + 0.5f * fabs(sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f * star.twinkleSpeed));
    }
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

// Special effects
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

// Utility functions
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


void switchCameraToRocket(int value) {
    if (currentCameraMode == MODE_FOLLOW_STAGE1 || currentCameraMode == MODE_FOLLOW_STAGE2) {
        currentCameraMode = MODE_FOLLOW_ROCKET;
        std::cout << "Auto-switching camera back to rocket view" << std::endl;
    }
}
