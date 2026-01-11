#pragma once

#include <string>
#include <vector>
#include <atomic>

class AudioRecorder {
public:
    AudioRecorder();
    ~AudioRecorder();

    bool startRecording();
    bool stopRecording();
    bool isRecording() const { return m_isRecording; }
    
    std::string getLastRecordingPath() const { return m_lastRecordingPath; }

private:
    std::atomic<bool> m_isRecording;
    std::string m_lastRecordingPath;
    void* m_device;  // ma_device pointer
    void* m_encoder; // ma_encoder pointer
    
    std::vector<float> m_recordedSamples;
    
    static void dataCallback(void* pDevice, void* pOutput, const void* pInput, unsigned int frameCount);
};
