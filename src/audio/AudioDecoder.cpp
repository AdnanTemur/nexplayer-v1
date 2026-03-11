#include "AudioDecoder.h"
#include <iostream>
#include <cstring>
#include <QDebug>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

AudioDecoder::AudioDecoder()
{
    std::cout << "AudioDecoder created" << std::endl;
}

AudioDecoder::~AudioDecoder()
{
    cleanup();
}

bool AudioDecoder::openFile(const std::string &filename)
{
    cleanup();

    // Open input file
    if (avformat_open_input(&m_formatContext, filename.c_str(), nullptr, nullptr) < 0)
    {
        std::cerr << "AudioDecoder: Could not open file: " << filename << std::endl;
        return false;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0)
    {
        std::cerr << "AudioDecoder: Could not find stream information" << std::endl;
        cleanup();
        return false;
    }

    // Find audio stream
    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++)
    {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_audioStreamIndex = i;
            break;
        }
    }

    if (m_audioStreamIndex == -1)
    {
        std::cerr << "AudioDecoder: No audio stream found" << std::endl;
        cleanup();
        return false;
    }

    m_hasAudio = true;

    // Initialize audio codec
    if (!initializeAudioCodec())
    {
        cleanup();
        return false;
    }

    m_isOpen = true;

    std::cout << "AudioDecoder: File opened successfully" << std::endl;
    std::cout << "  Sample rate: " << m_sampleRate << " Hz" << std::endl;
    std::cout << "  Channels: " << m_channels << std::endl;

    return true;
}

bool AudioDecoder::initializeAudioCodec()
{
    AVStream *audioStream = m_formatContext->streams[m_audioStreamIndex];

    // Find decoder
    const AVCodec *codec = avcodec_find_decoder(audioStream->codecpar->codec_id);
    if (!codec)
    {
        std::cerr << "AudioDecoder: Codec not found" << std::endl;
        return false;
    }

    // Allocate codec context
    m_audioCodecContext = avcodec_alloc_context3(codec);
    if (!m_audioCodecContext)
    {
        std::cerr << "AudioDecoder: Could not allocate codec context" << std::endl;
        return false;
    }

    // Copy codec parameters
    if (avcodec_parameters_to_context(m_audioCodecContext, audioStream->codecpar) < 0)
    {
        std::cerr << "AudioDecoder: Could not copy codec parameters" << std::endl;
        return false;
    }

    // Open codec
    if (avcodec_open2(m_audioCodecContext, codec, nullptr) < 0)
    {
        std::cerr << "AudioDecoder: Could not open codec" << std::endl;
        return false;
    }

    // Get audio properties
    m_sampleRate = m_audioCodecContext->sample_rate;
    m_channels = m_audioCodecContext->ch_layout.nb_channels;

    // Allocate frame and packet
    m_frame = av_frame_alloc();
    m_packet = av_packet_alloc();

    if (!m_frame || !m_packet)
    {
        std::cerr << "AudioDecoder: Could not allocate frame/packet" << std::endl;
        return false;
    }

    // Initialize resampler (convert to float planar)
    m_swrContext = swr_alloc();
    if (!m_swrContext)
    {
        std::cerr << "AudioDecoder: Could not allocate resampler" << std::endl;
        return false;
    }

    // Set resampler options
    av_opt_set_chlayout(m_swrContext, "in_chlayout", &m_audioCodecContext->ch_layout, 0);
    av_opt_set_int(m_swrContext, "in_sample_rate", m_sampleRate, 0);
    av_opt_set_sample_fmt(m_swrContext, "in_sample_fmt", m_audioCodecContext->sample_fmt, 0);

    AVChannelLayout out_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
    av_opt_set_chlayout(m_swrContext, "out_chlayout", &out_ch_layout, 0);
    av_opt_set_int(m_swrContext, "out_sample_rate", m_sampleRate, 0);
    av_opt_set_sample_fmt(m_swrContext, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

    if (swr_init(m_swrContext) < 0)
    {
        std::cerr << "AudioDecoder: Could not initialize resampler" << std::endl;
        return false;
    }

    return true;
}

void AudioDecoder::close()
{
    cleanup();
}

void AudioDecoder::cleanup()
{
    if (m_swrContext)
    {
        swr_free(&m_swrContext);
    }

    if (m_frame)
    {
        av_frame_free(&m_frame);
    }

    if (m_packet)
    {
        av_packet_free(&m_packet);
    }

    if (m_audioCodecContext)
    {
        avcodec_free_context(&m_audioCodecContext);
    }

    if (m_formatContext)
    {
        avformat_close_input(&m_formatContext);
    }

    m_audioBuffer.clear();
    m_bufferReadPos = 0;
    m_isOpen = false;
    m_hasAudio = false;
    m_audioStreamIndex = -1;
}

bool AudioDecoder::readAudioSamples(float *outputBuffer, int numSamples)
{
    if (!m_isOpen || !m_hasAudio)
    {
        // Fill with silence
        std::memset(outputBuffer, 0, numSamples * m_channels * sizeof(float));
        return false;
    }

    int samplesNeeded = numSamples * m_channels;
    int samplesWritten = 0;

    // First, use any buffered samples
    while (m_bufferReadPos < m_audioBuffer.size() && samplesWritten < samplesNeeded)
    {
        outputBuffer[samplesWritten++] = m_audioBuffer[m_bufferReadPos++];
    }

    // If we need more samples, decode more frames
    while (samplesWritten < samplesNeeded)
    {
        // Read packet
        int ret = av_read_frame(m_formatContext, m_packet);
        if (ret < 0)
        {
            // End of file - fill remaining with silence
            while (samplesWritten < samplesNeeded)
            {
                outputBuffer[samplesWritten++] = 0.0f;
            }
            return false;
        }

        // Check if packet is from audio stream
        if (m_packet->stream_index != m_audioStreamIndex)
        {
            av_packet_unref(m_packet);
            continue;
        }

        // Send packet to decoder
        ret = avcodec_send_packet(m_audioCodecContext, m_packet);
        av_packet_unref(m_packet);

        if (ret < 0)
        {
            continue;
        }

        // Receive decoded frame
        ret = avcodec_receive_frame(m_audioCodecContext, m_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            continue;
        }
        else if (ret < 0)
        {
            continue;
        }

        // Calculate output samples
        int out_samples = av_rescale_rnd(
            swr_get_delay(m_swrContext, m_sampleRate) + m_frame->nb_samples,
            m_sampleRate,
            m_sampleRate,
            AV_ROUND_UP);

        // Allocate output buffer (stereo)
        std::vector<float> converted(out_samples * 2);
        uint8_t *out_ptr = reinterpret_cast<uint8_t *>(converted.data());

        // Convert samples
        int converted_samples = swr_convert(
            m_swrContext,
            &out_ptr,
            out_samples,
            const_cast<const uint8_t **>(m_frame->data),
            m_frame->nb_samples);

        if (converted_samples > 0)
        {
            int total_samples = converted_samples * 2; // stereo

            // Copy what we need to output buffer
            int samples_to_copy = std::min(total_samples, samplesNeeded - samplesWritten);

            std::memcpy(
                outputBuffer + samplesWritten,
                converted.data(),
                samples_to_copy * sizeof(float));
            samplesWritten += samples_to_copy;

            // Buffer any remaining samples
            if (samples_to_copy < total_samples)
            {
                m_audioBuffer.clear();
                m_audioBuffer.insert(
                    m_audioBuffer.end(),
                    converted.begin() + samples_to_copy,
                    converted.begin() + total_samples);
                m_bufferReadPos = 0;
            }
        }
    }

    return true;
}

bool AudioDecoder::seekToTime(double seconds)
{
    if (!m_formatContext || m_audioStreamIndex < 0)
    {
        return false;
    }

    // Convert seconds to stream time base
    AVStream *stream = m_formatContext->streams[m_audioStreamIndex];
    int64_t timestamp = (int64_t)(seconds / av_q2d(stream->time_base));

    // Seek in the stream
    int ret = av_seek_frame(m_formatContext, m_audioStreamIndex, timestamp,
                            AVSEEK_FLAG_BACKWARD);

    if (ret < 0)
    {
        qWarning() << "Audio seek failed";
        return false;
    }

    // Flush codec buffers
    if (m_audioCodecContext)
    {
        avcodec_flush_buffers(m_audioCodecContext);
    }

    // Clear internal audio buffer
    m_audioBuffer.clear();
    m_bufferReadPos = 0;

    qDebug() << "🎵 Audio seeked to" << seconds << "seconds";
    return true;
}

double AudioDecoder::getCurrentTime() const
{
    if (!m_formatContext || m_audioStreamIndex < 0)
    {
        return 0.0;
    }

    // Get current timestamp from last decoded frame
    if (m_frame && m_frame->pts != AV_NOPTS_VALUE)
    {
        AVStream *stream = m_formatContext->streams[m_audioStreamIndex];
        double timeBase = av_q2d(stream->time_base);
        return m_frame->pts * timeBase;
    }

    return 0.0;
}

double AudioDecoder::getDuration() const
{
    if (!m_formatContext)
        return 0.0;

    if (m_formatContext->duration != AV_NOPTS_VALUE)
    {
        return m_formatContext->duration / (double)AV_TIME_BASE;
    }
    return 0.0;
}