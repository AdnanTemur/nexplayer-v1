#pragma once

#include "AudioEffect.h"
#include <vector>

/**
 * @brief Stereo delay/echo effect
 * 
 * Simple delay line with feedback and dry/wet mix.
 * Can create echo, slapback, and other delay-based effects.
 */
class DelayEffect : public AudioEffect
{
public:
    DelayEffect();
    ~DelayEffect() override = default;
    
    void process(float *left, float *right, int numSamples) override;
    void reset() override;
    std::string getName() const override { return "Delay"; }
    
    /**
     * @brief Set delay time in milliseconds
     * @param ms Delay time (1 to 2000 ms)
     */
    void setDelayTime(float ms);
    
    /**
     * @brief Set feedback amount
     * @param feedback Feedback level (0.0 to 0.95)
     */
    void setFeedback(float feedback);
    
    /**
     * @brief Set dry/wet mix
     * @param mix Mix level (0.0 = dry only, 1.0 = wet only)
     */
    void setMix(float mix);
    
    float getDelayTime() const { return m_delayTimeMs; }
    float getFeedback() const { return m_feedback; }
    float getMix() const { return m_mix; }

private:
    void updateDelayBuffer();
    
    // Delay buffers (one per channel)
    std::vector<float> m_bufferL;
    std::vector<float> m_bufferR;
    
    int m_writeIndex = 0;
    int m_readIndex = 0;
    
    // Parameters
    float m_delayTimeMs = 250.0f;
    float m_feedback = 0.3f;
    float m_mix = 0.5f;
    
    // Calculated values
    int m_delaySamples = 0;
    
    // Sample rate
    float m_sampleRate = 44100.0f;
    
    // Max delay time (2 seconds)
    static constexpr int MAX_DELAY_SAMPLES = 88200;  // 2 seconds at 44.1kHz
};
