#pragma once

#include <string>

/**
 * @brief Base class for all audio effects
 * 
 * Defines the interface that all audio effects must implement.
 */
class AudioEffect
{
public:
    virtual ~AudioEffect() = default;
    
    /**
     * @brief Process audio buffer in-place
     * @param buffer Audio buffer (interleaved stereo)
     * @param numSamples Number of samples per channel
     */
    virtual void process(float *left, float *right, int numSamples) = 0;
    
    /**
     * @brief Reset effect state
     */
    virtual void reset() = 0;
    
    /**
     * @brief Set whether effect is bypassed
     */
    virtual void setEnabled(bool enabled) { m_enabled = enabled; }
    
    /**
     * @brief Check if effect is enabled
     */
    virtual bool isEnabled() const { return m_enabled; }
    
    /**
     * @brief Get effect name
     */
    virtual std::string getName() const = 0;

protected:
    bool m_enabled = true;
};
