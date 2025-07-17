#define BUILD_DLL
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <stdbool.h>
#include "render.h"
#include <conio.h>
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
            if(y <= 1 && x <= 3) { 
                pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1B[0m  ", color);
            }else {
                pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1B[%dmXO", color);
            }
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
