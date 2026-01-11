#include "ClipboardHelper.h"
#include <iostream>
#include <windows.h>


bool ClipboardHelper::copyToClipboard(const std::string &text) {
  if (!OpenClipboard(nullptr)) {
    std::cerr << "Failed to open clipboard!" << std::endl;
    return false;
  }

  EmptyClipboard();

  // Allocate global memory for text
  HGLOBAL hglbCopy =
      GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(char));
  if (hglbCopy == nullptr) {
    CloseClipboard();
    std::cerr << "Failed to allocate memory!" << std::endl;
    return false;
  }

  // Lock the handle and copy the text
  char *lptstrCopy = (char *)GlobalLock(hglbCopy);
  memcpy(lptstrCopy, text.c_str(), text.size() + 1);
  GlobalUnlock(hglbCopy);

  // Place the handle on the clipboard
  SetClipboardData(CF_TEXT, hglbCopy);
  CloseClipboard();

  std::cout << "📋 Text in Zwischenablage kopiert" << std::endl;
  return true;
}

bool ClipboardHelper::pasteFromClipboard() {
  // Simulate Ctrl+V key press
  INPUT inputs[4] = {};

  // Press Ctrl
  inputs[0].type = INPUT_KEYBOARD;
  inputs[0].ki.wVk = VK_CONTROL;

  // Press V
  inputs[1].type = INPUT_KEYBOARD;
  inputs[1].ki.wVk = 'V';

  // Release V
  inputs[2].type = INPUT_KEYBOARD;
  inputs[2].ki.wVk = 'V';
  inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

  // Release Ctrl
  inputs[3].type = INPUT_KEYBOARD;
  inputs[3].ki.wVk = VK_CONTROL;
  inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

  UINT uSent = SendInput(4, inputs, sizeof(INPUT));

  if (uSent != 4) {
    std::cerr << "Failed to send input!" << std::endl;
    return false;
  }

  std::cout << "⌨️  Strg+V simuliert - Text eingefügt" << std::endl;
  return true;
}
