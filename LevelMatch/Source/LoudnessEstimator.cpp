#include "LoudnessEstimator.h"

#include "DspHelpers.h"

#include <cmath>

LoudnessEstimator::LoudnessEstimator(float samplerate)
    : m_instantPower {0.0f}
    , m_alpha {DspHelpers::msToCoefficient(DEFAULT_TIME_CONSTANT_MS, samplerate)}
    , m_resampler {std::make_unique<LanczosResampler>(samplerate, INTERNAL_SAMPLE_RATE)}
{
}

void LoudnessEstimator::processBlock(const float* buffer, int numSamples)
{
    // resample to 48kHz
    int outputSize = m_resampler->getOutputSize(numSamples);
    float outputBuffer[outputSize];
    int numDownsampled = m_resampler->process(buffer, numSamples, outputBuffer, outputSize);

    for (int i = 0; i < numDownsampled; ++i)
    {
        m_instantPower += m_alpha * (outputBuffer[i] * outputBuffer[i] - m_instantPower);
    }
}

float LoudnessEstimator::getLoudness() const
{
    return DspHelpers::powerToDb(m_instantPower);
}