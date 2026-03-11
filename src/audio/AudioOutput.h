#pragma once

#include <QObject>
#include <QIODevice>
#include <memory>

class QAudioSink;
class QAudioFormat;
class AudioDecoder;
class EffectChain;

/**
 * @brief Audio output device for Qt Multimedia
 * 
 * Bridges between AudioDecoder and QAudioSink.
 * Provides audio samples on demand.
 */
class AudioOutputDevice : public QIODevice
{
    Q_OBJECT
    
public:
    explicit AudioOutputDevice(AudioDecoder *decoder, EffectChain *effectChain, QObject *parent = nullptr);
    
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    bool isSequential() const override;
    
    void setVolume(float volume) { m_volume = volume; }
    
private:
    AudioDecoder *m_decoder;
    EffectChain *m_effectChain;
    float m_volume = 1.0f;
};

/**
 * @brief Audio output manager using Qt Multimedia
 * 
 * Handles audio playback through system audio device.
 */
class AudioOutput : public QObject
{
    Q_OBJECT
    
public:
    explicit AudioOutput(QObject *parent = nullptr);
    ~AudioOutput();
    
    bool initialize(AudioDecoder *decoder, EffectChain *effectChain);
    void start();
    void stop();
    void pause();
    void resume();
    
    void setVolume(float volume);
    float getVolume() const { return m_volume; }
    
    bool isPlaying() const { return m_playing; }

signals:
    void error(const QString &message);

private:
    std::unique_ptr<QAudioSink> m_audioSink;
    AudioOutputDevice *m_audioDevice = nullptr;
    
    float m_volume = 1.0f;
    bool m_playing = false;
};