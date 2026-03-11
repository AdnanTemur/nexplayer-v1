#pragma once

#include "AudioEffect.h"

/**
 * @brief Phaser effect
 * Creates a sweeping notch filter effect
 */
class PhaserEffect : public AudioEffect
{
public:
    PhaserEffect();
    std::string getName() const override { return "Phaser"; }  
    void process(float *left, float *right, int numSamples) override;
    void reset() override;
    
    // Parameters
    void setDepth(float depth) { m_depth = depth; }     // 0.0 to 1.0
    void setRate(float rate) { m_rate = rate; }         // 0.1 to 5 Hz
    void setFeedback(float fb) { m_feedback = fb; }     // 0.0 to 0.9
    void setStages(int stages) { m_stages = stages; }   // 2 to 12
    void setMix(float mix) { m_mix = mix; }             // 0.0 to 1.0
    
private:
    struct AllPassFilter {
        float a1 = 0.0f;
        float zm1 = 0.0f;
        
        float process(float input, float coefficient) {
            float output = -input + coefficient * (input - a1 * zm1) + zm1;
            zm1 = output;
            return output;
        }
    };
    
    float m_depth = 0.7f;
    float m_rate = 0.5f;
    float m_feedback = 0.7f;
    int m_stages = 6;
    float m_mix = 0.5f;
    
    AllPassFilter m_filtersLeft[12];
    AllPassFilter m_filtersRight[12];
    
    float m_lfoPhase = 0.0f;
    float m_sampleRate = 44100.0f;
};