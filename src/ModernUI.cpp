#include "ModernUI.h"
#include <dwmapi.h>
#include <string>
#include <windowsx.h>

#pragma comment(lib, "dwmapi.lib")

// Window dimensions
static const int WINDOW_WIDTH = 500;
static const int WINDOW_HEIGHT = 400;
static const int BUTTON_SIZE = 120;

ModernUI::ModernUI()
    : m_hwnd(nullptr), m_isRecording(false), m_translateToEnglish(false),
      m_statusText("Bereit"), m_transcriptionText("") {}

ModernUI::~ModernUI() {
  if (m_hwnd) {
    DestroyWindow(m_hwnd);
  }
}

bool ModernUI::initialize(HINSTANCE hInstance) {
  // Register window class
  const wchar_t CLASS_NAME[] = L"SpracheZuTextWindow";

  WNDCLASSW wc = {};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  wc.hCursor = LoadCursor(nullptr, IDC_HAND);
  wc.hbrBackground = CreateSolidBrush(COLOR_BG);

  RegisterClassW(&wc);

  // Create window
  m_hwnd =
      CreateWindowExW(0, CLASS_NAME, L"Sprache zu Text • F9",
                      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                      CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
                      nullptr, nullptr, hInstance, this);

  if (!m_hwnd) {
    return false;
  }

  // Enable dark mode title bar (Windows 10+)
  BOOL value = TRUE;
  DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value,
                        sizeof(value));

  // Center window on screen
  RECT rc;
  GetWindowRect(m_hwnd, &rc);
  int screenW = GetSystemMetrics(SM_CXSCREEN);
  int screenH = GetSystemMetrics(SM_CYSCREEN);
  int x = (screenW - (rc.right - rc.left)) / 2;
  int y = (screenH - (rc.bottom - rc.top)) / 2;
  SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

  ShowWindow(m_hwnd, SW_SHOW);
  UpdateWindow(m_hwnd);

  return true;
}

int ModernUI::run() {
  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (int)msg.wParam;
}

void ModernUI::setStatus(const std::string &status) {
  m_statusText = status;
  if (m_hwnd) {
    InvalidateRect(m_hwnd, nullptr, FALSE);
  }
}

void ModernUI::setRecording(bool recording) {
  m_isRecording = recording;
  if (m_hwnd) {
    InvalidateRect(m_hwnd, nullptr, FALSE);
  }
}

void ModernUI::showTranscription(const std::string &text) {
  m_transcriptionText = text;
  if (m_hwnd) {
    InvalidateRect(m_hwnd, nullptr, FALSE);
  }
}

LRESULT CALLBACK ModernUI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam) {
  ModernUI *pThis = nullptr;

  if (uMsg == WM_NCCREATE) {
    CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
    pThis = (ModernUI *)pCreate->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
  } else {
    pThis = (ModernUI *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  }

  if (pThis) {
    switch (uMsg) {
    case WM_PAINT:
      pThis->onPaint(hwnd);
      return 0;

    case WM_LBUTTONDOWN: {
      int x = GET_X_LPARAM(lParam);
      int y = GET_Y_LPARAM(lParam);
      pThis->onClick(x, y);
      return 0;
    }

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    }
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ModernUI::onPaint(HWND hwnd) {
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);

  RECT rc;
  GetClientRect(hwnd, &rc);

  // Enable anti-aliasing
  SetBkMode(hdc, TRANSPARENT);

  // Draw background
  HBRUSH bgBrush = CreateSolidBrush(COLOR_BG);
  FillRect(hdc, &rc, bgBrush);
  DeleteObject(bgBrush);

  // Draw title
  HFONT titleFont =
      CreateFontW(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  SelectObject(hdc, titleFont);
  SetTextColor(hdc, COLOR_TEXT);

  RECT titleRect = {20, 30, rc.right - 20, 80};
  DrawTextW(hdc, L"Sprache zu Text", -1, &titleRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);
  DeleteObject(titleFont);

  // Draw status
  HFONT statusFont =
      CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  SelectObject(hdc, statusFont);
  SetTextColor(hdc, COLOR_TEXT_DIM);

  std::wstring statusWide(m_statusText.begin(), m_statusText.end());
  RECT statusRect = {20, 70, rc.right - 20, 100};
  DrawTextW(hdc, statusWide.c_str(), -1, &statusRect,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE);
  DeleteObject(statusFont);

  // Draw translation toggle
  HFONT toggleFont =
      CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  SelectObject(hdc, toggleFont);

  // Draw checkbox
  RECT checkboxRect = {20, 95, 35, 110};
  HBRUSH checkBrush =
      CreateSolidBrush(m_translateToEnglish ? COLOR_ACCENT : RGB(60, 60, 60));
  FillRect(hdc, &checkboxRect, checkBrush);
  DeleteObject(checkBrush);

  // Draw checkmark if enabled
  if (m_translateToEnglish) {
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"✓", -1, &checkboxRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  }

  // Draw label
  SetTextColor(hdc, COLOR_TEXT_DIM);
  RECT toggleLabelRect = {40, 95, rc.right - 20, 110};
  DrawTextW(hdc, L"Deutsch  Englisch �bersetzen", -1, &toggleLabelRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
  DeleteObject(toggleFont);
  // Draw record button (centered)
  int centerX = rc.right / 2;
  int centerY = rc.bottom / 2 - 20;

  COLORREF buttonColor = m_isRecording ? COLOR_RECORDING : COLOR_ACCENT;

  // Draw button circle with gradient effect
  HBRUSH buttonBrush = CreateSolidBrush(buttonColor);
  HPEN buttonPen = CreatePen(PS_NULL, 0, buttonColor);
  SelectObject(hdc, buttonBrush);
  SelectObject(hdc, buttonPen);

  Ellipse(hdc, centerX - BUTTON_SIZE / 2, centerY - BUTTON_SIZE / 2,
          centerX + BUTTON_SIZE / 2, centerY + BUTTON_SIZE / 2);

  DeleteObject(buttonBrush);
  DeleteObject(buttonPen);

  // Draw button icon/text
  HFONT buttonFont =
      CreateFontW(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  SelectObject(hdc, buttonFont);
  SetTextColor(hdc, RGB(255, 255, 255));

  const wchar_t *icon = m_isRecording ? L"■" : L"●";
  RECT iconRect = {centerX - BUTTON_SIZE / 2, centerY - BUTTON_SIZE / 2,
                   centerX + BUTTON_SIZE / 2, centerY + BUTTON_SIZE / 2};
  DrawTextW(hdc, icon, -1, &iconRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  DeleteObject(buttonFont);

  // Draw button label
  HFONT labelFont =
      CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  SelectObject(hdc, labelFont);
  SetTextColor(hdc, COLOR_TEXT_DIM);

  const wchar_t *label =
      m_isRecording ? L"Aufnahme stoppen" : L"Aufnahme starten";
  RECT labelRect = {20, centerY + BUTTON_SIZE / 2 + 20, rc.right - 20,
                    centerY + BUTTON_SIZE / 2 + 50};
  DrawTextW(hdc, label, -1, &labelRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  DeleteObject(labelFont);

  // Draw transcription if available
  if (!m_transcriptionText.empty()) {
    HFONT transFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SelectObject(hdc, transFont);
    SetTextColor(hdc, COLOR_TEXT);

    std::wstring transWide(m_transcriptionText.begin(),
                           m_transcriptionText.end());
    RECT transRect = {20, rc.bottom - 80, rc.right - 20, rc.bottom - 20};
    DrawTextW(hdc, transWide.c_str(), -1, &transRect, DT_CENTER | DT_WORDBREAK);
    DeleteObject(transFont);
  }

  // Draw hotkey hint
  HFONT hintFont =
      CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
  SelectObject(hdc, hintFont);
  SetTextColor(hdc, COLOR_TEXT_DIM);

  RECT hintRect = {20, rc.bottom - 30, rc.right - 20, rc.bottom - 10};
  DrawTextW(hdc, L"Tipp: Dr\u00FCcke F9 von \u00FCberall", -1, &hintRect,
            DT_CENTER | DT_SINGLELINE);
  DeleteObject(hintFont);

  EndPaint(hwnd, &ps);
}

void ModernUI::onClick(int x, int y) {
  RECT rc;
  GetClientRect(m_hwnd, &rc);

  // Check if click is on checkbox
  RECT checkboxRect = {20, 95, 40, 110};
  if (x >= checkboxRect.left && x <= checkboxRect.right + 250 &&
      y >= checkboxRect.top && y <= checkboxRect.bottom) {
    m_translateToEnglish = !m_translateToEnglish;
    InvalidateRect(m_hwnd, nullptr, FALSE);
    return;
  }

  int centerX = rc.right / 2;
  int centerY = rc.bottom / 2 - 20;

  // Check if click is inside button circle
  int dx = x - centerX;
  int dy = y - centerY;
  int distSq = dx * dx + dy * dy;
  int radiusSq = (BUTTON_SIZE / 2) * (BUTTON_SIZE / 2);

  if (distSq <= radiusSq) {
    // Button clicked - send F9 message
    PostMessage(m_hwnd, WM_HOTKEY, 1, 0);
  }
}

