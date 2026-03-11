#pragma once

#include "AudioEffect.h"

/**
 * @brief Dynamic range compressor
 * Reduces the difference between loud and quiet parts
 */
class CompressorEffect : public AudioEffect
{
public:
    CompressorEffect();
    std::string getName() const override { return "Compressor"; } 
    void process(float *left, float *right, int numSamples) override;
    void reset() override;
    
    // Parameters
    void setThreshold(float threshold) { m_threshold = threshold; }      // -60 to 0 dB
    void setRatio(float ratio) { m_ratio = ratio; }                      // 1 to 20
    void setAttack(float attack) { m_attack = attack; }                  // 0.1 to 100 ms
    void setRelease(float release) { m_release = release; }              // 10 to 1000 ms
    void setMakeupGain(float gain) { m_makeupGain = gain; }             // 0 to 20 dB
    
    float getThreshold() const { return m_threshold; }
    float getRatio() const { return m_ratio; }
    float getAttack() const { return m_attack; }
    float getRelease() const { return m_release; }
    float getMakeupGain() const { return m_makeupGain; }
    
private:
    float calculateGainReduction(float inputLevel);
    
    float m_threshold = -20.0f;   // dB
    float m_ratio = 4.0f;         // ratio
    float m_attack = 5.0f;        // ms
    float m_release = 50.0f;      // ms
    float m_makeupGain = 0.0f;    // dB
    
    float m_envelope = 0.0f;      // Current envelope follower value
    float m_sampleRate = 44100.0f;
};