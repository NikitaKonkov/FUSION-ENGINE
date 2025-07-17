#ifndef BUILD_DLL
#define BUILD_DLL
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "render3d.h"

// 3D renderer constants
#define FOV 60.0f
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define CUBE_SIZE 1.0f
#define CAMERA_DISTANCE 3.0f

// Math helper functions
float deg_to_rad(float degrees) {
    return degrees * M_PI / 180.0f;
}

// Matrix operations for 3D transformations
void rotate_x(float* x, float* y, float* z, float angle) {
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    float temp_y = *y;
    float temp_z = *z;
    *y = temp_y * cos_a - temp_z * sin_a;
    *z = temp_y * sin_a + temp_z * cos_a;
}

void rotate_y(float* x, float* y, float* z, float angle) {
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    float temp_x = *x;
    float temp_z = *z;
    *x = temp_x * cos_a + temp_z * sin_a;
    *z = -temp_x * sin_a + temp_z * cos_a;
}

void rotate_z(float* x, float* y, float* z, float angle) {
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    float temp_x = *x;
    float temp_y = *y;
    *x = temp_x * cos_a - temp_y * sin_a;
    *y = temp_x * sin_a + temp_y * cos_a;
}

// Initialize 3D renderer
DLL_EXPORT int render3d_init() {
    return 1; // Success
}

// Cleanup 3D renderer
DLL_EXPORT void render3d_cleanup() {
    // Nothing to cleanup for now
}

// Transform 3D point to 2D screen coordinates
DLL_EXPORT void render3d_project(float x, float y, float z, int* screen_x, int* screen_y) {
    // Move camera back
    z += CAMERA_DISTANCE;
    
    // Perspective projection
    if (z > NEAR_PLANE) {
        float scale = 1.0f / z;
        float projected_x = x * scale;
        float projected_y = y * scale;
        
        // Convert to screen coordinates
        *screen_x = (int)(projected_x * RENDER_WIDTH / 2 + RENDER_WIDTH / 2);
        *screen_y = (int)(-projected_y * RENDER_HEIGHT / 2 + RENDER_HEIGHT / 2);
    } else {
        // Point is behind camera
        *screen_x = -1;
        *screen_y = -1;
    }
}

// Clear the framebuffer
DLL_EXPORT void render3d_clear(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], int color) {
    for (int y = 0; y < RENDER_HEIGHT; y++) {
        for (int x = 0; x < RENDER_WIDTH; x++) {
            framebuffer[y][x] = color;
        }
    }
}

// Draw a pixel in the framebuffer
DLL_EXPORT void render3d_pixel(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], int x, int y, int color) {
    if (x >= 0 && x < RENDER_WIDTH && y >= 0 && y < RENDER_HEIGHT) {
        framebuffer[y][x] = color;
    }
}

// Draw a line using Bresenham's algorithm
DLL_EXPORT void render3d_line(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], int x0, int y0, int x1, int y1, int color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        render3d_pixel(framebuffer, x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Render a rotating cube
DLL_EXPORT void render3d_cube(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], float rotation_x, float rotation_y, float rotation_z) {
    // Clear the framebuffer
    render3d_clear(framebuffer, 0);
    
    // Define cube vertices
    Vertex3D cube_vertices[8] = {
        {-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE}, // 0
        { CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE}, // 1
        { CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE}, // 2
        {-CUBE_SIZE,  CUBE_SIZE, -CUBE_SIZE}, // 3
        {-CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE}, // 4
        { CUBE_SIZE, -CUBE_SIZE,  CUBE_SIZE}, // 5
        { CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE}, // 6
        {-CUBE_SIZE,  CUBE_SIZE,  CUBE_SIZE}  // 7
    };
    
    // Rotate vertices
    for (int i = 0; i < 8; i++) {
        float x = cube_vertices[i].x;
        float y = cube_vertices[i].y;
        float z = cube_vertices[i].z;
        
        rotate_x(&x, &y, &z, rotation_x);
        rotate_y(&x, &y, &z, rotation_y);
        rotate_z(&x, &y, &z, rotation_z);
        
        cube_vertices[i].x = x;
        cube_vertices[i].y = y;
        cube_vertices[i].z = z;
    }
    
    // Project vertices to screen coordinates
    int screen_coords[8][2];
    for (int i = 0; i < 8; i++) {
        render3d_project(cube_vertices[i].x, cube_vertices[i].y, cube_vertices[i].z, 
                        &screen_coords[i][0], &screen_coords[i][1]);
    }
    
    // Define cube edges (vertex index pairs)
    int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Back face
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Front face
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Connecting edges
    };
    
    // Draw edges
    for (int i = 0; i < 12; i++) {
        int v0 = edges[i][0];
        int v1 = edges[i][1];
        
        int x0 = screen_coords[v0][0];
        int y0 = screen_coords[v0][1];
        int x1 = screen_coords[v1][0];
        int y1 = screen_coords[v1][1];
        
        // Only draw if both vertices are on screen
        if (x0 >= 0 && x0 < RENDER_WIDTH && y0 >= 0 && y0 < RENDER_HEIGHT &&
            x1 >= 0 && x1 < RENDER_WIDTH && y1 >= 0 && y1 < RENDER_HEIGHT) {
            
            // Use different colors for different edges
            int color = 1 + (i % 7); // Colors 1-7
            render3d_line(framebuffer, x0, y0, x1, y1, color);
        }
    }
}
