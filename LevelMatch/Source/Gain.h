#pragma once

class Gain
{
public:
    Gain(float samplerate);
    ~Gain() = default;

    float getGainDb() const;
    void setGainDb(float gain);

    void processBlock(float* buffer, int numSamples);

private:
    static constexpr auto DEFAULT_TIME_CONSTANT_MS = 50.0f;

    static constexpr auto MIN_GAIN_DB = -96.0f;
    static constexpr auto DEFAULT_GAIN_DB = 0.0f;
    static constexpr auto MAX_GAIN_DB = 24.0f;

    float m_gain;
    float m_smoothGain;
    float m_alpha;
};