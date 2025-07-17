#ifndef COUNT_H
#define COUNT_H

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif


DLL_EXPORT int rand_bot(int axis);


#endif // COUNT_H