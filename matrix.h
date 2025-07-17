#ifndef MATRIX_H
#define MATRIX_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif


DLL_EXPORT void matrix_draw();
DLL_EXPORT void dot_position();
DLL_EXPORT void bot_navigate(int x, int y);


#endif // MATRIX_H