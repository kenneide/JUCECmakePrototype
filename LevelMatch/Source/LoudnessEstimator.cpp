#include "LoudnessEstimator.h"

#include "DspHelpers.h"

#include <cmath>
#include <cassert>

LoudnessEstimator::LoudnessEstimator(float samplerate)
    : m_instantPower {0.0f}
    , m_alpha {DspHelpers::msToCoefficient(DEFAULT_TIME_CONSTANT_MS, samplerate)}
    , m_resampler {std::make_unique<LanczosResampler>(samplerate, INTERNAL_SAMPLE_RATE)}
    , m_kWeight {std::make_unique<BiquadCascade>(
          samplerate,
          std::vector {
              BiquadCoefficients {
                  {1.53512485958697f, -2.69169618940638f, 1.19839281085285f},
                  {1.0f, -1.69065929318241f, 0.73248077421585f}},
              BiquadCoefficients {{1.0f, -2.0f, 1.0f},
                                  {1.0f, -1.99004745483398f, 0.99007225036621f}}})}

{
}

void LoudnessEstimator::processBlock(const float* buffer, int numSamples)
{
    int outputSize = m_resampler->getOutputSize(numSamples);
    float outputBuffer[outputSize];
    int numDownsampled = m_resampler->process(buffer, numSamples, outputBuffer, outputSize);

    m_kWeight->processBlock(outputBuffer, outputBuffer, numDownsampled);

    for (int i = 0; i < numDownsampled; ++i)
    {
        m_instantPower += m_alpha * (outputBuffer[i] * outputBuffer[i] - m_instantPower);
    }
}

float LoudnessEstimator::getLoudness() const
{
    return DspHelpers::powerToDb(m_instantPower);
}