#include "Gain.h"

#include "DspHelpers.h"

#include <cmath>
#include <cassert>

Gain::Gain(float samplerate)
    : m_gain {0.0f}
    , m_smoothGain {1.0f}
    , m_alpha {DspHelpers::msToCoefficient(DEFAULT_TIME_CONSTANT_MS, samplerate)}
{
    setGainDb(DEFAULT_GAIN_DB);
    m_smoothGain = m_gain;
}

float Gain::getGainDb() const
{
    return DspHelpers::linearToDb(m_gain);
}

void Gain::setGainDb(float gainDb)
{
    assert(gainDb >= MIN_GAIN_DB && gainDb <= MAX_GAIN_DB);
    m_gain = DspHelpers::dbToLinear(gainDb);
}

void Gain::processBlock(float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        m_smoothGain += m_alpha * (m_gain - m_smoothGain);
        buffer[i] *= m_smoothGain;
    }
}