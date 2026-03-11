#include "PhaserEffect.h"
#include <cmath>
#include <algorithm>

PhaserEffect::PhaserEffect()
{
}

void PhaserEffect::process(float *left, float *right, int numSamples)
{
    if (!m_enabled)
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        // Generate LFO
        float lfo = (sinf(2.0f * M_PI * m_lfoPhase) + 1.0f) * 0.5f; // 0 to 1
        m_lfoPhase += m_rate / m_sampleRate;
        if (m_lfoPhase >= 1.0f)
            m_lfoPhase -= 1.0f;

        // Calculate allpass coefficient based on LFO
        float minFreq = 200.0f;
        float maxFreq = 2000.0f;
        float freq = minFreq + (maxFreq - minFreq) * lfo * m_depth;
        float coefficient = (tanf(M_PI * freq / m_sampleRate) - 1.0f) /
                            (tanf(M_PI * freq / m_sampleRate) + 1.0f);

        // Process through allpass filter cascade
        float outputLeft = left[i];
        float outputRight = right[i];

        for (int stage = 0; stage < m_stages; ++stage)
        {
            outputLeft = m_filtersLeft[stage].process(outputLeft, coefficient);
            outputRight = m_filtersRight[stage].process(outputRight, coefficient);
        }

        // Add feedback
        outputLeft += left[i] * m_feedback;
        outputRight += right[i] * m_feedback;

        // Mix dry and wet
        left[i] = left[i] * (1.0f - m_mix) + outputLeft * m_mix;
        right[i] = right[i] * (1.0f - m_mix) + outputRight * m_mix;
    }
}

void PhaserEffect::reset()
{
    // Reset LFO phase
    m_lfoPhase = 0.0f;

    // Reset all-pass filter memory
    for (int i = 0; i < 12; i++)
    {
        m_filtersLeft[i].zm1 = 0.0f;
        m_filtersRight[i].zm1 = 0.0f;
    }
}
