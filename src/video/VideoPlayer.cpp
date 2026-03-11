#include "VideoPlayer.h"
#include "VideoDecoder.h"
#include "../audio/AudioDecoder.h"
#include "../audio/AudioOutput.h"
#include "../audio/EffectChain.h"
#include <QImage>
#include <QDebug>

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent), m_videoDecoder(std::make_unique<VideoDecoder>()), m_audioDecoder(std::make_unique<AudioDecoder>()), m_audioOutput(std::make_unique<AudioOutput>()), m_effectChain(std::make_unique<EffectChain>()), m_frameTimer(new QTimer(this))
{
    connect(m_frameTimer, &QTimer::timeout, this, &VideoPlayer::readNextFrame);
    qDebug() << "VideoPlayer created with effects";
}

VideoPlayer::~VideoPlayer()
{
    stop();
}

bool VideoPlayer::openFile(const std::string &filename)
{
    stop();

    // Open video decoder (might fail for audio-only files)
    bool hasVideo = m_videoDecoder->openFile(filename);

    // Open audio decoder (required)
    bool hasAudio = m_audioDecoder->openFile(filename);

    if (!hasVideo && !hasAudio)
    {
        emit error("Failed to open file - no video or audio streams found");
        return false;
    }

    // Initialize audio output if we have audio
    if (hasAudio)
    {
        if (!m_audioOutput->initialize(m_audioDecoder.get(), m_effectChain.get()))
        {
            qWarning() << "Failed to initialize audio output";
            hasAudio = false;
        }
    }

    // Calculate frame interval for video (or default for audio-only)
    if (hasVideo)
    {
        double fps = m_videoDecoder->getFrameRate();
        if (fps > 0)
        {
            m_frameInterval = 1000.0 / fps;
        }
        else
        {
            m_frameInterval = 1000.0 / 30.0;
        }
    }
    else
    {
        // Audio-only: use timer to update position display
        m_frameInterval = 100.0; // Update position every 100ms
    }

    qDebug() << "File opened:" << filename.c_str();
    if (hasVideo)
    {
        qDebug() << "  Video:" << m_videoDecoder->getWidth() << "x" << m_videoDecoder->getHeight();
        qDebug() << "  FPS:" << m_videoDecoder->getFrameRate();
    }
    else
    {
        qDebug() << "  Audio-only file";
    }
    qDebug() << "  Duration:" << getDuration() << "seconds";
    qDebug() << "  Has audio:" << hasAudio;

    emit durationChanged(getDuration());

    // Read first frame if video exists
    if (hasVideo && m_videoDecoder->readFrame())
    {
        int width = m_videoDecoder->getWidth();
        int height = m_videoDecoder->getHeight();
        const uint8_t *data = m_videoDecoder->getFrameData();

        QImage frame(data, width, height, width * 3, QImage::Format_RGB888);
        emit frameReady(frame.copy());
    }

    return true;
}

void VideoPlayer::play()
{
    m_playing = true;
    m_frameTimer->start(static_cast<int>(m_frameInterval));

    // Start/resume audio
    if (m_audioDecoder->hasAudio() && m_audioOutput)
    {
        m_audioOutput->start();
    }

    qDebug() << "Playing (frame interval:" << m_frameInterval << "ms)";
}

void VideoPlayer::pause()
{
    m_playing = false;
    m_frameTimer->stop();

    // Pause audio
    if (m_audioDecoder->hasAudio())
    {
        m_audioOutput->pause();
    }

    qDebug() << "Paused";
}

void VideoPlayer::stop()
{
    m_playing = false;
    m_frameTimer->stop();

    // Stop audio
    if (m_audioDecoder->hasAudio())
    {
        m_audioOutput->stop();
    }

    m_videoDecoder->close();
    m_audioDecoder->close();

    qDebug() << "Stopped";
}

void VideoPlayer::seek(double seconds)
{
    bool hasVideo = m_videoDecoder->hasVideo();

    if (hasVideo)
    {
        // Seek video
        if (m_videoDecoder->seekToTime(seconds))
        {
            // Seek audio too
            if (m_audioDecoder->hasAudio())
            {
                m_audioDecoder->seekToTime(seconds);
            }

            // Read frame at new position
            if (m_videoDecoder->readFrame())
            {
                int width = m_videoDecoder->getWidth();
                int height = m_videoDecoder->getHeight();
                const uint8_t *data = m_videoDecoder->getFrameData();

                QImage frame(data, width, height, width * 3, QImage::Format_RGB888);
                emit frameReady(frame.copy());
                emit positionChanged(seconds);
            }
        }
    }
    else
    {
        // Audio-only: just seek audio
        if (m_audioDecoder->hasAudio())
        {
            if (m_audioDecoder->seekToTime(seconds))
            {
                emit positionChanged(seconds);
                qDebug() << "🎵 Audio-only seek to" << seconds;
            }
        }
    }
}

double VideoPlayer::getDuration() const
{
    if (m_videoDecoder->hasVideo())
    {
        return m_videoDecoder->getDuration();
    }
    else if (m_audioDecoder->hasAudio())
    {
        return m_audioDecoder->getDuration();
    }
    return 0.0;
}

double VideoPlayer::getPosition() const
{
    if (m_videoDecoder->hasVideo())
    {
        return m_videoDecoder->getCurrentTime();
    }
    else if (m_audioDecoder->hasAudio())
    {
        return m_audioDecoder->getCurrentTime();
    }
    return 0.0;
}

int VideoPlayer::getWidth() const
{
    return m_videoDecoder->getWidth();
}

int VideoPlayer::getHeight() const
{
    return m_videoDecoder->getHeight();
}

void VideoPlayer::setVolume(float volume)
{
    m_audioOutput->setVolume(volume);
}

float VideoPlayer::getVolume() const
{
    return m_audioOutput->getVolume();
}

void VideoPlayer::readNextFrame()
{
    if (!m_playing)
    {
        return;
    }

    bool hasVideo = m_videoDecoder->hasVideo();

    if (hasVideo)
    {
        // Normal video playback
        if (m_videoDecoder->readFrame())
        {
            int width = m_videoDecoder->getWidth();
            int height = m_videoDecoder->getHeight();
            const uint8_t *data = m_videoDecoder->getFrameData();

            QImage frame(data, width, height, width * 3, QImage::Format_RGB888);
            emit frameReady(frame.copy());

            double pos = m_videoDecoder->getCurrentTime();
            emit positionChanged(pos);
        }
        else
        {
            // End of video
            if (m_loop)
            {
                qDebug() << "🔁 Looping video";
                seek(0.0);
                play();
            }
            else
            {
                pause();
                emit endOfFile();
                qDebug() << "⏹️ End of file reached";
            }
        }
    }
    else
    {
        // Audio-only: just update position from audio decoder
        if (m_audioDecoder->hasAudio())
        {
            double pos = m_audioDecoder->getCurrentTime();
            emit positionChanged(pos);

            // Check if we've reached the end
            if (pos >= getDuration() - 0.1)
            {
                if (m_loop)
                {
                    qDebug() << "🔁 Looping audio";
                    seek(0.0);
                    play();
                }
                else
                {
                    pause();
                    emit endOfFile();
                    qDebug() << "⏹️ End of audio reached";
                }
            }
        }
    }
}

// NEW: Advanced playback control methods
void VideoPlayer::skipForward(double seconds)
{
    double newPos = getPosition() + seconds;
    double duration = getDuration();

    if (newPos > duration)
    {
        newPos = duration;
    }

    seek(newPos);
    qDebug() << "⏭️ Skipped forward" << seconds << "seconds to" << newPos;
}

void VideoPlayer::skipBackward(double seconds)
{
    double newPos = getPosition() - seconds;

    if (newPos < 0.0)
    {
        newPos = 0.0;
    }

    seek(newPos);
    qDebug() << "⏮️ Skipped backward" << seconds << "seconds to" << newPos;
}

void VideoPlayer::setPlaybackSpeed(double speed)
{
    // Clamp speed to reasonable range
    m_playbackSpeed = std::clamp(speed, 0.25, 4.0);

    // Update frame timer interval
    if (m_frameInterval > 0)
    {
        double adjustedInterval = m_frameInterval / m_playbackSpeed;
        m_frameTimer->setInterval(static_cast<int>(adjustedInterval));
    }

    // Mute audio if not at normal speed (audio time-stretching is complex)
    if (m_audioDecoder->hasAudio() && m_audioOutput)
    {
        if (m_playbackSpeed != 1.0)
        {
            // Pause audio at non-normal speeds
            if (m_playing)
            {
                m_audioOutput->pause();
                qDebug() << "⏸️ Audio paused (speed != 1x)";
            }
        }
        else
        {
            // Resume audio at normal speed
            if (m_playing)
            {
                m_audioOutput->resume();
                qDebug() << "▶️ Audio resumed (speed = 1x)";
            }
        }
    }

    qDebug() << "⚡ Playback speed set to" << m_playbackSpeed << "x";
}

void VideoPlayer::stepForward()
{
    // Remember if we were playing
    bool wasPlaying = m_playing;

    // Pause playback
    if (wasPlaying)
    {
        pause();
    }

    // Read next frame
    if (m_videoDecoder->readFrame())
    {
        int width = m_videoDecoder->getWidth();
        int height = m_videoDecoder->getHeight();
        const uint8_t *data = m_videoDecoder->getFrameData();

        QImage frame(data, width, height, width * 3, QImage::Format_RGB888);
        emit frameReady(frame.copy());

        double pos = m_videoDecoder->getCurrentTime();
        emit positionChanged(pos);

        qDebug() << "▶️ Stepped forward to" << pos;
    }

    // Don't resume - user is manually stepping
}

void VideoPlayer::stepBackward()
{
    // Remember if we were playing
    bool wasPlaying = m_playing;

    // Pause playback
    if (wasPlaying)
    {
        pause();
    }

    // Go back one frame
    double currentPos = getPosition();
    double frameDuration = 1.0 / (m_videoDecoder->getFrameRate() > 0 ? m_videoDecoder->getFrameRate() : 30.0);

    double targetPos = currentPos - frameDuration;
    if (targetPos < 0.0)
    {
        targetPos = 0.0;
    }

    seek(targetPos);
    qDebug() << "◀️ Stepped backward to" << targetPos;

    // Don't resume - user is manually stepping
}

QImage VideoPlayer::captureFrame()
{
    if (m_videoDecoder && m_videoDecoder->hasVideo())
    {
        int width = m_videoDecoder->getWidth();
        int height = m_videoDecoder->getHeight();
        const uint8_t *data = m_videoDecoder->getFrameData();

        if (data)
        {
            QImage frame(data, width, height, width * 3, QImage::Format_RGB888);
            qDebug() << "📸 Frame captured:" << width << "x" << height;
            return frame.copy();
        }
    }

    qDebug() << "❌ Failed to capture frame";
    return QImage();
}