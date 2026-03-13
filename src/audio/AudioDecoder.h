#pragma once

#include <string> 
#include <vector>
#include <memory>
#include <cstdint>

// Forward declarations for FFmpeg
struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwrContext;

/**
 * @brief Audio decoder using FFmpeg
 *
 * Extracts and decodes audio from media files.
 * Outputs PCM audio samples ready for playback.
 */
class AudioDecoder
{
public:
    AudioDecoder();
    ~AudioDecoder();

    // Setup
    bool openFile(const std::string &filename);
    void close();

    // Audio extraction
    bool readAudioSamples(float *outputBuffer, int numSamples);

    // Properties
    int getSampleRate() const { return m_sampleRate; }
    int getChannels() const { return m_channels; }
    bool hasAudio() const { return m_hasAudio; }
    double getCurrentTime() const;
    double getDuration() const;

    // Seeking
    bool seekToTime(double seconds);

private:
    void cleanup();
    bool initializeAudioCodec();

    AVFormatContext *m_formatContext = nullptr;
    AVCodecContext *m_audioCodecContext = nullptr;
    SwrContext *m_swrContext = nullptr;

    AVFrame *m_frame = nullptr;
    AVPacket *m_packet = nullptr;

    int m_audioStreamIndex = -1;
    int m_sampleRate = 44100;
    int m_channels = 2;
    bool m_hasAudio = false;
    bool m_isOpen = false;

    // Audio buffer for decoded samples
    std::vector<float> m_audioBuffer;
    size_t m_bufferReadPos = 0;
};
