#pragma once

class PowerEstimator
{
public:
    PowerEstimator(float alpha = 0.99f);
    ~PowerEstimator() = default;

    float getLoudness() const;
    float getPower() const;

    void processBlock(const float* buffer, int numSamples);

private:
    float m_instantPower;
    float m_alpha;
};