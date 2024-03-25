#include "LoudnessEstimator.h"

#include "DspHelpers.h"

#include <cmath>

LoudnessEstimator::LoudnessEstimator(float samplerate)
    : m_instantPower {0.0f}
    , m_alpha {DspHelpers::msToCoefficient(DEFAULT_TIME_CONSTANT_MS, samplerate)}
{
}

void LoudnessEstimator::processBlock(const float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        m_instantPower += m_alpha * (buffer[i] * buffer[i] - m_instantPower);
    }
}

float LoudnessEstimator::getLoudness() const
{
    return DspHelpers::powerToDb(m_instantPower);
}