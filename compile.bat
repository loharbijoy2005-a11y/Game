@echo off
echo ========================================================
echo   Stickman Temple Run 3D - C++ Raylib Compilation
echo ========================================================
echo.
echo Make sure you have GCC/MinGW-w64 installed and 
echo Raylib in your compiler's library path.
echo.
echo Compiling main.cpp with g++ ...
g++ main.cpp -o game.exe -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================================
    echo   BUILD SUCCESSFUL! Running game.exe ...
    echo ========================================================
    game.exe
) else (
    echo.
    echo ========================================================
    echo   BUILD FAILED. Please ensure:
    echo   1. MinGW-w64 is in your system PATH (g++ is accessible).
    echo   2. Raylib is installed.
    echo      (run: pacman -S mingw-w64-x86_64-raylib in MSYS2)
    echo ========================================================
)
pause
