#include "DelayEffect.h"
#include <algorithm>
#include <cmath>

DelayEffect::DelayEffect()
{
    // Initialize delay buffers
    m_bufferL.resize(MAX_DELAY_SAMPLES, 0.0f);
    m_bufferR.resize(MAX_DELAY_SAMPLES, 0.0f);
    
    updateDelayBuffer();
}

void DelayEffect::process(float *left, float *right, int numSamples)
{
    if (!m_enabled) return;
    
    for (int i = 0; i < numSamples; ++i) {
        float inputL = left[i];
        float inputR = right[i];
        
        // Read delayed samples
        float delayedL = m_bufferL[m_readIndex];
        float delayedR = m_bufferR[m_readIndex];
        
        // Write to delay buffer (input + feedback)
        m_bufferL[m_writeIndex] = inputL + (delayedL * m_feedback);
        m_bufferR[m_writeIndex] = inputR + (delayedR * m_feedback);
        
        // Mix dry and wet signals
        float outputL = inputL * (1.0f - m_mix) + delayedL * m_mix;
        float outputR = inputR * (1.0f - m_mix) + delayedR * m_mix;
        
        left[i] = outputL;
        right[i] = outputR;
        
        // Advance read/write indices
        m_writeIndex = (m_writeIndex + 1) % MAX_DELAY_SAMPLES;
        m_readIndex = (m_readIndex + 1) % MAX_DELAY_SAMPLES;
    }
}

void DelayEffect::reset()
{
    std::fill(m_bufferL.begin(), m_bufferL.end(), 0.0f);
    std::fill(m_bufferR.begin(), m_bufferR.end(), 0.0f);
    m_writeIndex = 0;
    m_readIndex = 0;
}

void DelayEffect::setDelayTime(float ms)
{
    m_delayTimeMs = std::clamp(ms, 1.0f, 2000.0f);
    updateDelayBuffer();
}

void DelayEffect::setFeedback(float feedback)
{
    m_feedback = std::clamp(feedback, 0.0f, 0.95f);
}

void DelayEffect::setMix(float mix)
{
    m_mix = std::clamp(mix, 0.0f, 1.0f);
}

void DelayEffect::updateDelayBuffer()
{
    // Calculate delay in samples
    m_delaySamples = static_cast<int>(m_delayTimeMs * m_sampleRate / 1000.0f);
    m_delaySamples = std::clamp(m_delaySamples, 1, MAX_DELAY_SAMPLES - 1);
    
    // Update read index to maintain delay time
    m_readIndex = (m_writeIndex - m_delaySamples + MAX_DELAY_SAMPLES) % MAX_DELAY_SAMPLES;
}