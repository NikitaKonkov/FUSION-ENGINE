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

// 3D framebuffer for rendering
static int framebuffer[RENDER_HEIGHT][RENDER_WIDTH];

// New function to render 3D cube using cmd_drawer_ANSI
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

    // Calculate rotation based on ticks
    float rotation_x = (ticks * 0.02f);
    float rotation_y = (ticks * 0.03f);
    float rotation_z = (ticks * 0.01f);
    
    // Render 3D cube to framebuffer
    render3d_cube(framebuffer, rotation_x, rotation_y, rotation_z);
    
    pos = 0; // Reset position
    
    // Convert 3D framebuffer to ANSI output
    int render_width = RENDER_WIDTH;
    int render_height = RENDER_HEIGHT;
    
    // Scale down if necessary to fit console
    if (render_width > width_1) render_width = width_1;
    if (render_height > height_1 - 4) render_height = height_1 - 4;
    
    for (int y = 2; y < render_height + 2; y++) {
        for (int x = 0; x < render_width; x++) {
            // Check buffer bounds
            if (pos >= sizeof(buffer) - 20) break;
            
            // Get color from 3D framebuffer
            int fb_y = (y - 2) * RENDER_HEIGHT / render_height;
            int fb_x = x * RENDER_WIDTH / render_width;
            
            if (fb_y >= 0 && fb_y < RENDER_HEIGHT && fb_x >= 0 && fb_x < RENDER_WIDTH) {
                int color_index = framebuffer[fb_y][fb_x];
                
                if (color_index > 0) {
                    // Map color index to ANSI color
                    int ansi_color = 30 + (color_index % 8);
                    pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1B[%dm##", ansi_color);
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

    printf("\x1B[4H%s", buffer);
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
