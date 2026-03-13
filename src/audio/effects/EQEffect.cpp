#include "EQEffect.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

EQEffect::EQEffect()
{
    // Initialize all gains to 1.0 (0dB)
    m_gains.fill(1.0f);
    
    // Calculate initial coefficients
    for (int i = 0; i < NUM_BANDS; ++i) {
        calculateCoefficients(i);
    }
}

void EQEffect::process(float *left, float *right, int numSamples)
{
    if (!m_enabled) return;
    
    // Process separate left and right channels
    for (int i = 0; i < numSamples; ++i) {
        float leftSample = left[i];
        float rightSample = right[i];
        
        // Apply all EQ bands in series
        for (int band = 0; band < NUM_BANDS; ++band) {
            leftSample = processSample(leftSample, m_coeffs[band], m_states[0][band]);
            rightSample = processSample(rightSample, m_coeffs[band], m_states[1][band]);
        }
        
        left[i] = leftSample;
        right[i] = rightSample;
    }
}

void EQEffect::reset()
{
    // Clear filter state
    for (auto &channelStates : m_states) {
        for (auto &state : channelStates) {
            state = BiquadState{};
        }
    }
}

void EQEffect::setBandGain(int band, float gainDB)
{
    if (band < 0 || band >= NUM_BANDS) return;
    
    // Clamp gain
    gainDB = std::clamp(gainDB, -12.0f, 12.0f);
    
    // Convert dB to linear
    m_gains[band] = std::pow(10.0f, gainDB / 20.0f);
    
    // Recalculate coefficients for this band
    calculateCoefficients(band);
}

float EQEffect::getBandGain(int band) const
{
    if (band < 0 || band >= NUM_BANDS) return 0.0f;
    
    // Convert linear to dB
    return 20.0f * std::log10(m_gains[band]);
}

float EQEffect::getBandFrequency(int band) const
{
    if (band < 0 || band >= NUM_BANDS) return 0.0f;
    return BAND_FREQUENCIES[band];
}

void EQEffect::resetBands()
{
    for (int i = 0; i < NUM_BANDS; ++i) {
        setBandGain(i, 0.0f);
    }
}

void EQEffect::calculateCoefficients(int band)
{
    // Simple peaking EQ filter (bell filter)
    float freq = BAND_FREQUENCIES[band];
    float gain = m_gains[band];
    float Q = 1.0f;  // Bandwidth
    
    float w0 = 2.0f * M_PI * freq / m_sampleRate;
    float cosw0 = std::cos(w0);
    float sinw0 = std::sin(w0);
    float alpha = sinw0 / (2.0f * Q);
    float A = gain;
    
    // Peaking filter coefficients
    float b0 = 1.0f + alpha * A;
    float b1 = -2.0f * cosw0;
    float b2 = 1.0f - alpha * A;
    float a0 = 1.0f + alpha / A;
    float a1 = -2.0f * cosw0;
    float a2 = 1.0f - alpha / A;
    
    // Normalize by a0
    m_coeffs[band].b0 = b0 / a0;
    m_coeffs[band].b1 = b1 / a0;
    m_coeffs[band].b2 = b2 / a0;
    m_coeffs[band].a1 = a1 / a0;
    m_coeffs[band].a2 = a2 / a0;
}

float EQEffect::processSample(float input, const BiquadCoeffs &coeffs, BiquadState &state)
{
    // Direct Form II Transposed biquad filter
    float output = coeffs.b0 * input + state.x1;
    state.x1 = coeffs.b1 * input - coeffs.a1 * output + state.x2;
    state.x2 = coeffs.b2 * input - coeffs.a2 * output;
    
    return output;
}