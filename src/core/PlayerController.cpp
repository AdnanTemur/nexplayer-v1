#include "PlayerController.h"
#include "video/VideoDecoder.h"
#include "video/VideoPlayer.h"
#include "video/VideoRenderer.h"
#include "audio/AudioEngine.h"
#include "audio/EffectChain.h"
#include "audio/effects/EQEffect.h"
#include "audio/effects/ReverbEffect.h"
#include "audio/effects/DelayEffect.h"
#include "audio/effects/CompressorEffect.h"
#include "audio/effects/FlangerEffect.h"
#include "audio/effects/PhaserEffect.h"
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>

PlayerController::PlayerController(QObject *parent)
    : QObject(parent), m_videoPlayer(std::make_unique<VideoPlayer>()), m_audioEngine(std::make_unique<AudioEngine>()), m_effectChain(std::make_unique<EffectChain>()), m_videoRenderer(new VideoRenderer()), m_positionTimer(new QTimer(this))
{
    // Connect video player signals
    connect(m_videoPlayer.get(), &VideoPlayer::frameReady,
            m_videoRenderer, &VideoRenderer::updateFrame);

    connect(m_videoPlayer.get(), &VideoPlayer::durationChanged, this, [this](double duration)
            {
        m_duration = duration;
        emit durationChanged(m_duration); });

    connect(m_videoPlayer.get(), &VideoPlayer::positionChanged, this, [this](double position)
            {
        m_position = position;
        emit positionChanged(m_position); });

    connect(m_videoPlayer.get(), &VideoPlayer::endOfFile, this, [this]()
            {
        pause();
        emit endOfMedia(); });

    connect(m_videoPlayer.get(), &VideoPlayer::error, this, [this](const QString &msg)
            { emit error(msg); });

    qDebug() << "PlayerController initialized";
}

PlayerController::~PlayerController()
{
    stop();
}

void PlayerController::openFile(const QUrl &fileUrl)
{
    stop();

    QString filePath = fileUrl.toLocalFile();
    if (filePath.isEmpty())
    {
        filePath = fileUrl.toString();
        if (filePath.startsWith("file://"))
        {
            filePath = filePath.mid(7);
        }
    }

    qDebug() << "Opening file:" << filePath;

    if (m_videoPlayer->openFile(filePath.toStdString()))
    {
        m_currentFile = filePath;
        m_duration = m_videoPlayer->getDuration();
        m_position = 0.0;

        // Check what we actually have
        m_hasVideo = m_videoPlayer->getWidth() > 0 && m_videoPlayer->getHeight() > 0;
        m_hasAudio = true; // Assume audio exists if file opened

        qDebug() << "📂 File loaded:";
        qDebug() << "   Video:" << (m_hasVideo ? "YES" : "NO");
        qDebug() << "   Audio:" << (m_hasAudio ? "YES" : "NO");
        qDebug() << "   Duration:" << m_duration << "seconds";

        emit currentFileChanged(m_currentFile);
        emit durationChanged(m_duration);
        emit positionChanged(m_position);
        emit hasVideoChanged(m_hasVideo);
        emit hasAudioChanged(m_hasAudio);
        emit fileLoaded();

        // Initialize effects to match UI defaults
        initializeEffectDefaults();

        // Auto-play
        play();
    }
    else
    {
        emit error("Failed to open file: " + filePath);
        emit showNotification("❌", "Failed to open file");
    }
}

void PlayerController::play()
{
    if (m_playing)
        return;

    qDebug() << "Playing";
    m_playing = true;
    m_videoPlayer->play();

    emit playingChanged(m_playing);
}

void PlayerController::pause()
{
    if (!m_playing)
        return;

    qDebug() << "Paused";
    m_playing = false;
    m_videoPlayer->pause();

    emit playingChanged(m_playing);
}

void PlayerController::stop()
{
    if (!m_playing && m_position == 0.0)
        return;

    qDebug() << "Stopped";
    m_playing = false;
    m_position = 0.0;
    m_videoPlayer->stop();

    emit playingChanged(m_playing);
    emit positionChanged(m_position);
}

void PlayerController::togglePlayPause()
{
    if (m_playing)
    {
        pause();
    }
    else
    {
        play();
    }
}

void PlayerController::seek(double seconds)
{
    if (seconds < 0)
        seconds = 0;
    if (seconds > m_duration)
        seconds = m_duration;

    m_position = seconds;

    qDebug() << "Seeking to:" << seconds;

    m_videoPlayer->seek(seconds);

    emit positionChanged(m_position);
}

void PlayerController::setPosition(double pos)
{
    seek(pos);
}

void PlayerController::setVolume(double vol)
{
    if (vol < 0.0)
        vol = 0.0;
    if (vol > 1.0)
        vol = 1.0;

    if (qFuzzyCompare(m_volume, vol))
        return;

    m_volume = vol;

    qDebug() << "Volume:" << vol;

    // Set audio volume
    m_videoPlayer->setVolume(static_cast<float>(vol));

    emit volumeChanged(m_volume);
}

void PlayerController::updatePosition()
{
    if (!m_playing)
        return;

    // Simulate playback progress
    m_position += 0.1; // 100ms increments

    if (m_position >= m_duration)
    {
        m_position = m_duration;
        pause();
        emit endOfMedia();
    }

    emit positionChanged(m_position);
}

// Audio Effects Control

void PlayerController::enableEffect(const QString &effectName, bool enabled)
{
    if (m_videoPlayer && m_videoPlayer->getEffectChain())
    {
        m_videoPlayer->getEffectChain()->setEffectEnabled(effectName.toStdString(), enabled);
        qDebug() << "Effect" << effectName << (enabled ? "enabled" : "disabled");
    }
}

void PlayerController::setEffectParameter(const QString &effectName, const QString &paramName, double value)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    EffectChain *chain = m_videoPlayer->getEffectChain();

    // Route to specific effect setters
    if (effectName == "Reverb")
    {
        ReverbEffect *reverb = dynamic_cast<ReverbEffect *>(chain->getEffect("Reverb"));
        if (reverb)
        {
            if (paramName == "roomSize")
                reverb->setRoomSize(value);
            else if (paramName == "damping")
                reverb->setDamping(value);
            else if (paramName == "wetLevel")
                reverb->setWetLevel(value);
            else if (paramName == "width")
                reverb->setWidth(value);
            qDebug() << "✨ Reverb" << paramName << "set to" << value;
        }
    }
    else if (effectName == "Delay")
    {
        DelayEffect *delay = dynamic_cast<DelayEffect *>(chain->getEffect("Delay"));
        if (delay)
        {
            if (paramName == "delayTime")
                delay->setDelayTime(value);
            else if (paramName == "feedback")
                delay->setFeedback(value);
            else if (paramName == "wetLevel" || paramName == "mix")
                delay->setMix(value);
            qDebug() << "🔄 Delay" << paramName << "set to" << value;
        }
    }
}

// EQ Control

void PlayerController::setEQBand(int bandIndex, double gain)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    EQEffect *eq = dynamic_cast<EQEffect *>(m_videoPlayer->getEffectChain()->getEffect("Equalizer"));
    if (eq)
    {
        eq->setBandGain(bandIndex, gain);
        qDebug() << "🎚️ EQ Band" << bandIndex << "set to" << gain << "dB";
    }
}

// Reverb Control

void PlayerController::setReverbRoomSize(double size)
{
    // Scale to 0-0.7 range for more subtle reverb
    double scaledSize = size;
    setEffectParameter("Reverb", "roomSize", scaledSize);
}

void PlayerController::setReverbDamping(double damping)
{
    // Keep full range but start higher for less harshness
    double scaledDamping = 0.3 + (damping * 0.5); // 0.3 to 0.8 range
    setEffectParameter("Reverb", "damping", scaledDamping);
}

void PlayerController::setReverbWetLevel(double wet)
{
    // Scale to 0-0.4 range for subtle wet mix
    double scaledWet = wet * 0.1;
    setEffectParameter("Reverb", "wetLevel", scaledWet);
}

void PlayerController::setReverbDryLevel(double dry)
{
    // Note: ReverbEffect doesn't have separate dry control
    // Wet = 0 means all dry, wet = 1 means all wet
    qDebug() << "Reverb dry level:" << dry << "(use wet level instead)";
}

// Delay Control

void PlayerController::setDelayTime(double milliseconds)
{
    // Keep full range for delay time (useful to have long delays)
    setEffectParameter("Delay", "delayTime", milliseconds);
}

void PlayerController::setDelayFeedback(double feedback)
{
    // Scale to 0-0.6 range to prevent runaway feedback
    double scaledFeedback = feedback * 0.6;
    setEffectParameter("Delay", "feedback", scaledFeedback);
}

void PlayerController::setDelayMix(double mix)
{
    // Scale to 0-0.5 range for subtle delay mix
    double scaledMix = mix * 0.5;
    setEffectParameter("Delay", "wetLevel", scaledMix);
}

// Advanced Playback Controls

void PlayerController::skipForward(double seconds)
{
    if (m_videoPlayer)
    {
        m_videoPlayer->skipForward(seconds);
    }
}

void PlayerController::skipBackward(double seconds)
{
    if (m_videoPlayer)
    {
        m_videoPlayer->skipBackward(seconds);
    }
}

void PlayerController::setPlaybackSpeed(double speed)
{
    if (m_videoPlayer)
    {
        m_videoPlayer->setPlaybackSpeed(speed);
        emit playbackSpeedChanged(speed);
    }
}

double PlayerController::playbackSpeed() const
{
    if (m_videoPlayer)
    {
        return m_videoPlayer->getPlaybackSpeed();
    }
    return 1.0;
}

void PlayerController::setLoop(bool loop)
{
    if (m_videoPlayer)
    {
        m_videoPlayer->setLoop(loop);
        emit loopChanged(loop);
        qDebug() << "🔁 Loop" << (loop ? "enabled" : "disabled");
    }
}

bool PlayerController::isLooping() const
{
    if (m_videoPlayer)
    {
        return m_videoPlayer->isLooping();
    }
    return false;
}

// Frame Control

void PlayerController::stepForward()
{
    if (m_videoPlayer)
    {
        // Ensure we're paused
        if (m_playing)
        {
            pause();
        }
        m_videoPlayer->stepForward();
    }
}

void PlayerController::stepBackward()
{
    if (m_videoPlayer)
    {
        // Ensure we're paused
        if (m_playing)
        {
            pause();
        }
        m_videoPlayer->stepBackward();
    }
}

void PlayerController::saveScreenshot(const QString &filename)
{
    if (!m_videoPlayer)
    {
        qWarning() << "No video player available";
        emit showNotification("❌", "No video loaded");
        return;
    }

    QImage frame = m_videoPlayer->captureFrame();
    if (frame.isNull())
    {
        qWarning() << "Failed to capture frame";
        emit showNotification("❌", "Failed to capture frame");
        return;
    }

    QString filepath;
    if (filename.isEmpty())
    {
        // Generate filename with timestamp
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        filepath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/NexPlayer_" + timestamp + ".png";
    }
    else
    {
        filepath = filename;
    }

    if (frame.save(filepath))
    {
        qDebug() << "📸 Screenshot saved to:" << filepath;
        // Show success notification with shortened path
        QFileInfo fileInfo(filepath);
        QString shortPath = "~/" + fileInfo.dir().dirName() + "/" + fileInfo.fileName();
        emit showNotification("📸", "Screenshot saved!\n" + shortPath);
    }
    else
    {
        qWarning() << "❌ Failed to save screenshot to:" << filepath;
        emit showNotification("❌", "Failed to save screenshot");
    }
}

void PlayerController::initializeEffectDefaults()
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    qDebug() << "🎛️ Initializing effect defaults to match UI...";

    // Reverb defaults (match EffectsPanel.qml)
    setReverbRoomSize(0.5);  // Room Size = 0.5
    setReverbDamping(0.0);   // Damping = 0.0
    setReverbWetLevel(0.15); // Wet Level = 0.15
    setReverbDryLevel(0.7);  // Dry Level = 0.7

    // Delay defaults (match EffectsPanel.qml)
    setDelayTime(900);      // Time = 900ms
    setDelayFeedback(0.35); // Feedback = 0.35
    setDelayMix(0.35);      // Mix = 0.35

    qDebug() << "✅ Effect defaults initialized";
}

// Compressor Control

void PlayerController::setCompressorThreshold(double threshold)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    CompressorEffect *comp = dynamic_cast<CompressorEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Compressor"));
    if (comp)
    {
        comp->setThreshold(threshold);
        qDebug() << "🎚️ Compressor threshold:" << threshold << "dB";
    }
}

void PlayerController::setCompressorRatio(double ratio)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    CompressorEffect *comp = dynamic_cast<CompressorEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Compressor"));
    if (comp)
    {
        comp->setRatio(ratio);
        qDebug() << "🎚️ Compressor ratio:" << ratio;
    }
}

void PlayerController::setCompressorAttack(double attack)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    CompressorEffect *comp = dynamic_cast<CompressorEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Compressor"));
    if (comp)
    {
        comp->setAttack(attack);
        qDebug() << "🎚️ Compressor attack:" << attack << "ms";
    }
}

void PlayerController::setCompressorRelease(double release)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    CompressorEffect *comp = dynamic_cast<CompressorEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Compressor"));
    if (comp)
    {
        comp->setRelease(release);
        qDebug() << "🎚️ Compressor release:" << release << "ms";
    }
}

void PlayerController::setCompressorMakeupGain(double gain)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    CompressorEffect *comp = dynamic_cast<CompressorEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Compressor"));
    if (comp)
    {
        comp->setMakeupGain(gain);
        qDebug() << "🎚️ Compressor makeup gain:" << gain << "dB";
    }
}

// Flanger Control

void PlayerController::setFlangerDepth(double depth)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    FlangerEffect *flanger = dynamic_cast<FlangerEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Flanger"));
    if (flanger)
    {
        flanger->setDepth(depth);
        qDebug() << "🌊 Flanger depth:" << depth;
    }
}

void PlayerController::setFlangerRate(double rate)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    FlangerEffect *flanger = dynamic_cast<FlangerEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Flanger"));
    if (flanger)
    {
        flanger->setRate(rate);
        qDebug() << "🌊 Flanger rate:" << rate << "Hz";
    }
}

void PlayerController::setFlangerFeedback(double feedback)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    FlangerEffect *flanger = dynamic_cast<FlangerEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Flanger"));
    if (flanger)
    {
        flanger->setFeedback(feedback);
        qDebug() << "🌊 Flanger feedback:" << feedback;
    }
}

void PlayerController::setFlangerMix(double mix)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    FlangerEffect *flanger = dynamic_cast<FlangerEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Flanger"));
    if (flanger)
    {
        flanger->setMix(mix);
        qDebug() << "🌊 Flanger mix:" << mix;
    }
}

// Phaser Control

void PlayerController::setPhaserDepth(double depth)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    PhaserEffect *phaser = dynamic_cast<PhaserEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Phaser"));
    if (phaser)
    {
        phaser->setDepth(depth);
        qDebug() << "🔮 Phaser depth:" << depth;
    }
}

void PlayerController::setPhaserRate(double rate)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    PhaserEffect *phaser = dynamic_cast<PhaserEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Phaser"));
    if (phaser)
    {
        phaser->setRate(rate);
        qDebug() << "🔮 Phaser rate:" << rate << "Hz";
    }
}

void PlayerController::setPhaserFeedback(double feedback)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    PhaserEffect *phaser = dynamic_cast<PhaserEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Phaser"));
    if (phaser)
    {
        phaser->setFeedback(feedback);
        qDebug() << "🔮 Phaser feedback:" << feedback;
    }
}

void PlayerController::setPhaserStages(int stages)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    PhaserEffect *phaser = dynamic_cast<PhaserEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Phaser"));
    if (phaser)
    {
        phaser->setStages(stages);
        qDebug() << "🔮 Phaser stages:" << stages;
    }
}

void PlayerController::setPhaserMix(double mix)
{
    if (!m_videoPlayer || !m_videoPlayer->getEffectChain())
        return;

    PhaserEffect *phaser = dynamic_cast<PhaserEffect *>(
        m_videoPlayer->getEffectChain()->getEffect("Phaser"));
    if (phaser)
    {
        phaser->setMix(mix);
        qDebug() << "🔮 Phaser mix:" << mix;
    }
}