@echo off
echo ==========================================
echo  Sprache-zu-Text Build-Skript
echo ==========================================
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Run CMake
echo [1/3] Konfiguriere CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo Fehler: CMake-Konfiguration fehlgeschlagen!
    pause
    exit /b 1
)

echo.
echo [2/3] Kompiliere Projekt...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Fehler: Kompilierung fehlgeschlagen!
    pause
    exit /b 1
)

echo.
echo [3/3] Build abgeschlossen!
echo.
echo Executable: build\bin\Release\SpracheZuText.exe
echo.
pause
