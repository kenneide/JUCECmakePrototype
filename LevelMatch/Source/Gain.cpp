#include "Gain.h"

#include <cmath>
#include <cassert>

Gain::Gain(float gainDb)
    : m_gain(0.0f)
    , m_smoothGain(0.0f)
    , m_alpha(0.99f)
{
    setGainDb(gainDb);
    m_smoothGain = m_gain;
}

float Gain::getGainDb() const
{
    return 20.0f * std::log10(m_gain);
}

void Gain::setGainDb(float gainDb)
{
    assert(gainDb >= MIN_GAIN_DB && gainDb <= MAX_GAIN_DB);
    m_gain = std::pow(10.0f, gainDb / 20.0f);
}

void Gain::processBlock(float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        m_smoothGain += m_alpha * (m_gain - m_smoothGain);
        buffer[i] *= m_smoothGain;
    }
}