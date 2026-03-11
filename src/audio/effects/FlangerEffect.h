#pragma once

#include "AudioEffect.h"
#include <vector>

/**
 * @brief Flanger effect
 * Creates a sweeping, jet-plane like sound
 */
class FlangerEffect : public AudioEffect
{
public:
    FlangerEffect();
    std::string getName() const override { return "Flanger"; } 
    void process(float *left, float *right, int numSamples) override;
    void reset() override;
    
    // Parameters
    void setDepth(float depth) { m_depth = depth; }           // 0.0 to 1.0
    void setRate(float rate) { m_rate = rate; }               // 0.1 to 10 Hz
    void setFeedback(float feedback) { m_feedback = feedback; } // 0.0 to 0.9
    void setMix(float mix) { m_mix = mix; }                   // 0.0 to 1.0
    
private:
    float m_depth = 0.7f;
    float m_rate = 0.5f;        // Hz
    float m_feedback = 0.5f;
    float m_mix = 0.5f;
    
    // Delay line
    std::vector<float> m_delayBufferLeft;
    std::vector<float> m_delayBufferRight;
    int m_writePos = 0;
    
    // LFO
    float m_lfoPhase = 0.0f;
    float m_sampleRate = 44100.0f;
    
    static constexpr int MAX_DELAY_SAMPLES = 2205; // 50ms at 44.1kHz
};