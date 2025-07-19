#ifndef RENDER3D_H
#define RENDER3D_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

// Maximum 3D rendering resolution
#define MAX_RENDER_WIDTH 2528/2
#define MAX_RENDER_HEIGHT 712

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

// Render a rotating cube with dynamic resolution
DLL_EXPORT void render3d_cube_dynamic(int** framebuffer, int width, int height, float rotation_x, float rotation_y, float rotation_z);

// Clear the framebuffer with dynamic resolution
DLL_EXPORT void render3d_clear_dynamic(int** framebuffer, int width, int height, int color);

// Draw a pixel in the framebuffer with dynamic resolution
DLL_EXPORT void render3d_pixel_dynamic(int** framebuffer, int width, int height, int x, int y, int color);

// Draw a line in the framebuffer with dynamic resolution
DLL_EXPORT void render3d_line_dynamic(int** framebuffer, int width, int height, int x0, int y0, int x1, int y1, int color);

// Transform 3D point to 2D screen coordinates with dynamic resolution
DLL_EXPORT void render3d_project_dynamic(float x, float y, float z, int render_width, int render_height, int* screen_x, int* screen_y);

#endif // RENDER3D_H
