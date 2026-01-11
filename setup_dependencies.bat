@echo off
echo ==========================================
echo  Dependencies Setup Script
echo ==========================================
echo.

REM Create external directory
echo [1/4] Creating directories...
if not exist external mkdir external
if not exist external\miniaudio mkdir external\miniaudio
if not exist models mkdir models

REM Download miniaudio
echo.
echo [2/4] Downloading miniaudio.h...
powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h' -OutFile 'external\miniaudio\miniaudio.h'"
if %ERRORLEVEL% NEQ 0 (
    echo Fehler beim Download von miniaudio!
    pause
    exit /b 1
)
echo ✓ miniaudio.h heruntergeladen

REM Clone whisper.cpp
echo.
echo [3/4] Klone whisper.cpp Repository...
cd external
if exist whisper.cpp (
    echo whisper.cpp existiert bereits, überspringe...
) else (
    git clone https://github.com/ggerganov/whisper.cpp.git
    if %ERRORLEVEL% NEQ 0 (
        echo Fehler: Git clone fehlgeschlagen!
        echo Stelle sicher, dass Git installiert ist.
        cd ..
        pause
        exit /b 1
    )
)
cd ..
echo ✓ whisper.cpp Repository bereit

REM Build whisper.cpp
echo.
echo [4/4] Kompiliere whisper.cpp...
cd external\whisper.cpp
if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo Fehler: CMake-Konfiguration fehlgeschlagen!
    cd ..\..\..
    pause
    exit /b 1
)

cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Fehler: Kompilierung fehlgeschlagen!
    cd ..\..\..
    pause
    exit /b 1
)
cd ..\..\..
echo ✓ whisper.cpp kompiliert

echo.
echo ==========================================
echo  Dependencies erfolgreich installiert!
echo ==========================================
echo.
echo Nächster Schritt: Lade das Whisper-Modell herunter mit:
echo   download_model.bat
echo.
pause
