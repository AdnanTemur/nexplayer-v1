#include "AudioOutput.h"
#include "AudioDecoder.h"
#include "EffectChain.h"
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QDebug>
#include <cstring>
#include <algorithm>

// AudioOutputDevice implementation
AudioOutputDevice::AudioOutputDevice(AudioDecoder *decoder, EffectChain *effectChain, QObject *parent)
    : QIODevice(parent)
    , m_decoder(decoder)
    , m_effectChain(effectChain)
{
    // Must open in ReadOnly mode BEFORE passing to QAudioSink
    open(QIODevice::ReadOnly | QIODevice::Unbuffered);
    qDebug() << "AudioOutputDevice created and opened";
}

qint64 AudioOutputDevice::readData(char *data, qint64 maxlen)
{
    static int callCount = 0;
    bool shouldLog = (callCount++ % 100 == 0);
    
    if (shouldLog) {
        qDebug() << "🔊 Audio callback called, requesting" << maxlen << "bytes";
    }
    
    if (!m_decoder) {
        std::memset(data, 0, maxlen);
        return maxlen;
    }
    
    // Calculate number of samples (stereo float)
    int numSamples = maxlen / sizeof(float) / 2; // Divide by 2 for stereo
    
    // Read from decoder
    float *floatData = reinterpret_cast<float*>(data);
    
    bool success = m_decoder->readAudioSamples(floatData, numSamples);
    
    if (success) {
        // 🎛️ PROCESS THROUGH EFFECTS! This is where the magic happens!
        if (m_effectChain) {
            m_effectChain->process(floatData, numSamples);
        }
        
        // Apply volume
        if (m_volume != 1.0f) {
            int totalSamples = numSamples * 2; // stereo
            for (int i = 0; i < totalSamples; ++i) {
                floatData[i] *= m_volume;
            }
        }
        
        // Debug: check audio levels
        if (shouldLog) {
            float maxVal = 0.0f;
            for (int i = 0; i < numSamples * 2 && i < 100; ++i) {
                maxVal = std::max(maxVal, std::abs(floatData[i]));
            }
            qDebug() << "🎵 Audio data max value:" << maxVal << "volume:" << m_volume;
        }
        
        return maxlen;
    }
    
    // If failed, fill with silence
    std::memset(data, 0, maxlen);
    return maxlen;
}

qint64 AudioOutputDevice::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    return 0; // Read-only device
}

qint64 AudioOutputDevice::bytesAvailable() const
{
    // Tell Qt we always have data available
    return std::numeric_limits<qint64>::max();
}

bool AudioOutputDevice::isSequential() const
{
    return true;
}

// AudioOutput implementation
AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent)
{
    qDebug() << "AudioOutput created";
}

AudioOutput::~AudioOutput()
{
    stop();
}

bool AudioOutput::initialize(AudioDecoder *decoder, EffectChain *effectChain)
{
    if (!decoder || !decoder->hasAudio()) {
        qWarning() << "AudioOutput: Invalid decoder or no audio";
        return false;
    }
    
    // Setup audio format
    QAudioFormat format;
    format.setSampleRate(decoder->getSampleRate());
    format.setChannelCount(2); // Stereo
    format.setSampleFormat(QAudioFormat::Float);
    
    // Initialize effect chain with correct sample rate
    if (effectChain) {
        effectChain->setSampleRate(decoder->getSampleRate());
    }
    
    // Get default audio output device
    QAudioDevice outputDevice = QMediaDevices::defaultAudioOutput();
    
    if (outputDevice.isNull()) {
        qWarning() << "AudioOutput: No audio output device found";
        emit error("No audio output device found");
        return false;
    }
    
    // Check if format is supported
    if (!outputDevice.isFormatSupported(format)) {
        qWarning() << "AudioOutput: Audio format not supported";
        qWarning() << "  Requested:" << format.sampleRate() << "Hz," 
                   << format.channelCount() << "channels, Float";
        emit error("Audio format not supported");
        return false;
    }
    
    // Create audio sink
    m_audioSink = std::make_unique<QAudioSink>(outputDevice, format);
    
    // Create audio device with effect chain
    m_audioDevice = new AudioOutputDevice(decoder, effectChain, this);
    m_audioDevice->setVolume(m_volume);
    
    qDebug() << "AudioOutput initialized:";
    qDebug() << "  Sample rate:" << format.sampleRate() << "Hz";
    qDebug() << "  Channels:" << format.channelCount();
    qDebug() << "  Format: Float";
    qDebug() << "  Effects: Enabled";
    
    return true;
}

void AudioOutput::start()
{
    if (!m_audioSink || !m_audioDevice) {
        qWarning() << "AudioOutput: Not initialized";
        return;
    }
    
    // Only call start() if not already started
    if (m_audioSink->state() == QAudio::StoppedState || 
        m_audioSink->state() == QAudio::IdleState) {
        m_audioSink->setVolume(m_volume);
        m_audioSink->start(m_audioDevice);
        m_playing = true;
        qDebug() << "AudioOutput: Started";
    } else if (m_audioSink->state() == QAudio::SuspendedState) {
        // Was paused, resume instead
        resume();
    }
}

void AudioOutput::stop()
{
    if (!m_audioSink) {
        return;
    }
    
    m_audioSink->stop();
    m_playing = false;
    
    qDebug() << "AudioOutput: Stopped";
}

void AudioOutput::pause()
{
    if (!m_audioSink) {
        return;
    }
    
    m_audioSink->suspend();
    m_playing = false;
    
    qDebug() << "AudioOutput: Paused";
}

void AudioOutput::resume()
{
    if (!m_audioSink) {
        return;
    }
    
    m_audioSink->resume();
    m_playing = true;
    
    qDebug() << "AudioOutput: Resumed";
}

void AudioOutput::setVolume(float volume)
{
    m_volume = qBound(0.0f, volume, 1.0f);
    
    if (m_audioSink) {
        m_audioSink->setVolume(m_volume);
    }
    
    if (m_audioDevice) {
        m_audioDevice->setVolume(m_volume);
    }
}