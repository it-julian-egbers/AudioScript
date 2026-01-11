#include <chrono>
#include <iostream>
#include <thread>
#include <windows.h>


#include "AudioRecorder.h"
#include "ClipboardHelper.h"
#include "WhisperSTT.h"


// Global state
AudioRecorder g_recorder;
WhisperSTT g_whisper;
bool g_isRecording = false;

// Hotkey ID
#define HOTKEY_F9 1

void processRecording() {
  std::string audioFile = g_recorder.getLastRecordingPath();

  std::cout << "\n🔄 Verarbeite Aufnahme..." << std::endl;

  // Transcribe audio
  std::string transcribedText = g_whisper.transcribe(audioFile);

  if (transcribedText.empty()) {
    std::cerr << "❌ Keine Transkription möglich!" << std::endl;
    return;
  }

  // Copy to clipboard
  if (ClipboardHelper::copyToClipboard(transcribedText)) {
    // Wait a moment for clipboard to update
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Paste into active window
    ClipboardHelper::pasteFromClipboard();

    std::cout << "\n✅ Fertig! Text wurde eingefügt.\n" << std::endl;
  } else {
    std::cerr << "❌ Fehler beim Kopieren in Zwischenablage!" << std::endl;
  }
}

int main() {
  // Enable UTF-8 output in console for German umlauts and Unicode
  SetConsoleOutputCP(CP_UTF8);

  std::cout << "==========================================" << std::endl;
  std::cout << "   Sprache-zu-Text Hotkey-Anwendung" << std::endl;
  std::cout << "==========================================" << std::endl
            << std::endl;

  // Initialize Whisper
  std::string modelPath = "ggml-base.bin";
  std::cout << "Initialisiere Whisper..." << std::endl;

  if (!g_whisper.initialize(modelPath)) {
    std::cerr << "\n❌ Fehler: Whisper-Modell konnte nicht geladen werden!"
              << std::endl;
    std::cerr << "Bitte stelle sicher, dass '" << modelPath
              << "' im gleichen Verzeichnis liegt.\n"
              << std::endl;
    std::cerr << "Download: "
                 "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/"
                 "ggml-base.bin\n"
              << std::endl;
    system("pause");
    return 1;
  }

  // Register global hotkey (F9)
  std::cout << "\nRegistriere globalen Hotkey F9..." << std::endl;
  if (!RegisterHotKey(nullptr, HOTKEY_F9, 0, VK_F9)) {
    std::cerr << "❌ Fehler: Hotkey F9 konnte nicht registriert werden!"
              << std::endl;
    std::cerr << "   Möglicherweise wird F9 bereits von einer anderen "
                 "Anwendung verwendet."
              << std::endl;
    std::cerr << "   Windows Fehlercode: " << GetLastError() << std::endl;
    system("pause");
    return 1;
  }

  std::cout << "✓ Hotkey F9 erfolgreich registriert!" << std::endl;
  std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
  std::cout << "  🎤 BEREIT ZUM AUFNEHMEN" << std::endl;
  std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
  std::cout << "\n📌 Drücke F9 zum Starten/Stoppen der Aufnahme" << std::endl;
  std::cout << "📌 Drücke Strg+C zum Beenden\n" << std::endl;
  std::cout << "⏳ Warte auf Hotkey...\n" << std::endl;

  // Message loop
  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (msg.message == WM_HOTKEY) {
      if (msg.wParam == HOTKEY_F9) {
        if (!g_isRecording) {
          // Start recording
          std::cout << "\n>>> F9 GEDRÜCKT - Starte Aufnahme..." << std::endl;
          if (g_recorder.startRecording()) {
            g_isRecording = true;
            MessageBeep(MB_ICONASTERISK); // System sound for start
            std::cout << ">>> Audio-Feedback: Aufnahme-Start-Signal"
                      << std::endl;
          }
        } else {
          // Stop recording
          std::cout << "\n>>> F9 GEDRÜCKT - Stoppe Aufnahme..." << std::endl;
          if (g_recorder.stopRecording()) {
            g_isRecording = false;
            MessageBeep(MB_ICONEXCLAMATION); // System sound for stop
            std::cout << ">>> Audio-Feedback: Aufnahme-Stopp-Signal"
                      << std::endl;

            // Process in separate thread to not block message loop
            std::thread processingThread(processRecording);
            processingThread.detach();
          }
        }
      }
    }
  }

  // Cleanup
  UnregisterHotKey(nullptr, HOTKEY_F9);

  std::cout << "\nAnwendung beendet." << std::endl;
  return 0;
}
