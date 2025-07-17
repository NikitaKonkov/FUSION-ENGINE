#ifndef RENDER3D_H
#define RENDER3D_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

// 3D rendering resolution
#define RENDER_WIDTH 421
#define RENDER_HEIGHT 237




// 3D vertex structure
typedef struct {
    float x, y, z;
} Vertex3D;

// 3D triangle structure
typedef struct {
    Vertex3D vertices[3];
    int color;
} Triangle3D;

// Initialize 3D renderer
DLL_EXPORT int render3d_init();

// Cleanup 3D renderer
DLL_EXPORT void render3d_cleanup();

// Render a rotating cube and return the 2D array
DLL_EXPORT void render3d_cube(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], float rotation_x, float rotation_y, float rotation_z);

// Clear the framebuffer
DLL_EXPORT void render3d_clear(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], int color);

// Draw a pixel in the framebuffer
DLL_EXPORT void render3d_pixel(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], int x, int y, int color);

// Draw a line in the framebuffer
DLL_EXPORT void render3d_line(int framebuffer[RENDER_HEIGHT][RENDER_WIDTH], int x0, int y0, int x1, int y1, int color);

// Transform 3D point to 2D screen coordinates
DLL_EXPORT void render3d_project(float x, float y, float z, int* screen_x, int* screen_y);

#endif // RENDER3D_H
