@echo off
echo ==========================================
echo  Whisper Model Download
echo ==========================================
echo.

if not exist models mkdir models

echo Lade Whisper Base Model herunter (~140MB)...
echo Dies kann einige Minuten dauern...
echo.

powershell -Command "Invoke-WebRequest -Uri 'https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.bin' -OutFile 'models\ggml-base.bin'"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Fehler beim Download!
    echo.
    echo Bitte lade das Modell manuell herunter:
    echo https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.bin
    echo.
    echo Speichere es als: models\ggml-base.bin
    pause
    exit /b 1
)

echo.
echo ✓ Modell erfolgreich heruntergeladen!
echo.
echo Gespeichert in: models\ggml-base.bin
echo Größe: ~140MB
echo.
echo Nächster Schritt: Kompiliere das Projekt mit:
echo   build.bat
echo.
pause
