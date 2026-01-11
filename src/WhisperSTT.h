#pragma once

#include <memory>
#include <string>
#include <vector>

struct whisper_context;

class WhisperSTT {
public:
  WhisperSTT();
  ~WhisperSTT();

  bool initialize(const std::string &modelPath);
  std::string transcribe(const std::string &audioFilePath,
                         bool translate = false);
  bool isInitialized() const { return m_initialized; }

private:
  whisper_context *m_context;
  bool m_initialized;

  std::vector<float> loadWavFile(const std::string &filepath);
};
