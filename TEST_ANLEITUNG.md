# Test-Anleitung - Verbesserte Version

## Was wurde geändert?
1. ✅ `Beep()` durch `MessageBeep()` ersetzt → Bessere Kompatibilität
2. ✅ Umfangreiche Debug-Ausgaben hinzugefügt
3. ✅ Detaillierte Fehlermeldungen bei Hotkey-Problemen

## Testen

### 1. Starte die Anwendung
```bash
cd d:\Julian-zb-prd\Pogramme\sprache-zu-text\build\bin\Release
.\SpracheZuText.exe
```

### 2. Was du sehen solltest:
```
==========================================
   Sprache-zu-Text Hotkey-Anwendung
==========================================

Initialisiere Whisper...
Loading Whisper model: ggml-base.bin
✓ Whisper model loaded successfully

Registriere globalen Hotkey F9...
✓ Hotkey F9 erfolgreich registriert!

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  🎤 BEREIT ZUM AUFNEHMEN
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

📌 Drücke F9 zum Starten/Stoppen der Aufnahme
📌 Drücke Strg+C zum Beenden

⏳ Warte auf Hotkey...
```

### 3. F9 drücken
Wenn du F9 drückst, solltest du sehen:
```
>>> F9 GEDRÜCKT - Starte Aufnahme...
🔴 Aufnahme gestartet: recording_20260111_232944.wav
>>> Audio-Feedback: Aufnahme-Start-Signal
```

**+ Windows System-Sound sollte abgespielt werden!**

### 4. Etwas sagen, dann erneut F9 drücken
```
>>> F9 GEDRÜCKT - Stoppe Aufnahme...
⏹️ Aufnahme gestoppt: recording_20260111_232944.wav
>>> Audio-Feedback: Aufnahme-Stopp-Signal

🔄 Verarbeite Aufnahme...
```

## Problemdiagnose

### Wenn NICHTS in der Konsole erscheint:
→ Das Programm läuft nicht!
→ Überprüfe ob es im Task-Manager ist

### Wenn "Hotkey F9 konnte nicht registriert werden":
```
❌ Fehler: Hotkey F9 konnte nicht registriert werden!
   Möglicherweise wird F9 bereits von einer anderen Anwendung verwendet.
   Windows Fehlercode: 1409
```
→ F9 wird von einem anderen Programm blockiert
→ Schließe andere Programme (z.B. Gaming-Software, Makro-Tools)

### Wenn F9 gedrückt wird aber KEINE Ausgabe erscheint:
→ F9 wird möglicherweise von Windows abgefangen
→ Versuche als Administrator zu starten

### Wenn kein System-Sound zu hören ist:
→ Windows-Systemsounds sind deaktiviert
→ Gehe zu: Systemsteuerung → Sound → Sounds
→ Aktiviere "Windows-Standardschema"

## Alternative: Anderen Hotkey verwenden

Falls F9 nicht funktioniert, ändere in `main.cpp` Zeile 71:

**Für F10:**
```cpp
if (!RegisterHotKey(nullptr, HOTKEY_F9, 0, VK_F10)) {
```

**Für Strg+R:**
```cpp
if (!RegisterHotKey(nullptr, HOTKEY_F9, MOD_CONTROL, 'R')) {
```

Dann neu kompilieren:
```bash
cd d:\Julian-zb-prd\Pogramme\sprache-zu-text\build
cmake --build . --config Release
```
