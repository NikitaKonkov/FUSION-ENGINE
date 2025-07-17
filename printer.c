#define BUILD_DLL
#include "printer.h"

void print_e(const char* error) {
    printf("Error: %s\n", error);
}

void print_t(const char* message) {
    printf("Message: %s\n", message);
}