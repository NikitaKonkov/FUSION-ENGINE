#define BUILD_DLL
#include "count.h"


int ticks = 0;

int count() {
    ++ticks;
    return ticks;
}