#include "AudioEngine.h"
#include "EffectChain.h"
#include <algorithm>  
#include <iostream>

AudioEngine::AudioEngine()
{
    std::cout << "AudioEngine initialized" << std::endl;

    // TODO: Initialize JUCE AudioDeviceManager
    // or PortAudio stream
}

AudioEngine::~AudioEngine()
{
    stop();
}

void AudioEngine::start()
{
    if (m_playing)
        return;

    m_playing = true;
    std::cout << "Audio playback started" << std::endl;

    // TODO: Start audio device
}

void AudioEngine::stop()
{
    if (!m_playing)
        return;

    m_playing = false;
    std::cout << "Audio playback stopped" << std::endl;

    // TODO: Stop audio device
}

void AudioEngine::pause()
{
    if (!m_playing)
        return;

    m_playing = false;
    std::cout << "Audio playback paused" << std::endl;

    // TODO: Pause audio device
}

void AudioEngine::setVolume(float volume)
{
    m_volume = std::clamp(volume, 0.0f, 1.0f);
    std::cout << "Volume set to: " << m_volume << std::endl;
}

void AudioEngine::setEffectChain(EffectChain *chain)
{
    m_effectChain = chain;
}

void AudioEngine::audioCallback(float *outputBuffer, int numSamples)
{
    // This will be called by the audio thread in production

    // TODO: Get audio from decoder
    // For now, generate silence
    for (int i = 0; i < numSamples * 2; ++i)
    {
        outputBuffer[i] = 0.0f;
    }

    // Apply effects
    if (m_effectChain)
    {
        m_effectChain->process(outputBuffer, numSamples);
    }

    // Apply master volume
    for (int i = 0; i < numSamples * 2; ++i)
    {
        outputBuffer[i] *= m_volume;
    }
}
