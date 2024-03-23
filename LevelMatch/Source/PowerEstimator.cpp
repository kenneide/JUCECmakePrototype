#include "PowerEstimator.h"

#include <cmath>

PowerEstimator::PowerEstimator(float alpha)
    : m_instantPower(0.0f)
    , m_alpha(alpha)
{
}

float PowerEstimator::getPower() const
{
    return m_instantPower;
}

void PowerEstimator::processBlock(const float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        m_instantPower += m_alpha * (buffer[i] * buffer[i] - m_instantPower);
    }
}

float PowerEstimator::getLoudness() const
{
    return 10.0f * std::log10(m_instantPower);
}