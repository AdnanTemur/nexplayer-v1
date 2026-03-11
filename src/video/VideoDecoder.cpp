#include "VideoDecoder.h"
#include <iostream>

// FFmpeg headers
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

VideoDecoder::VideoDecoder()
{
    std::cout << "VideoDecoder created" << std::endl;
}

VideoDecoder::~VideoDecoder()
{
    cleanup();
}

bool VideoDecoder::openFile(const std::string &filename)
{
    cleanup();
    
    // Open input file
    if (avformat_open_input(&m_formatContext, filename.c_str(), nullptr, nullptr) < 0) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }
    
    // Retrieve stream information
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        cleanup();
        return false;
    }
    
    // Find video and audio streams
    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStreamIndex == -1) {
            m_videoStreamIndex = i;
        }
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStreamIndex == -1) {
            m_audioStreamIndex = i;
        }
    }
    
    m_hasVideo = (m_videoStreamIndex >= 0);
    m_hasAudio = (m_audioStreamIndex >= 0);
    
    if (!m_hasVideo && !m_hasAudio) {
        std::cerr << "No video or audio streams found" << std::endl;
        cleanup();
        return false;
    }
    
    // Initialize video codec if present
    if (m_hasVideo) {
        if (!initializeCodecs()) {
            cleanup();
            return false;
        }
    }
    
    m_isOpen = true;
    
    // Get duration
    if (m_formatContext->duration != AV_NOPTS_VALUE) {
        m_duration = m_formatContext->duration / static_cast<double>(AV_TIME_BASE);
    }
    
    std::cout << "File opened successfully: " << filename << std::endl;
    std::cout << "  Duration: " << m_duration << " seconds" << std::endl;
    std::cout << "  Has video: " << m_hasVideo << std::endl;
    std::cout << "  Has audio: " << m_hasAudio << std::endl;
    
    return true;
}

bool VideoDecoder::initializeCodecs()
{
    if (m_videoStreamIndex < 0) return false;
    
    AVStream *videoStream = m_formatContext->streams[m_videoStreamIndex];
    
    // Find decoder
    const AVCodec *codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (!codec) {
        std::cerr << "Codec not found" << std::endl;
        return false;
    }
    
    // Allocate codec context
    m_videoCodecContext = avcodec_alloc_context3(codec);
    if (!m_videoCodecContext) {
        std::cerr << "Could not allocate codec context" << std::endl;
        return false;
    }
    
    // Copy codec parameters
    if (avcodec_parameters_to_context(m_videoCodecContext, videoStream->codecpar) < 0) {
        std::cerr << "Could not copy codec parameters" << std::endl;
        return false;
    }
    
    // Open codec
    if (avcodec_open2(m_videoCodecContext, codec, nullptr) < 0) {
        std::cerr << "Could not open codec" << std::endl;
        return false;
    }
    
    // Get video properties
    m_width = m_videoCodecContext->width;
    m_height = m_videoCodecContext->height;
    m_frameRate = av_q2d(videoStream->avg_frame_rate);
    
    // Allocate frames
    m_frame = av_frame_alloc();
    m_rgbFrame = av_frame_alloc();
    m_packet = av_packet_alloc();
    
    if (!m_frame || !m_rgbFrame || !m_packet) {
        std::cerr << "Could not allocate frames" << std::endl;
        return false;
    }
    
    // Initialize SWS context for RGB conversion
    m_swsContext = sws_getContext(
        m_width, m_height, m_videoCodecContext->pix_fmt,
        m_width, m_height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    
    if (!m_swsContext) {
        std::cerr << "Could not initialize SWS context" << std::endl;
        return false;
    }
    
    // Allocate RGB buffer
    m_rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_width, m_height, 1);
    m_rgbBuffer = static_cast<uint8_t*>(av_malloc(m_rgbBufferSize));
    
    av_image_fill_arrays(m_rgbFrame->data, m_rgbFrame->linesize, m_rgbBuffer, 
                        AV_PIX_FMT_RGB24, m_width, m_height, 1);
    
    return true;
}

void VideoDecoder::close()
{
    cleanup();
}

void VideoDecoder::cleanup()
{
    if (m_rgbBuffer) {
        av_free(m_rgbBuffer);
        m_rgbBuffer = nullptr;
    }
    
    if (m_swsContext) {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;
    }
    
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    
    if (m_rgbFrame) {
        av_frame_free(&m_rgbFrame);
    }
    
    if (m_packet) {
        av_packet_free(&m_packet);
    }
    
    if (m_videoCodecContext) {
        avcodec_free_context(&m_videoCodecContext);
    }
    
    if (m_formatContext) {
        avformat_close_input(&m_formatContext);
    }
    
    m_isOpen = false;
    m_hasVideo = false;
    m_hasAudio = false;
    m_videoStreamIndex = -1;
    m_audioStreamIndex = -1;
}

bool VideoDecoder::readFrame()
{
    if (!m_isOpen || m_videoStreamIndex < 0) {
        return false;
    }
    
    while (av_read_frame(m_formatContext, m_packet) >= 0) {
        // Check if packet is from video stream
        if (m_packet->stream_index == m_videoStreamIndex) {
            // Send packet to decoder
            int ret = avcodec_send_packet(m_videoCodecContext, m_packet);
            av_packet_unref(m_packet);
            
            if (ret < 0) {
                std::cerr << "Error sending packet to decoder" << std::endl;
                return false;
            }
            
            // Receive decoded frame
            ret = avcodec_receive_frame(m_videoCodecContext, m_frame);
            
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                continue;
            } else if (ret < 0) {
                std::cerr << "Error receiving frame from decoder" << std::endl;
                return false;
            }
            
            // Convert frame to RGB
            sws_scale(
                m_swsContext,
                m_frame->data,
                m_frame->linesize,
                0,
                m_height,
                m_rgbFrame->data,
                m_rgbFrame->linesize
            );
            
            return true;
        }
        
        av_packet_unref(m_packet);
    }
    
    return false;
}

bool VideoDecoder::seekToTime(double seconds)
{
    if (!m_isOpen || m_videoStreamIndex < 0) {
        return false;
    }
    
    AVStream *videoStream = m_formatContext->streams[m_videoStreamIndex];
    int64_t timestamp = static_cast<int64_t>(seconds * AV_TIME_BASE);
    
    // Convert to stream time base
    timestamp = av_rescale_q(timestamp, AV_TIME_BASE_Q, videoStream->time_base);
    
    // Seek to timestamp
    if (av_seek_frame(m_formatContext, m_videoStreamIndex, timestamp, AVSEEK_FLAG_BACKWARD) < 0) {
        std::cerr << "Error seeking to time: " << seconds << std::endl;
        return false;
    }
    
    // Flush codec buffers
    avcodec_flush_buffers(m_videoCodecContext);
    
    return true;
}

double VideoDecoder::getDuration() const
{
    return m_duration;
}

double VideoDecoder::getCurrentTime() const
{
    if (!m_frame || m_videoStreamIndex < 0) {
        return 0.0;
    }
    
    AVStream *videoStream = m_formatContext->streams[m_videoStreamIndex];
    
    // Get PTS (Presentation TimeStamp) of current frame
    int64_t pts = m_frame->pts;
    if (pts == AV_NOPTS_VALUE) {
        pts = m_frame->best_effort_timestamp;
    }
    
    if (pts != AV_NOPTS_VALUE) {
        return pts * av_q2d(videoStream->time_base);
    }
    
    return 0.0;
}

int VideoDecoder::getWidth() const
{
    return m_width;
}

int VideoDecoder::getHeight() const
{
    return m_height;
}

double VideoDecoder::getFrameRate() const
{
    return m_frameRate;
}

const uint8_t* VideoDecoder::getFrameData() const
{
    return m_rgbBuffer;
}

int VideoDecoder::getFrameSize() const
{
    return m_rgbBufferSize;
}
