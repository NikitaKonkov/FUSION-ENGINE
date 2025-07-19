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
#include <stdlib.h> // For malloc and free
#include <windows.h>
#include <time.h> // For time() to seed random
#include <process.h> // For threading

#define WIDTH 2528
#define HEIGHT 712

unsigned int x = 0;
unsigned int y = 0;

int coordinates[2] = {20,20};
int prev_coordinates[2] = {20,20}; // Track previous position
int square_size = 64; // Default square size
int prev_square_size = 64; // Track previous size for erasing

static char buffer[WIDTH * HEIGHT * 16]; // Static buffer for output8
static char voider[WIDTH * HEIGHT * 16]; // Static buffer for erasing previous square
void output_buffer() { // CPU based output

    // Seed random number generator with current time in milliseconds
    srand((unsigned int)GetTickCount());
    
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

    int pos = 0; // Reset position
    int void_pos = 0; // Position counter for voider buffer

    // Check if any movement key is pressed to determine if we need to erase
    int key_pressed = GetAsyncKeyState(VK_UP) || GetAsyncKeyState(VK_DOWN) || 
                     GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_RIGHT);
    
    // Check for size change keys
    int size_changed = 0;
    if (GetAsyncKeyState(VK_OEM_PLUS) || GetAsyncKeyState(VK_ADD)) { // + key
        if (square_size < 100) { // Maximum size limit
            square_size += 2; // Increase by 2 to keep even numbers
            size_changed = 1;
        }
    }
    if (GetAsyncKeyState(VK_OEM_MINUS) || GetAsyncKeyState(VK_SUBTRACT)) { // - key
        if (square_size > 10) { // Minimum size limit
            square_size -= 2; // Decrease by 2 to keep even numbers
            size_changed = 1;
        }
    }

    // If movement or size change detected, create the "negative" (erasing buffer) for previous position
    if (key_pressed || size_changed) {
        system("cls"); // Clear the console to avoid ghosting artifacts
        // Build erasing buffer for previous square position using previous size
        for (int a = 0; a < prev_square_size; a++) {
            void_pos += snprintf(&voider[void_pos], sizeof(voider) - void_pos, "\x1b[%d;%dH ", prev_coordinates[1] + a, prev_coordinates[0]); // Erase left side
            void_pos += snprintf(&voider[void_pos], sizeof(voider) - void_pos, "\x1b[%d;%dH ", prev_coordinates[1] + a, prev_coordinates[0] + (prev_square_size * 2)); // Erase right side
        }
        for (int b = 0; b < (prev_square_size * 2); b++) {
            void_pos += snprintf(&voider[void_pos], sizeof(voider) - void_pos, "\x1b[%d;%dH ", prev_coordinates[1], prev_coordinates[0] + b); // Erase top side
            void_pos += snprintf(&voider[void_pos], sizeof(voider) - void_pos, "\x1b[%d;%dH ", prev_coordinates[1] + prev_square_size, prev_coordinates[0] + b); // Erase bottom side
        }
        
        // Print the erasing buffer first to remove ghosting
        printf("%s", voider);
        
        // Update previous coordinates and size to current values
        prev_coordinates[0] = coordinates[0];
        prev_coordinates[1] = coordinates[1];
        prev_square_size = square_size;
    }

    // Demonstration of ANSI escape codes for drawing
    // for (unsigned int i = 0; i < (x/2) * y; i++) {
    //     
    //     pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1b[%dm\x1b[%d;%dHXO",30 + (rand() % 8), (i + 1) % y + 1, rand() % x); // Random foreground color
    //     
    // }

    // Handle input with bounds checking (updated for dynamic square size)
    if (GetAsyncKeyState(VK_UP) && coordinates[1] > 1) {
        coordinates[1] -= 1; // Move up
    }
    if (GetAsyncKeyState(VK_DOWN) && coordinates[1] < (int)bufferHeight - square_size - 1) {
        coordinates[1] += 1; // Move down
    }
    if (GetAsyncKeyState(VK_LEFT) && coordinates[0] > 1) {
        coordinates[0] -= 1; // Move left
    }
    if (GetAsyncKeyState(VK_RIGHT) && coordinates[0] < (int)bufferWidth - (square_size * 2) - 1) {
        coordinates[0] += 1; // Move right
    }

    // Draw vertical lines of the square (left and right sides) - dynamic size
    for (int a = 0; a < square_size; a++) {
        pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1b[%dm\x1b[%d;%dHX", 90 + (a % 8), coordinates[1] + a, coordinates[0]); // Left side
        pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1b[%dm\x1b[%d;%dHX", 90 + (a % 8), coordinates[1] + a, coordinates[0] + (square_size * 2)); // Right side
    }
    // Draw horizontal lines of the square (top and bottom) - dynamic size
    for (int b = 0; b < (square_size * 2); b++) {
        pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1b[%dm\x1b[%d;%dHX", 30 + (b % 8), coordinates[1], coordinates[0] + b); // Top side
        pos += snprintf(&buffer[pos], sizeof(buffer) - pos, "\x1b[%dm\x1b[%d;%dHX", 30 + (b % 8), coordinates[1] + square_size, coordinates[0] + b); // Bottom side
    }

    printf("%s", buffer); // Output the buffer
    // system("cls"); // Clear the console for the next frame

    // Edge testing
    printf("\x1b[0H#\x1b[0;%dH#\x1b[%d;%dH#\x1b[%d;0H#", bufferWidth, bufferHeight, bufferWidth, bufferHeight);
    
}