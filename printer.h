#ifndef PRINTER_H
#define PRINTER_H

#include <stdio.h>

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif


DLL_EXPORT void print_t(const char* message);
DLL_EXPORT void print_e(const char* error);



#endif // PRINTER_H