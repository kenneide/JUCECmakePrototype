#pragma once

#include <memory>

#include "LanczosResampler.h"

class LoudnessEstimator
{
public:
    LoudnessEstimator(float samplerate);
    ~LoudnessEstimator() = default;

    float getLoudness() const;

    void processBlock(const float* buffer, int numSamples);

private:
    static constexpr auto INTERNAL_SAMPLE_RATE = 48000;
    static constexpr auto DEFAULT_TIME_CONSTANT_MS = 5000.0f;

    float m_instantPower;
    float m_alpha;

    std::unique_ptr<LanczosResampler> m_resampler;
};