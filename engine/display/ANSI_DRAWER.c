// ANSI color palette (standard 16 colors)
// BLACK RED GRENN YELLOW BLUE MAGENTA CYAN GRAY 
// 30    31  32    33     34   35      36   37  DARKER
// 90    91  92    93     94   95      96   97  BRIGHTER

// BACKGROUND ANSI color palette (standard 16 colors)
// BLACK RED GRENN YELLOW BLUE MAGENTA CYAN GRAY 
// 40    41  42    43     44   45      46   47  DARKER
// 100   101 102   103    104  105     106  107 BRIGHTER

// FHD [1888 x 532]  [17.8 944 x 355]  [29.6 944 x 213] 
// 2k  [2528 x 712]  
// 4k  [3800 x 1070] 

#define BUILD_DLL
#include "ANSI_DRAWER.h"
#include <stdio.h>
#include <windows.h>

unsigned int x = 0;
unsigned int y = 0;

void output_buffer() { // CPU based output

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    unsigned int bufferWidth  = csbi.dwSize.X;
    unsigned int bufferHeight = csbi.dwSize.Y;
    
    // Check if the console buffer size matches the desired dimensions if console is resized
    if (bufferWidth != x || bufferHeight != y) {
        // Resize console buffer if necessary
        system("cls"); // Clear the console
    }
    
    // Save current console size
    x = bufferWidth;
    y = bufferHeight;

    // Demonstration of ANSI escape codes for drawing
    // for (unsigned int i = 0; i < (x/2) * y; i++) {
    //     
    //     pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1b[%dm\x1b[%d;%dHXO",30 + (rand() % 8), (i + 1) % y + 1, rand() % x); // Random foreground color
    //     
    // }


    // Edge testing
    printf("\x1b[0H#\x1b[0;%dH#\x1b[%d;%dH#\x1b[%d;0H#", bufferWidth, bufferHeight, bufferWidth, bufferHeight);
    
}