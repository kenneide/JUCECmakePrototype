#include "BiquadCascade.h"

#include <cassert>

BiquadCascade::BiquadCascade(float samplerate,
                             const std::vector<BiquadCoefficients>& coefficients)
    : m_coefficients {coefficients}
{
    for (const auto& coeff: coefficients)
    {
        assert(coeff.a[0] == 1.0f);
        m_states.push_back(std::array<float, 2> {0.0f, 0.0f});
    }
}

void BiquadCascade::processBlock(const float* input, float* output, int numSamples)
{
    float tempBuffer[numSamples];

    std::copy(input, input + numSamples, tempBuffer);

    for (size_t i = 0; i < m_coefficients.size(); ++i)
    {
        process(tempBuffer, tempBuffer, numSamples, m_coefficients[i], m_states[i]);
    }

    std::copy(tempBuffer, tempBuffer + numSamples, output);
}

void BiquadCascade::process(const float* input,
                            float* output,
                            int numSamples,
                            const BiquadCoefficients& c,
                            std::array<float, 2>& s)
{
    for (int index = 0; index < numSamples; ++index)
    {
        auto x = input[index];
        output[index] = c.b[0] * x + s[1];
        s[1] = c.b[1] * x - c.a[1] * output[index] + s[0];
        s[0] = c.b[2] * x - c.a[2] * output[index];

        assert(!isinf(output[index]));
    }
}