#include "WhisperSTT.h"
#include "whisper.h"

#include <fstream>
#include <iostream>
#include <vector>

WhisperSTT::WhisperSTT() : m_context(nullptr), m_initialized(false) {}

WhisperSTT::~WhisperSTT() {
  if (m_context) {
    whisper_free(m_context);
  }
}

bool WhisperSTT::initialize(const std::string &modelPath) {
  if (m_initialized) {
    std::cerr << "Already initialized!" << std::endl;
    return true;
  }

  std::cout << "Loading Whisper model: " << modelPath << std::endl;
  m_context = whisper_init_from_file(modelPath.c_str());

  if (!m_context) {
    std::cerr << "Failed to load Whisper model!" << std::endl;
    return false;
  }

  m_initialized = true;
  std::cout << "✓ Whisper model loaded successfully" << std::endl;
  return true;
}

std::vector<float> WhisperSTT::loadWavFile(const std::string &filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to open WAV file: " << filepath << std::endl;
    return {};
  }

  // Skip WAV header (44 bytes for standard PCM WAV)
  file.seekg(44);

  // Read 16-bit PCM samples and convert to float
  std::vector<float> samples;
  int16_t sample;

  while (file.read(reinterpret_cast<char *>(&sample), sizeof(sample))) {
    samples.push_back(static_cast<float>(sample) / 32768.0f);
  }

  std::cout << "Loaded " << samples.size() << " samples from " << filepath
            << std::endl;
  return samples;
}

std::string WhisperSTT::transcribe(const std::string &audioFilePath,
                                   bool translate) {
  if (!m_initialized) {
    std::cerr << "Whisper not initialized!" << std::endl;
    return "";
  }

  // Load audio file
  std::vector<float> audioData = loadWavFile(audioFilePath);
  if (audioData.empty()) {
    std::cerr << "Failed to load audio data!" << std::endl;
    return "";
  }

  // Prepare whisper parameters
  whisper_full_params params =
      whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
  params.language = "de";       // German language
  params.translate = translate; // Translate to English if true
  params.print_progress = false;
  params.print_timestamps = false;

  std::cout << "🎯 Transkribiere Audio..." << std::endl;

  // Run inference
  if (whisper_full(m_context, params, audioData.data(), audioData.size()) !=
      0) {
    std::cerr << "Failed to process audio!" << std::endl;
    return "";
  }

  // Extract transcribed text
  std::string result;
  const int n_segments = whisper_full_n_segments(m_context);

  for (int i = 0; i < n_segments; ++i) {
    const char *text = whisper_full_get_segment_text(m_context, i);
    result += text;
  }

  // Trim whitespace
  if (!result.empty()) {
    size_t start = result.find_first_not_of(" \t\n\r");
    size_t end = result.find_last_not_of(" \t\n\r");
    if (start != std::string::npos && end != std::string::npos) {
      result = result.substr(start, end - start + 1);
    }
  }

  std::cout << "✓ Transkription: \"" << result << "\"" << std::endl;
  return result;
}
