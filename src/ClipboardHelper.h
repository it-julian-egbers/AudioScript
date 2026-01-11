#pragma once

#include <string>

class ClipboardHelper {
public:
  static bool copyToClipboard(const std::string &text);
  static bool pasteFromClipboard();
};
