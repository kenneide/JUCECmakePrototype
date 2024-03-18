#pragma once

class PowerEstimator
{
public:
    PowerEstimator(float alpha);
    ~PowerEstimator() = default;

    float getPower() const;

    void processBlock(const float* buffer, int numSamples);

private:
    float m_instantPower;
    float m_alpha;
};