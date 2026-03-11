#include "ReverbEffect.h"
#include <cmath>

// Comb filter delays (samples at 44.1kHz)
static const int combTunings[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};

// Allpass filter delays (samples at 44.1kHz)
static const int allpassTunings[4] = {556, 441, 341, 225};

ReverbEffect::ReverbEffect()
{
    // Initialize comb filters
    for (int i = 0; i < NUM_COMBS; ++i) {
        m_combsL[i].setBuffer(combTunings[i]);
        m_combsR[i].setBuffer(combTunings[i] + (i % 2) * 23);  // Slightly different for stereo
    }
    
    // Initialize allpass filters
    for (int i = 0; i < NUM_ALLPASS; ++i) {
        m_allpassL[i].setBuffer(allpassTunings[i]);
        m_allpassR[i].setBuffer(allpassTunings[i] + (i % 2) * 23);
    }
    
    updateParameters();
}

void ReverbEffect::process(float *left, float *right, int numSamples)
{
    if (!m_enabled) return;
    
    for (int i = 0; i < numSamples; ++i) {
        float inputL = left[i];
        float inputR = right[i];
        
        // Mix to mono for reverb input
        float input = (inputL + inputR) * 0.5f;
        
        // Process through comb filters
        float outL = 0.0f;
        float outR = 0.0f;
        
        for (int c = 0; c < NUM_COMBS; ++c) {
            outL += m_combsL[c].process(input);
            outR += m_combsR[c].process(input);
        }
        
        // Process through allpass filters
        for (int a = 0; a < NUM_ALLPASS; ++a) {
            outL = m_allpassL[a].process(outL);
            outR = m_allpassR[a].process(outR);
        }
        
        // Apply wet/dry mix and stereo width
        float wet1 = outL * m_wet1 + outR * m_wet2;
        float wet2 = outR * m_wet1 + outL * m_wet2;
        
        left[i] = inputL * m_dry + wet1;
        right[i] = inputR * m_dry + wet2;
    }
}

void ReverbEffect::reset()
{
    for (auto &comb : m_combsL) {
        std::fill(comb.buffer.begin(), comb.buffer.end(), 0.0f);
        comb.bufferIndex = 0;
        comb.filterStore = 0.0f;
    }
    
    for (auto &comb : m_combsR) {
        std::fill(comb.buffer.begin(), comb.buffer.end(), 0.0f);
        comb.bufferIndex = 0;
        comb.filterStore = 0.0f;
    }
    
    for (auto &allpass : m_allpassL) {
        std::fill(allpass.buffer.begin(), allpass.buffer.end(), 0.0f);
        allpass.bufferIndex = 0;
    }
    
    for (auto &allpass : m_allpassR) {
        std::fill(allpass.buffer.begin(), allpass.buffer.end(), 0.0f);
        allpass.bufferIndex = 0;
    }
}

void ReverbEffect::setRoomSize(float size)
{
    m_roomSize = std::clamp(size, 0.0f, 1.0f);
    updateParameters();
}

void ReverbEffect::setDamping(float damping)
{
    m_damping = std::clamp(damping, 0.0f, 1.0f);
    updateParameters();
}

void ReverbEffect::setWetLevel(float wet)
{
    m_wetLevel = std::clamp(wet, 0.0f, 1.0f);
    updateParameters();
}

void ReverbEffect::setDryLevel(float dry)
{
    m_dryLevel = std::clamp(dry, 0.0f, 1.0f);
    updateParameters();
}

void ReverbEffect::setWidth(float width)
{
    m_width = std::clamp(width, 0.0f, 1.0f);
    updateParameters();
}

void ReverbEffect::updateParameters()
{
    // Calculate wet levels for stereo
    m_wet1 = m_wetLevel * (m_width * 0.5f + 0.5f);
    m_wet2 = m_wetLevel * ((1.0f - m_width) * 0.5f);
    
    m_dry = m_dryLevel * SCALE_DRY;
    
    // Update comb filter parameters
    float roomSize = m_roomSize * SCALE_ROOM + OFFSET_ROOM;
    float damp = m_damping * SCALE_DAMP;
    float damp1 = damp;
    float damp2 = 1.0f - damp;
    
    for (auto &comb : m_combsL) {
        comb.feedback = roomSize;
        comb.damp1 = damp1;
        comb.damp2 = damp2;
    }
    
    for (auto &comb : m_combsR) {
        comb.feedback = roomSize;
        comb.damp1 = damp1;
        comb.damp2 = damp2;
    }
}

// Comb filter implementation
void ReverbEffect::CombFilter::setBuffer(int size)
{
    buffer.resize(size, 0.0f);
    bufferIndex = 0;
}

float ReverbEffect::CombFilter::process(float input)
{
    float output = buffer[bufferIndex];
    
    filterStore = (output * damp2) + (filterStore * damp1);
    
    buffer[bufferIndex] = input + (filterStore * feedback);
    
    bufferIndex++;
    if (bufferIndex >= static_cast<int>(buffer.size())) {
        bufferIndex = 0;
    }
    
    return output;
}

// Allpass filter implementation
void ReverbEffect::AllpassFilter::setBuffer(int size)
{
    buffer.resize(size, 0.0f);
    bufferIndex = 0;
}

float ReverbEffect::AllpassFilter::process(float input)
{
    float bufout = buffer[bufferIndex];
    float output = -input + bufout;
    
    buffer[bufferIndex] = input + (bufout * 0.5f);
    
    bufferIndex++;
    if (bufferIndex >= static_cast<int>(buffer.size())) {
        bufferIndex = 0;
    }
    
    return output;
}