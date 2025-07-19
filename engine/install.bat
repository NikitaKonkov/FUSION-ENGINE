@echo off
echo Building DLL and executable...




gcc -shared -o bin\ANSI_DRAWER.dll display\ANSI_DRAWER.c
gcc main.c -o main.exe
