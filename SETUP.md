# Setup-Anleitung: Schritt für Schritt

Diese Anleitung hilft dir, das Projekt von Grund auf einzurichten.

## 📥 1. Projekt-Verzeichnis vorbereiten

```bash
cd d:\Julian-zb-prd\Pogramme\sprache-zu-text
```

## 📦 2. Dependencies installieren

### A) miniaudio herunterladen

```powershell
# Erstelle Verzeichnis
New-Item -ItemType Directory -Force -Path external\miniaudio

# Lade miniaudio.h herunter
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h" -OutFile "external\miniaudio\miniaudio.h"
```

### B) Whisper.cpp klonen und kompilieren

```powershell
# Navigiere zu external
cd external

# Klone Repository
git clone https://github.com/ggerganov/whisper.cpp.git
cd whisper.cpp

# Kompiliere mit CMake
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Zurück zum Hauptverzeichnis
cd ..\..\..
```

**Wichtig**: Nach dem Build sollte diese Datei existieren:
```
external\whisper.cpp\build\bin\Release\whisper.lib
```

### C) Whisper-Modell herunterladen

```powershell
# Erstelle models Verzeichnis
New-Item -ItemType Directory -Force -Path models

# Lade Modell herunter (~140MB, kann einige Minuten dauern)
Invoke-WebRequest -Uri "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.bin" -OutFile "models\ggml-base.bin"
```

## 🔨 3. Projekt kompilieren

### Option A: Mit build.bat (empfohlen)

```bash
build.bat
```

### Option B: Manuell

```powershell
# Erstelle build Verzeichnis
mkdir build
cd build

# Konfiguriere mit CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# Kompiliere
cmake --build . --config Release

# Zurück zum Hauptverzeichnis
cd ..
```

## 🚀 4. Ausführen

### Modell ins richtige Verzeichnis kopieren

```powershell
# Kopiere Modell neben die .exe
Copy-Item "models\ggml-base.bin" -Destination "build\bin\Release\ggml-base.bin"
```

### Anwendung starten

```powershell
.\build\bin\Release\SpracheZuText.exe
```

## ✅ 5. Testen

1. Starte die Anwendung
2. Öffne einen Text-Editor (z.B. Notepad)
3. Drücke **F9** → Hoher Piep-Ton
4. Sage etwas (z.B. "Dies ist ein Test")
5. Drücke erneut **F9** → Tiefer Piep-Ton
6. Warte 2-3 Sekunden
7. Der Text sollte automatisch im Editor erscheinen!

## 🔧 Problemlösung

### Problem: "whisper.lib not found"

**Lösung**: Whisper.cpp wurde nicht korrekt kompiliert.

```powershell
cd external\whisper.cpp\build
cmake --build . --config Release
```

Prüfe ob die Datei existiert:
```powershell
Test-Path "external\whisper.cpp\build\bin\Release\whisper.lib"
```

### Problem: "miniaudio.h not found"

**Lösung**: miniaudio.h fehlt.

```powershell
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h" -OutFile "external\miniaudio\miniaudio.h"
```

### Problem: "ggml-base.bin konnte nicht geladen werden"

**Lösung**: Modell ist nicht neben der .exe.

```powershell
# Kopiere Modell
Copy-Item "models\ggml-base.bin" -Destination "build\bin\Release\ggml-base.bin"

# ODER: Passe Pfad in main.cpp an
std::string modelPath = "../../models/ggml-base.bin";
```

## 📂 Erwartete Verzeichnisstruktur nach Setup

```
sprache-zu-text/
├── external/
│   ├── miniaudio/
│   │   └── miniaudio.h                 ✓
│   └── whisper.cpp/
│       ├── build/bin/Release/
│       │   └── whisper.lib             ✓
│       └── whisper.h                   ✓
├── models/
│   └── ggml-base.bin                   ✓
├── src/
│   ├── main.cpp                        ✓
│   ├── AudioRecorder.h/cpp             ✓
│   ├── WhisperSTT.h/cpp                ✓
│   └── ClipboardHelper.h/cpp           ✓
├── build/
│   └── bin/Release/
│       ├── SpracheZuText.exe           ✓
│       └── ggml-base.bin               ✓ (kopiert)
├── CMakeLists.txt                      ✓
├── build.bat                           ✓
└── README.md                           ✓
```

## 🎉 Fertig!

Du bist bereit! Drücke F9 und beginne zu sprechen.
