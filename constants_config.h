#ifndef CONSTANTS_CONFIG_H
#define CONSTANTS_CONFIG_H

// Rocket constants
const float LAUNCH_PAD_X = 400.0f;
const float LAUNCH_PAD_Y = 150.0f;

// Initial values
const float INITIAL_ROCKET_X = LAUNCH_PAD_X;
const float INITIAL_ROCKET_Y = LAUNCH_PAD_Y;
const float INITIAL_ROCKET_FUEL = 100.0f;
const int INITIAL_ROCKET_STAGE = 1;
const int INITIAL_COUNTDOWN = 10;
const float INITIAL_TARGET_ALTITUDE = 800.0f;
const float INITIAL_TARGET_VELOCITY = 5.0f;

// Camera defaults
const float INITIAL_CAMERA_ZOOM = 1.0f;
const float INITIAL_CAMERA_X = 400.0f;
const float INITIAL_CAMERA_Y = 300.0f;

// Environment defaults
const float INITIAL_DAY_TIME = 0.5f;
const float INITIAL_WIND_SPEED = 0.1f;
const float INITIAL_WIND_DIRECTION = 1.0f;

// Graphics constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#endif