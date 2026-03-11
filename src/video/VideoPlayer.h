#pragma once

#include <QObject>
#include <QTimer>
#include <QImage>
#include <memory>
#include <string>

class VideoDecoder;
class VideoRenderer;
class AudioDecoder;
class AudioOutput;
class EffectChain;

/**
 * @brief High-level video player managing playback
 */
class VideoPlayer : public QObject
{
    Q_OBJECT
    
public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();
    
    bool openFile(const std::string &filename);
    void play();
    void pause();
    void stop();
    void seek(double seconds);
    
    double getDuration() const;
    double getPosition() const;
    bool isPlaying() const { return m_playing; }
    
    int getWidth() const;
    int getHeight() const;
    
    void setVolume(float volume);
    float getVolume() const;
    
    // NEW: Advanced playback control
    void skipForward(double seconds = 10.0);
    void skipBackward(double seconds = 10.0);
    void setPlaybackSpeed(double speed);
    double getPlaybackSpeed() const { return m_playbackSpeed; }
    void setLoop(bool loop) { m_loop = loop; }
    bool isLooping() const { return m_loop; }
    void stepForward();
    void stepBackward();
    QImage captureFrame();
    
    // Access to effect chain for parameter control
    EffectChain* getEffectChain() { return m_effectChain.get(); }

signals:
    void frameReady(const QImage &frame);
    void positionChanged(double position);
    void durationChanged(double duration);
    void endOfFile();
    void error(const QString &message);

private slots:
    void readNextFrame();

private:
    std::unique_ptr<VideoDecoder> m_videoDecoder;
    std::unique_ptr<AudioDecoder> m_audioDecoder;
    std::unique_ptr<AudioOutput> m_audioOutput;
    std::unique_ptr<EffectChain> m_effectChain;
    
    QTimer *m_frameTimer;
    
    bool m_playing = false;
    double m_frameInterval = 0.0;
    double m_playbackSpeed = 1.0;  // NEW
    bool m_loop = false;            // NEW
};