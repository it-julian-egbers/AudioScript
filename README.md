# Sprache-zu-Text - Speech-to-Text Hotkey Anwendung

Eine Windows-Anwendung in C++, die mit **F9** Audio aufnimmt, mit **Whisper.cpp** in Text umwandelt und automatisch in das aktive Fenster einfügt.

## 🎯 Features

- ✅ **Globaler Hotkey F9**: Funktioniert in jeder Anwendung
- ✅ **Toggle Recording**: F9 startet/stoppt die Aufnahme
- ✅ **Deutsche Spracherkennung**: Whisper.cpp mit deutschem Modell
- ✅ **Auto-Paste**: Text wird automatisch in aktives Fenster eingefügt
- ✅ **Audio-Feedback**: Pieptöne beim Start/Stop der Aufnahme

## 📋 Voraussetzungen

### 1. Visual Studio 2019/2022
- Mit "Desktop development with C++" Workload
- Download: https://visualstudio.microsoft.com/de/

### 2. CMake (Version 3.15+)
- Download: https://cmake.org/download/

### 3. Dependencies

#### miniaudio (Header-only, bereits inkludiert)
```bash
# Erstelle Verzeichnis
mkdir external\miniaudio
cd external\miniaudio

# Lade miniaudio.h herunter
curl -L https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h -o miniaudio.h
```

#### Whisper.cpp
```bash
# Clone Whisper.cpp Repository
cd external
git clone https://github.com/ggerganov/whisper.cpp.git
cd whisper.cpp

# Kompiliere Whisper.cpp
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

#### Whisper Modell herunterladen
```bash
# Erstelle models Verzeichnis im Hauptprojekt
mkdir models
cd models

# Lade deutsches Modell herunter (~140MB)
curl -L https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-base.bin -o ggml-base.bin

# Alternative: Bessere Genauigkeit mit größerem Modell (~460MB)
# curl -L https://huggingface.co/ggerganov/whisper.cpp/resolve/main/ggml-small.bin -o ggml-small.bin
```

## 🔨 Kompilierung

### Option 1: Build-Skript (Einfach)
```bash
build.bat
```

### Option 2: Manuell mit CMake
```bash
# Konfiguriere
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64

# Kompiliere
cmake --build . --config Release
```

### Option 3: Visual Studio
```bash
# Generiere .sln Datei
cmake .. -G "Visual Studio 17 2022" -A x64

# Öffne SpracheZuText.sln in Visual Studio
# Baue Projekt (Strg+Shift+B)
```

## 🚀 Verwendung

1. **Starte die Anwendung**:
   ```bash
   build\bin\Release\SpracheZuText.exe
   ```

2. **Aufnahme starten**: Drücke **F9**
   - Kurzer hoher Piep-Ton
   - Konsole zeigt: "🔴 Aufnahme gestartet"

3. **Sprich deinen Text**

4. **Aufnahme stoppen**: Drücke erneut **F9**
   - Kurzer tiefer Piep-Ton
   - Konsole zeigt: "⏹️ Aufnahme gestoppt"

5. **Automatische Verarbeitung**:
   - Audio wird transkribiert
   - Text wird in Zwischenablage kopiert
   - Strg+V wird automatisch simuliert
   - Text erscheint im aktiven Fenster!

## 📁 Projektstruktur

```
sprache-zu-text/
├── src/
│   ├── main.cpp              # Hauptprogramm mit F9-Hotkey
│   ├── AudioRecorder.h/cpp   # Audio-Aufnahme (miniaudio)
│   ├── WhisperSTT.h/cpp      # Spracherkennung (Whisper.cpp)
│   └── ClipboardHelper.h/cpp # Zwischenablage & Paste
├── external/
│   ├── miniaudio/
│   │   └── miniaudio.h
│   └── whisper.cpp/
│       └── build/bin/Release/whisper.lib
├── models/
│   └── ggml-base.bin         # Whisper-Modell
├── CMakeLists.txt
├── build.bat
└── README.md
```

## ⚙️ Konfiguration

### Anderes Whisper-Modell verwenden
In `main.cpp` Zeile 43:
```cpp
std::string modelPath = "ggml-small.bin";  // Für bessere Genauigkeit
```

### Andere Sprache
In `WhisperSTT.cpp` Zeile 62:
```cpp
params.language = "en";  // Englisch
params.language = "fr";  // Französisch
// etc.
```

## 🌐 Alternative: OpenAI Whisper API

Wenn du die OpenAI API statt lokales Whisper verwenden möchtest:

### Vorteile
- Keine lokale Modell-Datei nötig
- Bessere Genauigkeit
- Weniger RAM/CPU-Verbrauch

### Nachteile
- Benötigt Internetverbindung
- API-Kosten (~$0.006 pro Minute)
- Audio wird zu OpenAI gesendet

### Implementation
```cpp
// WhisperSTT.cpp ersetzen durch HTTP-Request:
#include <curl/curl.h>

std::string WhisperSTT::transcribe(const std::string& audioFilePath) {
    CURL* curl = curl_easy_init();
    
    struct curl_httppost* formpost = nullptr;
    struct curl_httppost* lastptr = nullptr;
    
    // API Key
    std::string apiKey = "sk-YOUR-API-KEY";
    
    // Form data
    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "file",
        CURLFORM_FILE, audioFilePath.c_str(),
        CURLFORM_END);
    
    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "model",
        CURLFORM_COPYCONTENTS, "whisper-1",
        CURLFORM_END);
    
    curl_formadd(&formpost, &lastptr,
        CURLFORM_COPYNAME, "language",
        CURLFORM_COPYCONTENTS, "de",
        CURLFORM_END);
    
    // Headers
    struct curl_slist* headers = nullptr;
    std::string auth = "Authorization: Bearer " + apiKey;
    headers = curl_slist_append(headers, auth.c_str());
    
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/audio/transcriptions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    
    // Response
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(curl);
    
    // Cleanup
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headers);
    
    // Parse JSON response
    // ... (JSON-Parsing mit nlohmann/json)
    
    return extractedText;
}
```

## 🐛 Troubleshooting

### "Whisper-Modell konnte nicht geladen werden"
- Stelle sicher, dass `ggml-base.bin` im gleichen Verzeichnis wie die .exe liegt
- Oder passe den Pfad in `main.cpp` an

### "Hotkey F9 konnte nicht registriert werden"
- F9 wird bereits von einer anderen Anwendung verwendet
- Ändere in `main.cpp` zu einem anderen Hotkey (z.B. F10)

### Audio-Aufnahme funktioniert nicht
- Überprüfe Windows-Mikrofon-Einstellungen
- Stelle sicher, dass ein Standard-Mikrofon gesetzt ist

### Keine Transkription / Leerer Text
- Audio zu kurz (min. 1 Sekunde sprechen)
- Mikrofon zu leise → Windows-Pegel erhöhen
- Falsches Modell → `ggml-base.bin` verwenden, nicht `ggml-base.en.bin`

## 📝 Lizenz

Dieses Projekt verwendet:
- **miniaudio**: Public Domain / MIT-0
- **whisper.cpp**: MIT License

## 🤝 Mitwirken

Verbesserungsvorschläge und Pull Requests sind willkommen!

## 📧 Support

Bei Fragen oder Problemen erstelle bitte ein Issue im Repository.
