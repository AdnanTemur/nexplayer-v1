#pragma once

#include "AudioEffect.h"
#include <vector>
#include <array>

/**
 * @brief Reverb effect (room/hall simulation)
 * 
 * Algorithmic reverb based on Freeverb algorithm.
 * Simulates room acoustics with adjustable parameters.
 */
class ReverbEffect : public AudioEffect
{
public:
    ReverbEffect();
    ~ReverbEffect() override = default;
    
    void process(float *left, float *right, int numSamples) override;
    void reset() override;
    std::string getName() const override { return "Reverb"; }
    
    // Parameters (0.0 to 1.0 range)
    void setRoomSize(float size);
    void setDamping(float damping);
    void setWetLevel(float wet);
    void setDryLevel(float dry);
    void setWidth(float width);  // Stereo width
    
    float getRoomSize() const { return m_roomSize; }
    float getDamping() const { return m_damping; }
    float getWetLevel() const { return m_wetLevel; }
    float getDryLevel() const { return m_dryLevel; }
    float getWidth() const { return m_width; }

private:
    // Comb filter
    struct CombFilter {
        std::vector<float> buffer;
        int bufferIndex = 0;
        float filterStore = 0.0f;
        float feedback = 0.0f;
        float damp1 = 0.0f;
        float damp2 = 0.0f;
        
        void setBuffer(int size);
        float process(float input);
    };
    
    // Allpass filter
    struct AllpassFilter {
        std::vector<float> buffer;
        int bufferIndex = 0;
        
        void setBuffer(int size);
        float process(float input);
    };
    
    void updateParameters();
    
    // Comb filters (8 per channel)
    static constexpr int NUM_COMBS = 8;
    std::array<CombFilter, NUM_COMBS> m_combsL;
    std::array<CombFilter, NUM_COMBS> m_combsR;
    
    // Allpass filters (4 per channel)
    static constexpr int NUM_ALLPASS = 4;
    std::array<AllpassFilter, NUM_ALLPASS> m_allpassL;
    std::array<AllpassFilter, NUM_ALLPASS> m_allpassR;
    
    // Parameters
    float m_roomSize = 0.5f;
    float m_damping = 0.5f;
    float m_wetLevel = 0.3f;
    float m_dryLevel = 0.7f;
    float m_width = 1.0f;
    
    // Calculated values
    float m_wet1 = 0.0f;
    float m_wet2 = 0.0f;
    float m_dry = 0.0f;
    
    static constexpr float SCALE_WET = 3.0f;
    static constexpr float SCALE_DRY = 2.0f;
    static constexpr float SCALE_DAMP = 0.4f;
    static constexpr float SCALE_ROOM = 0.28f;
    static constexpr float OFFSET_ROOM = 0.7f;
};
