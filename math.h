#ifndef MATH_H
#define MATH_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT int add(int a, int b);
DLL_EXPORT int sub(int a, int b);
DLL_EXPORT int mul(int a, int b);

#endif
