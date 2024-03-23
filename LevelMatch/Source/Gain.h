#pragma once

class Gain
{
public:
    Gain(float gainDb = 0.0f);
    ~Gain() = default;

    float getGainDb() const;
    void setGainDb(float gain);

    void setAlpha(float alpha) { m_alpha = alpha; }

    void processBlock(float* buffer, int numSamples);

private:
    static constexpr auto MIN_GAIN_DB = -96.0f;
    static constexpr auto MAX_GAIN_DB = 24.0f;

    float m_gain;
    float m_smoothGain;
    float m_alpha;
};