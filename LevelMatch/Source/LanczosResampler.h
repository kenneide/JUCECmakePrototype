#pragma once

#include <filesystem> // for path
#include <sys/cdefs.h> // for __unused
#include <vector> // for vector

class LanczosResampler
{
public:
    LanczosResampler(long inputRate,
                     long outputRate,
                     int kernelSize = DEFAULT_KERNEL_SIZE,
                     float bandwidth = DEFAULT_NORMALIZED_BANDWIDTH);

    ~LanczosResampler() = default;

    bool setInputRate(long rate);
    long getInputRate() const { return m_inputRate; }

    bool setOutputRate(long rate);
    long getOutputRate() const { return m_outputRate; }

    float getUpsamplingFactor() const { return m_upsampling.factor; }
    int getDownsamplingFactor() const { return m_downsampling.factor; }

    int getKernelSize() const { return m_kernelSize; }

    // Returns the required number of input samples for a given number of output samples.
    int getInputSize(int outputSize) const;

    // Returns the required number of output samples for a given number of input samples.
    int getOutputSize(int inputSize) const;

    int getLatencyInSamples() const;
    float getLatencyTimeInSeconds() const;

    int getBufferCapacity() const { return BUFFER_SIZE; }

    void reset();

    int process(const float* monoInput, int inputSize, float* monoOutput, int outputSize);

private:
    static constexpr int BUFFER_SIZE = 6912;

    static constexpr int DEFAULT_KERNEL_SIZE = 12;
    static constexpr float DEFAULT_NORMALIZED_BANDWIDTH = 0.79f;

    void setup();

    static inline float kernel(float x, int size);
    static inline float kernel(float x, int size, float B);

    int interpolate(const float* samples,
                    int numSamples,
                    float* buffer,
                    int size __unused);
    int decimate(const float* samples, int numSamples, float* buffer, int size __unused);

    long m_inputRate;
    long m_outputRate;
    long m_internalRate;

    int m_kernelSize;

    bool m_isUpsampling;
    bool m_isDownsampling;

    struct upsampling
    {
        float factor;
        float stepSize;
        float offset;
        int windowLength;
        int overlap;
        int size;
    } m_upsampling;

    struct downsampling
    {
        int factor;
        float stepSize;
        int offset;
        std::vector<float> window;
        int overlap;
        float bandwidth;
    } m_downsampling;

    std::vector<float> m_buffer;
    std::vector<float> m_interimBuffer; // for interpolation
};