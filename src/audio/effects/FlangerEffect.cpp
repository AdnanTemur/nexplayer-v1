#include "FlangerEffect.h"
#include <cmath>

FlangerEffect::FlangerEffect()
{
    m_delayBufferLeft.resize(MAX_DELAY_SAMPLES, 0.0f);
    m_delayBufferRight.resize(MAX_DELAY_SAMPLES, 0.0f);
}

void FlangerEffect::process(float *left, float *right, int numSamples)
{
    if (!m_enabled) return;
    
    for (int i = 0; i < numSamples; ++i) {
        // Generate LFO (sine wave)
        float lfo = sinf(2.0f * M_PI * m_lfoPhase);
        m_lfoPhase += m_rate / m_sampleRate;
        if (m_lfoPhase >= 1.0f) m_lfoPhase -= 1.0f;
        
        // Calculate delay time (1-10 ms modulated by LFO)
        float baseDelay = 2.0f; // ms
        float delayModulation = 5.0f; // ms
        float delayMs = baseDelay + (lfo * delayModulation * m_depth);
        float delaySamples = (delayMs / 1000.0f) * m_sampleRate;
        
        // Calculate read position with linear interpolation
        float readPos = m_writePos - delaySamples;
        if (readPos < 0) readPos += MAX_DELAY_SAMPLES;
        
        int readPos1 = (int)readPos;
        int readPos2 = (readPos1 + 1) % MAX_DELAY_SAMPLES;
        float frac = readPos - readPos1;
        
        // Read delayed samples with interpolation
        float delayedLeft = m_delayBufferLeft[readPos1] * (1.0f - frac) + 
                           m_delayBufferLeft[readPos2] * frac;
        float delayedRight = m_delayBufferRight[readPos1] * (1.0f - frac) + 
                            m_delayBufferRight[readPos2] * frac;
        
        // Mix dry and wet signals
        float outputLeft = left[i] * (1.0f - m_mix) + delayedLeft * m_mix;
        float outputRight = right[i] * (1.0f - m_mix) + delayedRight * m_mix;
        
        // Write to delay buffer with feedback
        m_delayBufferLeft[m_writePos] = left[i] + delayedLeft * m_feedback;
        m_delayBufferRight[m_writePos] = right[i] + delayedRight * m_feedback;
        
        left[i] = outputLeft;
        right[i] = outputRight;
        
        // Advance write position
        m_writePos = (m_writePos + 1) % MAX_DELAY_SAMPLES;
    }
}

void FlangerEffect::reset()
{
    // Reset LFO
    m_lfoPhase = 0.0f;

    // Reset write position
    m_writePos = 0;

    // Clear delay buffers
    std::fill(m_delayBufferLeft.begin(),  m_delayBufferLeft.end(),  0.0f);
    std::fill(m_delayBufferRight.begin(), m_delayBufferRight.end(), 0.0f);
}

