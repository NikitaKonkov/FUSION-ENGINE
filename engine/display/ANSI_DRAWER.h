#ifndef ANSI_DRAWER_H
#define ANSI_DRAWER_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT void output_buffer();

#endif // ANSI_DRAWER_H