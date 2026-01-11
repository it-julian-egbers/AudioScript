#include <chrono>
#include <iostream>
#include <thread>
#include <windows.h>

#include "AudioRecorder.h"
#include "ClipboardHelper.h"
#include "ModernUI.h"
#include "WhisperSTT.h"

// Global state
AudioRecorder g_recorder;
WhisperSTT g_whisper;
ModernUI *g_ui = nullptr;
bool g_isRecording = false;

// Hotkey ID
#define HOTKEY_F9 1

void processRecording() {
  std::string audioFile = g_recorder.getLastRecordingPath();

  if (g_ui)
    g_ui->setStatus("Verarbeite Audio...");

  // Transcribe audio (with translation if enabled)
  bool shouldTranslate = g_ui ? g_ui->shouldTranslate() : false;
  std::string transcribedText =
      g_whisper.transcribe(audioFile, shouldTranslate);

  if (transcribedText.empty()) {
    if (g_ui)
      g_ui->setStatus("Fehler: Keine Transkription");
    return;
  }

  if (g_ui) {
    g_ui->showTranscription(transcribedText);
    g_ui->setStatus("✓ Fertig!");
  }

  // Copy to clipboard
  if (ClipboardHelper::copyToClipboard(transcribedText)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ClipboardHelper::pasteFromClipboard();

    if (g_ui) {
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      g_ui->setStatus("Bereit");
    }
  }
}

void toggleRecording() {
  if (!g_isRecording) {
    // Start recording
    if (g_recorder.startRecording()) {
      g_isRecording = true;
      if (g_ui) {
        g_ui->setRecording(true);
        g_ui->setStatus("● Aufnahme läuft...");
      }
      MessageBeep(MB_ICONASTERISK);
    }
  } else {
    // Stop recording
    if (g_recorder.stopRecording()) {
      g_isRecording = false;
      if (g_ui) {
        g_ui->setRecording(false);
        g_ui->setStatus("Transkribiere...");
      }
      MessageBeep(MB_ICONEXCLAMATION);

      // Process in separate thread
      std::thread processingThread(processRecording);
      processingThread.detach();
    }
  }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
  // Enable UTF-8 console output (for debugging)
  SetConsoleOutputCP(CP_UTF8);

  // Initialize Whisper
  std::string modelPath = "ggml-base.bin";

  if (!g_whisper.initialize(modelPath)) {
    MessageBoxW(nullptr,
                L"Fehler: Whisper-Modell konnte nicht geladen werden!\n\n"
                L"Bitte stelle sicher, dass 'ggml-base.bin' im gleichen "
                L"Verzeichnis liegt.",
                L"Sprache zu Text - Fehler", MB_ICONERROR | MB_OK);
    return 1;
  }

  // Create modern UI
  g_ui = new ModernUI();
  if (!g_ui->initialize(hInstance)) {
    delete g_ui;
    return 1;
  }

  // Register global hotkey (F9)
  if (!RegisterHotKey(nullptr, HOTKEY_F9, 0, VK_F9)) {
    MessageBoxW(nullptr,
                L"Warnung: Hotkey F9 konnte nicht registriert werden.\n"
                L"Möglicherweise wird F9 bereits von einer anderen Anwendung "
                L"verwendet.\n\n"
                L"Du kannst aber trotzdem den Button in der App verwenden!",
                L"Sprache zu Text - Warnung", MB_ICONWARNING | MB_OK);
  }

  // Message loop
  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_F9) {
      toggleRecording();
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // Cleanup
  UnregisterHotKey(nullptr, HOTKEY_F9);
  delete g_ui;

  return (int)msg.wParam;
}
