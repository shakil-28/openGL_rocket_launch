#ifndef MAIN_INCLUDES_H
#define MAIN_INCLUDES_H

// Standard libraries
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

// Project headers
#include "types_structures.h"
#include "constants_config.h"
#include "globals_extern.h"
#include "graphics_algorithms.h"
#include "functions_declarations.h"

// GLUT callback declarations
void display();
void timer(int value);
void reshape(int width, int height);

#endif