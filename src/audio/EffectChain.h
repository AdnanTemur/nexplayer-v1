#pragma once

#include <vector>
#include <memory>
#include <string>

class AudioEffect;

/**
 * @brief Manages a chain of audio effects
 * 
 * Processes audio through multiple effects in series.
 * Effects can be added, removed, reordered, and bypassed.
 */
class EffectChain
{
public:
    EffectChain();
    ~EffectChain();
    
    /**
     * @brief Set sample rate and recreate effects
     * @param sampleRate Sample rate in Hz
     */
    void setSampleRate(double sampleRate);
    
    /**
     * @brief Process audio buffer through all active effects
     * @param buffer Interleaved stereo audio buffer
     * @param numSamples Number of samples per channel
     */
    void process(float *buffer, int numSamples);
    
    /**
     * @brief Add an effect to the chain
     * @param effect Owned pointer to effect
     */
    void addEffect(std::unique_ptr<AudioEffect> effect);
    
    /**
     * @brief Remove an effect by name
     * @param name Effect name
     * @return true if removed, false if not found
     */
    bool removeEffect(const std::string &name);
    
    /**
     * @brief Get effect by name
     * @param name Effect name
     * @return Pointer to effect or nullptr if not found
     */
    AudioEffect* getEffect(const std::string &name);
    
    /**
     * @brief Enable/disable an effect
     * @param name Effect name
     * @param enabled Whether to enable the effect
     */
    void setEffectEnabled(const std::string &name, bool enabled);
    
    /**
     * @brief Clear all effects
     */
    void clear();
    
    /**
     * @brief Reset all effects (clear buffers/state)
     */
    void reset();
    
    /**
     * @brief Get number of effects in chain
     */
    size_t size() const { return m_effects.size(); }

private:
    std::vector<std::unique_ptr<AudioEffect>> m_effects;
    double m_sampleRate = 44100.0;
};