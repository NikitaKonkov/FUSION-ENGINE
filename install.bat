@echo off
echo Building DLL and executable...

rem gcc -shared -o binary\math.dll math.c -DBUILD_DLL
rem gcc -shared -o binary\printer.dll printer.c -DBUILD_DLL
rem gcc -shared -o binary\count.dll count.c -DBUILD_DLL
rem gcc -shared -o binary\matrix.dll matrix.c -DBUILD_DLL
rem gcc -shared -o binary\bot.dll bot.c -DBUILD_DLL
rem gcc -shared -o binary\sound.dll sound.c -lwinmm -lpthread -DBUILD_DLL
gcc -shared -o binary\render.dll render.c

gcc -o binary\example.exe example.c

if exist binary\example.exe (
    echo Build successful!
) else (
    echo Build failed!
)
