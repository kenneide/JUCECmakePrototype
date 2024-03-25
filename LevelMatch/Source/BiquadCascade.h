#pragma once

#include <vector>
#include <array>

struct BiquadCoefficients
{
    std::array<float, 3> b;
    std::array<float, 3> a;
};

class BiquadCascade
{
public:
    BiquadCascade(float samplerate, const std::vector<BiquadCoefficients>& coefficients);
    ~BiquadCascade() = default;

    void processBlock(const float* input, float* output, int numSamples);

private:
    static void process(const float* input,
                        float* output,
                        int numSamples,
                        const BiquadCoefficients& c,
                        std::array<float, 2>& s);

    std::vector<BiquadCoefficients> m_coefficients;
    std::vector<std::array<float, 2>> m_states;
};