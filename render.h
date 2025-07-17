#ifndef RENDER_H
#define RENDER_H


#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

DLL_EXPORT void cmd_drawer_ANSI(unsigned int ticks);

#endif // RENDER_H
