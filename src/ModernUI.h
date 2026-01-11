#pragma once

#include <string>
#include <windows.h>

class ModernUI {
public:
  ModernUI();
  ~ModernUI();

  bool initialize(HINSTANCE hInstance);
  int run();

  void setStatus(const std::string &status);
  void setRecording(bool recording);
  void showTranscription(const std::string &text);
  bool shouldTranslate() const { return m_translateToEnglish; }

private:
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                     LPARAM lParam);
  void onPaint(HWND hwnd);
  void onClick(int x, int y);

  HWND m_hwnd;
  bool m_isRecording;
  bool m_translateToEnglish;
  std::string m_statusText;
  std::string m_transcriptionText;

  // Colors (Modern Dark Theme)
  static const COLORREF COLOR_BG = RGB(30, 30, 30);
  static const COLORREF COLOR_ACCENT = RGB(0, 120, 212);
  static const COLORREF COLOR_ACCENT_HOVER = RGB(0, 140, 240);
  static const COLORREF COLOR_RECORDING = RGB(220, 50, 50);
  static const COLORREF COLOR_TEXT = RGB(255, 255, 255);
  static const COLORREF COLOR_TEXT_DIM = RGB(180, 180, 180);
};
