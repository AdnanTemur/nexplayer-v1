#pragma once

#include <QObject>
#include <QUrl>
#include <QString>
#include <QTimer>
#include <memory>
#include "video/VideoRenderer.h"

class VideoPlayer;
class VideoRenderer;
class AudioEngine;
class EffectChain;

/**
 * @brief Main controller connecting QML UI to C++ backend
 * 
 * This class bridges the QML interface with video decoding and audio processing.
 * It exposes properties and methods that QML can bind to and call.
 */
class PlayerController : public QObject
{
    Q_OBJECT
    
    // Properties exposed to QML
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
    Q_PROPERTY(bool hasVideo READ hasVideo NOTIFY hasVideoChanged)
    Q_PROPERTY(bool hasAudio READ hasAudio NOTIFY hasAudioChanged)
    Q_PROPERTY(VideoRenderer* videoRenderer READ videoRenderer CONSTANT)
    Q_PROPERTY(double playbackSpeed READ playbackSpeed NOTIFY playbackSpeedChanged)
    Q_PROPERTY(bool loop READ isLooping NOTIFY loopChanged)

public:
    explicit PlayerController(QObject *parent = nullptr);
    ~PlayerController();
    
    // Property getters
    bool isPlaying() const { return m_playing; }
    double position() const { return m_position; }
    double duration() const { return m_duration; }
    double volume() const { return m_volume; }
    QString currentFile() const { return m_currentFile; }
    bool hasVideo() const { return m_hasVideo; }
    bool hasAudio() const { return m_hasAudio; }
    VideoRenderer* videoRenderer() const { return m_videoRenderer; }
    double playbackSpeed() const;
    
    // Property setters
    void setPosition(double pos);
    void setVolume(double vol);

public slots:
    // Playback control
    void openFile(const QUrl &fileUrl);
    void play();
    void pause();
    void stop();
    void togglePlayPause();
    void seek(double seconds);

    void skipForward(double seconds = 10.0);
    void skipBackward(double seconds = 10.0);
    void setPlaybackSpeed(double speed);
    void setLoop(bool loop);
    bool isLooping() const;
    void stepForward();
    void stepBackward();
    void saveScreenshot(const QString &filename = "");
    
    // Audio effects control
    void enableEffect(const QString &effectName, bool enabled);
    void setEffectParameter(const QString &effectName, const QString &paramName, double value);
    
    // EQ specific
    void setEQBand(int bandIndex, double gain); // gain in dB (-12 to +12)
    
    // Reverb specific
    void setReverbRoomSize(double size);    // 0.0 to 1.0
    void setReverbDamping(double damping);  // 0.0 to 1.0
    void setReverbWetLevel(double wet);     // 0.0 to 1.0
    void setReverbDryLevel(double dry);     // 0.0 to 1.0
    
    // Delay specific
    void setDelayTime(double milliseconds);  // delay time in ms
    void setDelayFeedback(double feedback);  // 0.0 to 1.0
    void setDelayMix(double mix);            // 0.0 to 1.0

     //Compressor Control
    void setCompressorThreshold(double threshold);   // -60 to 0 dB
    void setCompressorRatio(double ratio);           // 1 to 20
    void setCompressorAttack(double attack);         // 0.1 to 100 ms
    void setCompressorRelease(double release);       // 10 to 1000 ms
    void setCompressorMakeupGain(double gain);       // 0 to 20 dB
    
    //Flanger Control
    void setFlangerDepth(double depth);              // 0.0 to 1.0
    void setFlangerRate(double rate);                // 0.1 to 10 Hz
    void setFlangerFeedback(double feedback);        // 0.0 to 0.9
    void setFlangerMix(double mix);                  // 0.0 to 1.0
    
    //Phaser Control
    void setPhaserDepth(double depth);               // 0.0 to 1.0
    void setPhaserRate(double rate);                 // 0.1 to 5 Hz
    void setPhaserFeedback(double feedback);         // 0.0 to 0.9
    void setPhaserStages(int stages);                // 2 to 12
    void setPhaserMix(double mix);                   // 0.0 to 1.0

signals:
    // Property change signals
    void playingChanged(bool playing);
    void positionChanged(double position);
    void durationChanged(double duration);
    void volumeChanged(double volume);
    void currentFileChanged(const QString &file);
    void hasVideoChanged(bool hasVideo);
    void hasAudioChanged(bool hasAudio);
    void playbackSpeedChanged(double speed);
    void loopChanged(bool loop);
    void showNotification(const QString &icon, const QString &message);
    
    // Error signals
    void error(const QString &message);
    
    // Status signals
    void fileLoaded();
    void endOfMedia();

private slots:
    void updatePosition();
    void initializeEffectDefaults(); 

private:
    // State
    bool m_playing = false;
    double m_position = 0.0;
    double m_duration = 0.0;
    double m_volume = 1.0;
    QString m_currentFile;
    bool m_hasVideo = false;
    bool m_hasAudio = false;
    
    // Backend components
    std::unique_ptr<VideoPlayer> m_videoPlayer;
    std::unique_ptr<AudioEngine> m_audioEngine;
    std::unique_ptr<EffectChain> m_effectChain;
    VideoRenderer *m_videoRenderer;
    
    // Timer for position updates
    QTimer *m_positionTimer;
};
