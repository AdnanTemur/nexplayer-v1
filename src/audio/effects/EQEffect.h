#pragma once

#include "AudioEffect.h"
#include <vector>
#include <array>

/**
 * @brief Parametric equalizer effect
 * 
 * 10-band parametric EQ with adjustable gain per band.
 * Frequency bands: 32Hz, 64Hz, 125Hz, 250Hz, 500Hz, 1kHz, 2kHz, 4kHz, 8kHz, 16kHz
 */
class EQEffect : public AudioEffect
{
public:
    static constexpr int NUM_BANDS = 10;
    
    EQEffect();
    ~EQEffect() override = default;
    
    void process(float *left, float *right, int numSamples) override;
    void reset() override;
    std::string getName() const override { return "Equalizer"; }
    
    /**
     * @brief Set gain for a specific band
     * @param band Band index (0-9)
     * @param gainDB Gain in decibels (-12 to +12)
     */
    void setBandGain(int band, float gainDB);
    
    /**
     * @brief Get current gain for a band
     */
    float getBandGain(int band) const;
    
    /**
     * @brief Get frequency for a band
     */
    float getBandFrequency(int band) const;
    
    /**
     * @brief Reset all bands to 0dB (flat response)
     */
    void resetBands();

private:
    // Biquad filter coefficients
    struct BiquadCoeffs {
        float b0, b1, b2;  // Numerator coefficients
        float a1, a2;       // Denominator coefficients
    };
    
    // Biquad filter state (per channel)
    struct BiquadState {
        float x1 = 0.0f, x2 = 0.0f;  // Input history
        float y1 = 0.0f, y2 = 0.0f;  // Output history
    };
    
    void calculateCoefficients(int band);
    float processSample(float input, const BiquadCoeffs &coeffs, BiquadState &state);
    
    // Center frequencies for each band (Hz)
    static constexpr std::array<float, NUM_BANDS> BAND_FREQUENCIES = {
        32.0f, 64.0f, 125.0f, 250.0f, 500.0f, 
        1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f
    };
    
    // Gain per band (linear, not dB)
    std::array<float, NUM_BANDS> m_gains;
    
    // Filter coefficients per band
    std::array<BiquadCoeffs, NUM_BANDS> m_coeffs;
    
    // Filter state per band per channel (stereo)
    std::array<std::array<BiquadState, NUM_BANDS>, 2> m_states;
    
    // Sample rate
    float m_sampleRate = 44100.0f;
};
