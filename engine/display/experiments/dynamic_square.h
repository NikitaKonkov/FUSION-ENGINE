#ifndef DYNAMIC_SQUARE_H
#define DYNAMIC_SQUARE_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT void output_buffer();

#endif // ANSI_DRAWER_H