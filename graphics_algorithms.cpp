#include "graphics_algorithms.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>
#include <limits>

// REMOVED: Duplicate iround function (it's already in the header)

// Set pixel using OpenGL
void setPixel(int x, int y) {
    glVertex2i(x, y);
}

void setPixel(float x, float y) {
    glVertex2f(x, y);
}

// ================== DDA LINE DRAWING ALGORITHM ==================
void drawLineDDA(int x1, int y1, int x2, int y2) {
    // Calculate differences
    int dx = x2 - x1;
    int dy = y2 - y1;
    
    // Calculate steps needed
    int steps = std::max(std::abs(dx), std::abs(dy));
    
    if (steps == 0) {
        setPixel(x1, y1);
        return;
    }
    
    // Calculate increment in x and y for each step
    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;
    
    // Start point
    float x = x1;
    float y = y1;
    
    // Begin drawing points
    glBegin(GL_POINTS);
    for (int i = 0; i <= steps; i++) {
        setPixel(iround(x), iround(y));
        x += xIncrement;
        y += yIncrement;
    }
    glEnd();
}

void drawLineDDA(float x1, float y1, float x2, float y2) {
    drawLineDDA(static_cast<int>(x1), static_cast<int>(y1), 
                static_cast<int>(x2), static_cast<int>(y2));
}

// ================== BRESENHAM'S LINE DRAWING ALGORITHM ==================
void drawLineBresenham(int x1, int y1, int x2, int y2) {
    // Calculate differences
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    
    // Determine direction
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    
    // Initial error
    int err = dx - dy;
    int e2;
    
    // Begin drawing points
    glBegin(GL_POINTS);
    
    while (true) {
        setPixel(x1, y1);
        
        // Check if we've reached the end point
        if (x1 == x2 && y1 == y2) break;
        
        e2 = 2 * err;
        
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    
    glEnd();
}

void drawLineBresenham(float x1, float y1, float x2, float y2) {
    drawLineBresenham(static_cast<int>(x1), static_cast<int>(y1), 
                      static_cast<int>(x2), static_cast<int>(y2));
}

// ================== MIDPOINT CIRCLE DRAWING ALGORITHM ==================
void drawCircleMidpoint(int xc, int yc, int radius) {
    if (radius <= 0) {
        setPixel(xc, yc);
        return;
    }
    
    int x = 0;
    int y = radius;
    int d = 1 - radius;  // Initial decision parameter
    
    glBegin(GL_POINTS);
    
    // Plot initial points in all octants
    setPixel(xc + x, yc + y);
    setPixel(xc - x, yc + y);
    setPixel(xc + x, yc - y);
    setPixel(xc - x, yc - y);
    setPixel(xc + y, yc + x);
    setPixel(xc - y, yc + x);
    setPixel(xc + y, yc - x);
    setPixel(xc - y, yc - x);
    
    // Iterate to plot the remaining points
    while (x < y) {
        x++;
        
        // Update decision parameter
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        
        // Plot points in all octants
        setPixel(xc + x, yc + y);
        setPixel(xc - x, yc + y);
        setPixel(xc + x, yc - y);
        setPixel(xc - x, yc - y);
        setPixel(xc + y, yc + x);
        setPixel(xc - y, yc + x);
        setPixel(xc + y, yc - x);
        setPixel(xc - y, yc - x);
    }
    
    glEnd();
}

void drawCircleMidpoint(float xc, float yc, float radius) {
    drawCircleMidpoint(static_cast<int>(xc), static_cast<int>(yc), 
                       static_cast<int>(radius));
}

// ================== FILLED CIRCLE USING MIDPOINT ALGORITHM ==================
void drawFilledCircleMidpoint(int xc, int yc, int radius) {
    if (radius <= 0) {
        setPixel(xc, yc);
        return;
    }
    
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    
    glBegin(GL_LINES);
    
    // Plot initial horizontal lines
    for (int i = -x; i <= x; i++) {
        setPixel(xc + i, yc + y);
        setPixel(xc + i, yc - y);
    }
    
    for (int i = -y; i <= y; i++) {
        setPixel(xc + i, yc + x);
        setPixel(xc + i, yc - x);
    }
    
    // Iterate to plot the remaining points
    while (x < y) {
        x++;
        
        // Update decision parameter
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        
        // Draw horizontal lines for the filled circle
        for (int i = -x; i <= x; i++) {
            setPixel(xc + i, yc + y);
            setPixel(xc + i, yc - y);
        }
        
        for (int i = -y; i <= y; i++) {
            setPixel(xc + i, yc + x);
            setPixel(xc + i, yc - x);
        }
    }
    
    glEnd();
}

void drawFilledCircleMidpoint(float xc, float yc, float radius) {
    drawFilledCircleMidpoint(static_cast<int>(xc), static_cast<int>(yc), 
                             static_cast<int>(radius));
}

// ================== POLYGON DRAWING USING DDA ==================
void drawPolygonDDA(const std::vector<std::pair<float, float>>& vertices) {
    if (vertices.size() < 2) return;
    
    // Draw lines between consecutive vertices
    for (size_t i = 0; i < vertices.size(); i++) {
        size_t next = (i + 1) % vertices.size();
        drawLineDDA(vertices[i].first, vertices[i].second,
                   vertices[next].first, vertices[next].second);
    }
}

// ================== QUAD DRAWING USING DDA ==================
void drawQuadDDA(float x1, float y1, float x2, float y2, 
                 float x3, float y3, float x4, float y4) {
    // Draw the four edges of the quadrilateral
    drawLineDDA(x1, y1, x2, y2);
    drawLineDDA(x2, y2, x3, y3);
    drawLineDDA(x3, y3, x4, y4);
    drawLineDDA(x4, y4, x1, y1);
}

// ================== TRIANGLE DRAWING USING DDA ==================
void drawTriangleDDA(float x1, float y1, float x2, float y2, float x3, float y3) {
    // Draw the three edges of the triangle
    drawLineDDA(x1, y1, x2, y2);
    drawLineDDA(x2, y2, x3, y3);
    drawLineDDA(x3, y3, x1, y1);
}

// ================== NEW: FILLED QUAD USING SCANLINE ==================
void drawFilledQuadDDA(float x1, float y1, float x2, float y2, 
                       float x3, float y3, float x4, float y4) {
    // Store vertices in a vector
    std::vector<std::pair<float, float>> vertices = {
        {x1, y1}, {x2, y2}, {x3, y3}, {x4, y4}
    };
    
    // Find min and max Y
    float minY = std::min({y1, y2, y3, y4});
    float maxY = std::max({y1, y2, y3, y4});
    
    // For each scanline
    glBegin(GL_LINES);
    for (int scanY = static_cast<int>(minY); scanY <= static_cast<int>(maxY); scanY++) {
        std::vector<float> intersections;
        
        // Check each edge
        for (size_t i = 0; i < vertices.size(); i++) {
            size_t j = (i + 1) % vertices.size();
            float xa = vertices[i].first, ya = vertices[i].second;
            float xb = vertices[j].first, yb = vertices[j].second;
            
            // Check if edge crosses this scanline
            if ((ya <= scanY && yb > scanY) || (yb <= scanY && ya > scanY)) {
                // Calculate intersection
                float t = (scanY - ya) / (yb - ya);
                float x = xa + t * (xb - xa);
                intersections.push_back(x);
            }
        }
        
        // Sort intersections and draw horizontal lines between pairs
        std::sort(intersections.begin(), intersections.end());
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            // Use explicit float version to avoid ambiguity
            drawLineDDAFloat(intersections[i], static_cast<float>(scanY), 
                           intersections[i + 1], static_cast<float>(scanY));
        }
    }
    glEnd();
    
    // Draw outline - use the safe wrapper
    drawQuadDDASafe(x1, y1, x2, y2, x3, y3, x4, y4);
}

// ================== NEW: FILLED TRIANGLE USING SCANLINE ==================
void drawFilledTriangleDDA(float x1, float y1, float x2, float y2, float x3, float y3) {
    // Create arrays for vertices
    float x[3] = {x1, x2, x3};
    float y[3] = {y1, y2, y3};
    
    // Sort vertices by Y coordinate using simple bubble sort (no lambda)
    for (int i = 0; i < 2; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (y[i] > y[j]) {
                std::swap(x[i], x[j]);
                std::swap(y[i], y[j]);
            }
        }
    }
    
    // Now vertices are sorted: (x[0],y[0]) is top, (x[2],y[2]) is bottom
    
    glBegin(GL_LINES);
    
    // Top half of triangle (from vertex 0 to vertex 1)
    if (y[1] > y[0]) {
        for (int scanY = static_cast<int>(y[0]); scanY <= static_cast<int>(y[1]); scanY++) {
            float t1 = (scanY - y[0]) / (y[2] - y[0]);
            float t2 = (scanY - y[0]) / (y[1] - y[0]);
            float xleft = x[0] + t1 * (x[2] - x[0]);
            float xright = x[0] + t2 * (x[1] - x[0]);
            
            // Ensure left < right
            if (xleft > xright) std::swap(xleft, xright);
            
            // Use explicit float version
            drawLineDDAFloat(xleft, static_cast<float>(scanY), 
                           xright, static_cast<float>(scanY));
        }
    }
    
    // Bottom half of triangle (from vertex 1 to vertex 2)
    if (y[2] > y[1]) {
        for (int scanY = static_cast<int>(y[1]); scanY <= static_cast<int>(y[2]); scanY++) {
            float t1 = (scanY - y[0]) / (y[2] - y[0]);
            float t2 = (scanY - y[1]) / (y[2] - y[1]);
            float xleft = x[0] + t1 * (x[2] - x[0]);
            float xright = x[1] + t2 * (x[2] - x[1]);
            
            // Ensure left < right
            if (xleft > xright) std::swap(xleft, xright);
            
            // Use explicit float version
            drawLineDDAFloat(xleft, static_cast<float>(scanY), 
                           xright, static_cast<float>(scanY));
        }
    }
    
    glEnd();
    
    // Draw outline
    drawTriangleDDASafe(x1, y1, x2, y2, x3, y3);
}


// Update the drawFilledRectDDA function to use the new filled quad:
void drawFilledRectDDA(float x1, float y1, float x2, float y2) {
    // Save current color
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);
    
    // Draw filled rectangle using new scanline algorithm
    drawFilledQuadDDA(x1, y1, x2, y1, x2, y2, x1, y2);
    
    // Draw border using DDA (darker color)
    glColor3f(currentColor[0] * 0.5f, currentColor[1] * 0.5f, currentColor[2] * 0.5f);
    drawQuadDDA(x1, y1, x2, y1, x2, y2, x1, y2);
    
    // Restore color
    glColor3f(currentColor[0], currentColor[1], currentColor[2]);
}

// Update the drawTriangleFanDDA to use the new filled triangle:
void drawTriangleFanDDA(float centerX, float centerY, 
                       const std::vector<std::pair<float, float>>& vertices) {
    if (vertices.size() < 2) return;
    
    // Use filled triangle algorithm for each triangle
    for (size_t i = 0; i + 1 < vertices.size(); i++) {
        drawFilledTriangleDDA(centerX, centerY, 
                             vertices[i].first, vertices[i].second,
                             vertices[i + 1].first, vertices[i + 1].second);
    }
    
    // Connect back to first vertex
    if (vertices.size() > 2) {
        drawFilledTriangleDDA(centerX, centerY,
                             vertices.back().first, vertices.back().second,
                             vertices[0].first, vertices[0].second);
    }
}

// Draw a quad strip (for smoke trails, etc.)
void drawQuadStripDDA(const std::vector<std::pair<float, float>>& vertices) {
    if (vertices.size() < 2) return;
    
    // Create filled quads between consecutive pairs
    for (size_t i = 0; i + 1 < vertices.size(); i++) {
        // Calculate perpendicular offsets for width
        float dx = vertices[i+1].first - vertices[i].first;
        float dy = vertices[i+1].second - vertices[i].second;
        float length = std::sqrt(dx*dx + dy*dy);
        
        if (length > 0) {
            float perpX = -dy / length * 3.0f;  // Width of 3 pixels
            float perpY = dx / length * 3.0f;
            
            float x1 = vertices[i].first - perpX, y1 = vertices[i].second - perpY;
            float x2 = vertices[i].first + perpX, y2 = vertices[i].second + perpY;
            float x3 = vertices[i+1].first + perpX, y3 = vertices[i+1].second + perpY;
            float x4 = vertices[i+1].first - perpX, y4 = vertices[i+1].second - perpY;
            
            drawFilledQuadDDA(x1, y1, x2, y2, x3, y3, x4, y4);
        }
    }
}

// ================== Draw a filled polygon (using scanline algorithm) ==================
void drawFilledPolygonDDA(const std::vector<std::pair<float, float>>& vertices) {
    if (vertices.size() < 3) return;
    
    // Find min and max Y
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    
    for (const auto& vertex : vertices) {
        if (vertex.second < minY) minY = vertex.second;
        if (vertex.second > maxY) maxY = vertex.second;
    }
    
    // For each scanline
    glBegin(GL_LINES);
    for (int scanY = static_cast<int>(minY); scanY <= static_cast<int>(maxY); scanY++) {
        std::vector<float> intersections;
        
        // Check each edge
        for (size_t i = 0; i < vertices.size(); i++) {
            size_t j = (i + 1) % vertices.size();
            float xa = vertices[i].first, ya = vertices[i].second;
            float xb = vertices[j].first, yb = vertices[j].second;
            
            // Check if edge crosses this scanline
            if ((ya <= scanY && yb > scanY) || (yb <= scanY && ya > scanY)) {
                // Calculate intersection
                float t = (scanY - ya) / (yb - ya);
                float x = xa + t * (xb - xa);
                intersections.push_back(x);
            }
        }
        
        // Sort intersections and draw horizontal lines between pairs
        std::sort(intersections.begin(), intersections.end());
        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            // Use explicit float version
            drawLineDDAFloat(intersections[i], static_cast<float>(scanY), 
                           intersections[i + 1], static_cast<float>(scanY));
        }
    }
    glEnd();
    
    // Draw outline
    drawPolygonDDA(vertices);
}

// Draw an ellipse using Midpoint algorithm (for special effects)
void drawEllipseMidpoint(int xc, int yc, int rx, int ry) {
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;
    
    // Initial decision parameter of region 1
    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;
    
    glBegin(GL_POINTS);
    
    // For region 1
    while (dx < dy) {
        // Plot points based on 4-way symmetry
        setPixel(xc + x, yc + y);
        setPixel(xc - x, yc + y);
        setPixel(xc + x, yc - y);
        setPixel(xc - x, yc - y);
        
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        } else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }
    
    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) + 
         ((rx * rx) * ((y - 1) * (y - 1))) - 
         (rx * rx * ry * ry);
    
    // Plotting points of region 2
    while (y >= 0) {
        // Plot points based on 4-way symmetry
        setPixel(xc + x, yc + y);
        setPixel(xc - x, yc + y);
        setPixel(xc + x, yc - y);
        setPixel(xc - x, yc - y);
        
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        } else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
    
    glEnd();
}