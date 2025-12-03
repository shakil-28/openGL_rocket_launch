// graphics_algorithms.h
#ifndef GRAPHICS_ALGORITHMS_H
#define GRAPHICS_ALGORITHMS_H

// Use more portable OpenGL includes
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <utility>

// Helper function to round to nearest integer
inline int iround(float x) {
    return static_cast<int>(x + 0.5f);
}

// Set pixel functions (declarations only)
void setPixel(int x, int y);
void setPixel(float x, float y);

// ================== DDA LINE DRAWING ==================
void drawLineDDA(int x1, int y1, int x2, int y2);
void drawLineDDA(float x1, float y1, float x2, float y2);

// ================== BRESENHAM'S LINE DRAWING ==================
void drawLineBresenham(int x1, int y1, int x2, int y2);
void drawLineBresenham(float x1, float y1, float x2, float y2);

// ================== MIDPOINT CIRCLE DRAWING ==================
void drawCircleMidpoint(int xc, int yc, int radius);
void drawCircleMidpoint(float xc, float yc, float radius);

// ================== FILLED CIRCLE DRAWING ==================
void drawFilledCircleMidpoint(int xc, int yc, int radius);
void drawFilledCircleMidpoint(float xc, float yc, float radius);

// ================== POLYGON DRAWING ==================
void drawPolygonDDA(const std::vector<std::pair<float, float>>& vertices);
void drawQuadDDA(float x1, float y1, float x2, float y2, 
                 float x3, float y3, float x4, float y4);
void drawTriangleDDA(float x1, float y1, float x2, float y2, float x3, float y3);

// ================== UTILITY FUNCTIONS ==================
void drawFilledRectDDA(float x1, float y1, float x2, float y2);
void drawTriangleFanDDA(float centerX, float centerY, 
                       const std::vector<std::pair<float, float>>& vertices);
void drawQuadStripDDA(const std::vector<std::pair<float, float>>& vertices);
void drawFilledPolygonDDA(const std::vector<std::pair<float, float>>& vertices);
void drawEllipseMidpoint(int xc, int yc, int rx, int ry);

// ================== SAFE HELPER FUNCTIONS (FOR RESOLVING AMBIGUITY) ==================
// These resolve overload ambiguity when calling with integer literals
inline void drawLineDDASafe(float x1, float y1, float x2, float y2) {
    drawLineDDA(x1, y1, x2, y2);
}

inline void drawLineBresenhamSafe(float x1, float y1, float x2, float y2) {
    drawLineBresenham(x1, y1, x2, y2);
}

inline void drawCircleMidpointSafe(float xc, float yc, float radius) {
    drawCircleMidpoint(xc, yc, radius);
}

inline void drawFilledCircleMidpointSafe(float xc, float yc, float radius) {
    drawFilledCircleMidpoint(xc, yc, radius);
}

inline void drawTriangleDDASafe(float x1, float y1, float x2, float y2, float x3, float y3) {
    drawTriangleDDA(x1, y1, x2, y2, x3, y3);
}

inline void drawQuadDDASafe(float x1, float y1, float x2, float y2, 
                           float x3, float y3, float x4, float y4) {
    drawQuadDDA(x1, y1, x2, y2, x3, y3, x4, y4);
}

// ================== TYPE-EXPLICIT HELPERS (ALTERNATIVE TO SAFE FUNCTIONS) ==================
// Alternative approach: Use these if you prefer explicit types
inline void drawLineBresenhamInt(int x1, int y1, int x2, int y2) {
    drawLineBresenham(x1, y1, x2, y2);
}

inline void drawLineBresenhamFloat(float x1, float y1, float x2, float y2) {
    drawLineBresenham(x1, y1, x2, y2);
}

inline void drawLineDDAInt(int x1, int y1, int x2, int y2) {
    drawLineDDA(x1, y1, x2, y2);
}

inline void drawLineDDAFloat(float x1, float y1, float x2, float y2) {
    drawLineDDA(x1, y1, x2, y2);
}

// Add this after the existing declarations:
void drawFilledQuadDDA(float x1, float y1, float x2, float y2, 
                       float x3, float y3, float x4, float y4);
void drawFilledTriangleDDA(float x1, float y1, float x2, float y2, float x3, float y3);

// Safe helper functions
inline void drawFilledQuadDDASafe(float x1, float y1, float x2, float y2, 
                                 float x3, float y3, float x4, float y4) {
    drawFilledQuadDDA(x1, y1, x2, y2, x3, y3, x4, y4);
}

inline void drawFilledTriangleDDASafe(float x1, float y1, float x2, float y2, float x3, float y3) {
    drawFilledTriangleDDA(x1, y1, x2, y2, x3, y3);
}

#endif