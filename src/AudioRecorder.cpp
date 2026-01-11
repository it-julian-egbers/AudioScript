#include "AudioRecorder.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>

AudioRecorder::AudioRecorder() 
    : m_isRecording(false)
    , m_device(nullptr)
    , m_encoder(nullptr)
{
}

AudioRecorder::~AudioRecorder() {
    if (m_isRecording) {
        stopRecording();
    }
}

bool AudioRecorder::startRecording() {
    if (m_isRecording) {
        std::cerr << "Already recording!" << std::endl;
        return false;
    }

    // Generate filename with timestamp
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << "recording_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".wav";
    m_lastRecordingPath = oss.str();

    // Configure encoder for WAV file
    ma_encoder_config encoderConfig = ma_encoder_config_init(
        ma_encoding_format_wav,
        ma_format_s16,  // 16-bit PCM
        1,              // Mono
        16000           // 16kHz sample rate (optimal for Whisper)
    );

    ma_encoder* encoder = new ma_encoder();
    if (ma_encoder_init_file(m_lastRecordingPath.c_str(), &encoderConfig, encoder) != MA_SUCCESS) {
        std::cerr << "Failed to initialize encoder!" << std::endl;
        delete encoder;
        return false;
    }
    m_encoder = encoder;

    // Configure capture device
    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = ma_format_s16;
    deviceConfig.capture.channels = 1;
    deviceConfig.sampleRate       = 16000;
    deviceConfig.dataCallback     = [](ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
        AudioRecorder* recorder = (AudioRecorder*)pDevice->pUserData;
        if (recorder && recorder->m_encoder) {
            ma_encoder* encoder = (ma_encoder*)recorder->m_encoder;
            ma_encoder_write_pcm_frames(encoder, pInput, frameCount, nullptr);
        }
    };
    deviceConfig.pUserData = this;

    ma_device* device = new ma_device();
    if (ma_device_init(nullptr, &deviceConfig, device) != MA_SUCCESS) {
        std::cerr << "Failed to initialize capture device!" << std::endl;
        ma_encoder_uninit(encoder);
        delete encoder;
        delete device;
        m_encoder = nullptr;
        return false;
    }
    m_device = device;

    if (ma_device_start(device) != MA_SUCCESS) {
        std::cerr << "Failed to start device!" << std::endl;
        ma_device_uninit(device);
        ma_encoder_uninit(encoder);
        delete device;
        delete encoder;
        m_device = nullptr;
        m_encoder = nullptr;
        return false;
    }

    m_isRecording = true;
    std::cout << "🔴 Aufnahme gestartet: " << m_lastRecordingPath << std::endl;
    return true;
}

bool AudioRecorder::stopRecording() {
    if (!m_isRecording) {
        std::cerr << "Not recording!" << std::endl;
        return false;
    }

    // Stop and cleanup device
    if (m_device) {
        ma_device* device = (ma_device*)m_device;
        ma_device_uninit(device);
        delete device;
        m_device = nullptr;
    }

    // Cleanup encoder
    if (m_encoder) {
        ma_encoder* encoder = (ma_encoder*)m_encoder;
        ma_encoder_uninit(encoder);
        delete encoder;
        m_encoder = nullptr;
    }

    m_isRecording = false;
    std::cout << "⏹️  Aufnahme gestoppt: " << m_lastRecordingPath << std::endl;
    return true;
}
