#include "CompressorEffect.h"
#include <cmath>
#include <algorithm>

CompressorEffect::CompressorEffect()
{
}

void CompressorEffect::process(float *left, float *right, int numSamples)
{
    if (!m_enabled)
        return;

    // Convert attack/release times to coefficients
    float attackCoeff = exp(-1.0f / (m_attack * m_sampleRate / 1000.0f));
    float releaseCoeff = exp(-1.0f / (m_release * m_sampleRate / 1000.0f));

    // Convert makeup gain from dB to linear
    float makeupGainLinear = pow(10.0f, m_makeupGain / 20.0f);

    for (int i = 0; i < numSamples; ++i)
    {
        // Get peak level of stereo signal
        float inputLevel = std::max(std::abs(left[i]), std::abs(right[i]));

        // Convert to dB
        float inputLevelDb = 20.0f * log10f(inputLevel + 1e-10f);

        // Calculate gain reduction
        float gainReduction = 0.0f;
        if (inputLevelDb > m_threshold)
        {
            // Above threshold - compress
            float excess = inputLevelDb - m_threshold;
            gainReduction = excess * (1.0f - 1.0f / m_ratio);
        }

        // Smooth gain reduction with envelope follower
        if (gainReduction > m_envelope)
        {
            // Attack
            m_envelope = attackCoeff * m_envelope + (1.0f - attackCoeff) * gainReduction;
        }
        else
        {
            // Release
            m_envelope = releaseCoeff * m_envelope + (1.0f - releaseCoeff) * gainReduction;
        }

        // Convert gain reduction back to linear
        float gainLinear = pow(10.0f, -m_envelope / 20.0f);

        // Apply compression and makeup gain
        left[i] = left[i] * gainLinear * makeupGainLinear;
        right[i] = right[i] * gainLinear * makeupGainLinear;
    }
}

float CompressorEffect::calculateGainReduction(float inputLevel)
{
    float inputLevelDb = 20.0f * log10f(inputLevel + 1e-10f);

    if (inputLevelDb > m_threshold)
    {
        float excess = inputLevelDb - m_threshold;
        return excess * (1.0f - 1.0f / m_ratio);
    }

    return 0.0f;
}

void CompressorEffect::reset()
{
    // Reset envelope follower
    m_envelope = 0.0f;
}
