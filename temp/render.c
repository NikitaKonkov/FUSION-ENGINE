#ifndef BUILD_DLL
#define BUILD_DLL
#endif
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdbool.h>
#include "render.h"
#include "render3d.h"
#include <conio.h>
#include <math.h>
// Global variables to track console dimensions
int width_0 = 0;
int width_1 = 0;
int height_0 = 0;
int height_1 = 0; 


// Define maximum supported dimensions using #define for compile-time constants
#define MAX_WIDTH 2528/2
#define MAX_HEIGHT 712
// FHD [1888 x 532]  [944 x 355]  [944 x 213] 
// 2k  [2528 x 712]  
// 4k  [3800 x 1070] 

// BEST RESOLUTION
// 25.59
// 316x178

// Global variable to track bit rate
#define BIT_RATE 8


// Static buffer - allocated once at compile time
static char buffer[MAX_WIDTH * MAX_HEIGHT * BIT_RATE];


// ANSI color palette (standard 16 colors)
// BLACK RED GRENN YELLOW BLUE MAGENTA CYAN GRAY 
// 30    31  32    33     34   35      36   37  DARKER
// 90    91  92    93     94   95      96   97  BRIGHTER

// BACKGROUND ANSI color palette (standard 16 colors)
// BLACK RED GRENN YELLOW BLUE MAGENTA CYAN GRAY 
// 40    41  42    43     44   45      46   47  DARKER
// 100   101 102   103    104  105     106  107 BRIGHTER


int pos = 0; // Current position in the buffer

// Dynamic 3D framebuffer allocation
static int** dynamic_framebuffer = NULL;
static int current_fb_width = 0;
static int current_fb_height = 0;

// Helper function to allocate dynamic framebuffer
int** allocate_framebuffer(int width, int height) {
    if (dynamic_framebuffer != NULL && current_fb_width == width && current_fb_height == height) {
        return dynamic_framebuffer; // Reuse existing buffer
    }
    
    // Free existing buffer if size changed
    if (dynamic_framebuffer != NULL) {
        for (int i = 0; i < current_fb_height; i++) {
            free(dynamic_framebuffer[i]);
        }
        free(dynamic_framebuffer);
    }
    
    // Allocate new buffer
    dynamic_framebuffer = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        dynamic_framebuffer[i] = (int*)malloc(width * sizeof(int));
    }
    
    current_fb_width = width;
    current_fb_height = height;
    
    return dynamic_framebuffer;
}

// New function to render 3D cube using dynamic resolution
void cmd_drawer_3D_cube(unsigned ticks) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    int bufferWidth = csbi.dwSize.X;
    int bufferHeight = csbi.dwSize.Y;

    printf("\x1B[2H%dx%d", bufferWidth/2, bufferHeight);

    // Clear screen if window size changed
    if (width_0 != bufferWidth || height_0 != bufferHeight) {
        system("cls");
    }
    
    // Check if width is uneven
    bool is_uneven = bufferWidth % 2 != 0;
    
    // Update dimensions
    width_0 = bufferWidth;
    width_1 = bufferWidth / 2 - is_uneven;
    height_0 = bufferHeight;
    height_1 = bufferHeight;

    // Clamp dimensions to maximum supported size
    if (width_1 > MAX_WIDTH) width_1 = MAX_WIDTH;
    if (height_1 > MAX_HEIGHT) height_1 = MAX_HEIGHT;

    // Calculate actual render dimensions (avoid first 2 rows and last 2 rows)
    int render_width = width_1 - is_uneven;
    int render_height = height_1 - 4; // -4 for 2 rows top and 2 rows bottom
    
    // Ensure minimum size
    if (render_width < 10) render_width = 10;
    if (render_height < 10) render_height = 10;

    // Calculate rotation based on ticks
    float rotation_x = (ticks * 0.02f);
    float rotation_y = (ticks * 0.03f);
    float rotation_z = (ticks * 0.01f);
    
    // Allocate dynamic framebuffer
    int** framebuffer = allocate_framebuffer(render_width, render_height);
    
    // Render 3D cube to dynamic framebuffer
    render3d_cube_dynamic(framebuffer, render_width, render_height, rotation_x, rotation_y, rotation_z);
    
    pos = 0; // Reset position
    
    // Convert 3D framebuffer to ANSI output (following the same pattern as cmd_drawer_ANSI)
    for (int y = 2; y < height_1 - 2; y++) {
        for (int x = 0; x < render_width; x++) {
            // Check buffer bounds
            if (pos >= sizeof(buffer) - 20) break;
            
            // Get color from 3D framebuffer
            int fb_y = y - 2;
            int fb_x = x;
            
            if (fb_y >= 0 && fb_y < render_height && fb_x >= 0 && fb_x < render_width) {
                int color_index = framebuffer[fb_y][fb_x];
                
                if (color_index > 0) {
                    // Map color index to ANSI color
                    int ansi_color = 30 + (color_index % 8);
                    pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1B[%dmXO", ansi_color);
                } else {
                    // Background
                    pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "  ");
                }
            } else {
                pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "  ");
            }
        }
        
        // Add newline if needed
        if (is_uneven && pos + 20 <= sizeof(buffer)) {
            buffer[pos++] = '\n';
        }
    }

    // Null terminate and output
    if (pos < sizeof(buffer)) buffer[pos] = '\0';

    printf("\x1B[3H%s", buffer);
}


void cmd_drawer_ANSI(unsigned ticks) {

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    int bufferWidth = csbi.dwSize.X;
    int bufferHeight = csbi.dwSize.Y;

    printf("\x1B[2H%dx%d", bufferWidth/2, bufferHeight); // Reset color at end

    

    // Clear screen if window size changed
    if (width_0 != bufferWidth || height_0 != bufferHeight) {
        system("cls");
    }
    
    // Check if width is uneven
    bool is_uneven = bufferWidth % 2 != 0;
    
    // Update dimensions
    width_0 = bufferWidth;
    width_1 = bufferWidth / 2 - is_uneven;
    height_0 = bufferHeight;
    height_1 = bufferHeight;

    // Clamp dimensions to maximum supported size
    if (width_1 > MAX_WIDTH) width_1 = MAX_WIDTH;
    if (height_1 > MAX_HEIGHT) height_1 = MAX_HEIGHT;

    pos = 0; // Reset position
    // Draw the color pattern

    for (int y = 2; y < height_1 - 2; y++) {
        for (int x = 0; x < width_1 - is_uneven; x++) {
            // Check buffer bounds
            if (pos >= sizeof(buffer) - 20) break;
            
            // Generate color based on position
            int color = 30  + ((x + y + ticks) % 8); // Cycle through 8 colors
            
            // Add character with color
            pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1B[%dmXO", color);

        }
        
        // Add newline if needed
        if (is_uneven && pos + 20 <= sizeof(buffer)) {
            buffer[pos++] = '\n';
        }
        
    }

    // Null terminate and output
    if (pos < sizeof(buffer)) buffer[pos] = '\0';

    // Sleep(16); //
    printf("\x1B[3H%s", buffer); // Reset color at end
}
