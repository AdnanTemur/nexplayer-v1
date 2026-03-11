#pragma once

#include <memory>

class EffectChain;

/**
 * @brief Main audio engine
 * 
 * Manages audio playback and processing.
 * In production, this would interface with JUCE or PortAudio.
 * For now, this is a stub that will be expanded.
 */
class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();
    
    /**
     * @brief Start audio playback
     */
    void start();
    
    /**
     * @brief Stop audio playback
     */
    void stop();
    
    /**
     * @brief Pause audio playback
     */
    void pause();
    
    /**
     * @brief Set master volume
     * @param volume Volume level (0.0 to 1.0)
     */
    void setVolume(float volume);
    
    /**
     * @brief Get current volume
     */
    float getVolume() const { return m_volume; }
    
    /**
     * @brief Set effect chain for processing
     */
    void setEffectChain(EffectChain *chain);
    
    /**
     * @brief Check if audio is playing
     */
    bool isPlaying() const { return m_playing; }

private:
    // Audio callback (called by audio thread)
    void audioCallback(float *outputBuffer, int numSamples);
    
    EffectChain *m_effectChain = nullptr;
    
    float m_volume = 1.0f;
    bool m_playing = false;
    
    // TODO: Add JUCE AudioDeviceManager or PortAudio stream
};
