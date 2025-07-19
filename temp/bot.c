#define BUILD_DLL
#include <stdlib.h>
int rand_bot(int axis) {
    return axis%2 == 0 ? (rand() % 3) - 1: (rand() % 3) - 1;
}