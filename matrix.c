#define BUILD_DLL
#include "matrix.h"
#include <stdio.h>
#include <windows.h>

#define size 32


int dot[2] = {17, 17};
int array[size][size] = {0};

void matrix_draw(){
    printf("\x1B[0H");
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            x == dot[0] && y == dot[1] ? printf(" ") : printf("#");
        }
        printf("\n");
    }
}

void dot_position(){
    if(GetAsyncKeyState(VK_UP) & 0x8000) {
        dot[0] = (dot[0] - 1 + size) % size;
    }
    if(GetAsyncKeyState(VK_DOWN) & 0x8000) {
        dot[0] = (dot[0] + 1) % size;
    }
    if(GetAsyncKeyState(VK_LEFT) & 0x8000) {
        dot[1] = (dot[1] - 1 + size) % size;
    }
    if(GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        dot[1] = (dot[1] + 1) % size;
    }

}

void bot_navigate(int x, int y) {
    dot[0] = (dot[0] + x) % size;
    dot[1] = (dot[1] + y) % size;
}