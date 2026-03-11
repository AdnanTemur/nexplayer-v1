#pragma once

#include <string>
#include <memory>

// Forward declarations for FFmpeg
struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;

/**
 * @brief Video decoder using FFmpeg
 * 
 * Handles video file decoding using FFmpeg libraries.
 * Supports virtually all video formats through FFmpeg.
 */
class VideoDecoder
{
public:
    VideoDecoder();
    ~VideoDecoder();
    
    // File operations
    bool openFile(const std::string &filename);
    void close();
    
    // Playback control
    bool readFrame();
    bool seekToTime(double seconds);
    
    // Getters
    double getDuration() const;
    double getCurrentTime() const;
    int getWidth() const;
    int getHeight() const;
    double getFrameRate() const;
    
    // Frame data
    const uint8_t* getFrameData() const;
    int getFrameSize() const;
    
    bool hasVideo() const { return m_hasVideo; }
    bool hasAudio() const { return m_hasAudio; }

private:
    void cleanup();
    bool initializeCodecs();
    
    // FFmpeg contexts
    AVFormatContext *m_formatContext = nullptr;
    AVCodecContext *m_videoCodecContext = nullptr;
    AVCodecContext *m_audioCodecContext = nullptr;
    SwsContext *m_swsContext = nullptr;
    
    // Frames and packets
    AVFrame *m_frame = nullptr;
    AVFrame *m_rgbFrame = nullptr;
    AVPacket *m_packet = nullptr;
    
    // Stream indices
    int m_videoStreamIndex = -1;
    int m_audioStreamIndex = -1;
    
    // Video properties
    int m_width = 0;
    int m_height = 0;
    double m_frameRate = 0.0;
    double m_duration = 0.0;
    
    // State
    bool m_hasVideo = false;
    bool m_hasAudio = false;
    bool m_isOpen = false;
    
    // RGB buffer for frame conversion
    uint8_t *m_rgbBuffer = nullptr;
    int m_rgbBufferSize = 0;
};
