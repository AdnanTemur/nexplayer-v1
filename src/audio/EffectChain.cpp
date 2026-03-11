#include "EffectChain.h"
#include "effects/AudioEffect.h"
#include "effects/EQEffect.h"
#include "effects/ReverbEffect.h"
#include "effects/DelayEffect.h"
#include "effects/CompressorEffect.h"
#include "effects/FlangerEffect.h"
#include "effects/PhaserEffect.h"
#include <algorithm>
#include <iostream>
#include <QDebug>

EffectChain::EffectChain()
{
    // Create effects with default sample rate
    setSampleRate(44100.0);
    std::cout << "EffectChain created" << std::endl;
}

EffectChain::~EffectChain()
{
    clear();
}

void EffectChain::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;

    // Clear existing effects
    m_effects.clear();

    // Create effects in processing order

    // 1. EQ (first - shape frequency response)
    auto eq = std::make_unique<EQEffect>();
    eq->setEnabled(true);
    m_effects.push_back(std::move(eq));

    // 2. Compressor (dynamic control)
    auto compressor = std::make_unique<CompressorEffect>();
    compressor->setEnabled(false); // Disabled by default
    m_effects.push_back(std::move(compressor));

    // 3. Flanger (modulation effect)
    auto flanger = std::make_unique<FlangerEffect>();
    flanger->setEnabled(false);
    m_effects.push_back(std::move(flanger));

    // 4. Phaser (modulation effect)
    auto phaser = std::make_unique<PhaserEffect>();
    phaser->setEnabled(false);
    m_effects.push_back(std::move(phaser));

    // 5. Reverb (spatial effect)
    auto reverb = std::make_unique<ReverbEffect>();
    reverb->setEnabled(false);
    m_effects.push_back(std::move(reverb));

    // 6. Delay (last - time-based effect)
    auto delay = std::make_unique<DelayEffect>();
    delay->setEnabled(false);
    m_effects.push_back(std::move(delay));

    qDebug() << "EffectChain initialized with 6 effects at" << sampleRate << "Hz";
}

void EffectChain::process(float *buffer, int numSamples)
{
    // This processes interleaved stereo: LRLRLR...
    // We need to deinterleave, process, then reinterleave

    int totalSamples = numSamples * 2; // stereo

    // Allocate temp buffers
    std::vector<float> left(numSamples);
    std::vector<float> right(numSamples);

    // Deinterleave
    for (int i = 0; i < numSamples; ++i)
    {
        left[i] = buffer[i * 2];
        right[i] = buffer[i * 2 + 1];
    }

    // Process through each enabled effect
    for (auto &effect : m_effects)
    {
        if (effect && effect->isEnabled())
        {
            effect->process(left.data(), right.data(), numSamples);
        }
    }

    // Reinterleave
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i * 2] = left[i];
        buffer[i * 2 + 1] = right[i];
    }
}

void EffectChain::addEffect(std::unique_ptr<AudioEffect> effect)
{
    if (effect)
    {
        m_effects.push_back(std::move(effect));
    }
}

bool EffectChain::removeEffect(const std::string &name)
{
    auto it = std::find_if(m_effects.begin(), m_effects.end(),
                           [&name](const std::unique_ptr<AudioEffect> &effect)
                           {
                               return effect && effect->getName() == name;
                           });

    if (it != m_effects.end())
    {
        m_effects.erase(it);
        return true;
    }

    return false;
}

AudioEffect *EffectChain::getEffect(const std::string &name)
{
    auto it = std::find_if(m_effects.begin(), m_effects.end(),
                           [&name](const std::unique_ptr<AudioEffect> &effect)
                           {
                               return effect && effect->getName() == name;
                           });

    if (it != m_effects.end())
    {
        return it->get();
    }

    return nullptr;
}

void EffectChain::setEffectEnabled(const std::string &name, bool enabled)
{
    AudioEffect *effect = getEffect(name);
    if (effect)
    {
        effect->setEnabled(enabled);
        std::cout << "Effect \"" << name << "\" " << (enabled ? "enabled" : "disabled") << std::endl;
    }
}

void EffectChain::clear()
{
    m_effects.clear();
}

void EffectChain::reset()
{
    for (auto &effect : m_effects)
    {
        if (effect)
        {
            effect->reset();
        }
    }
}