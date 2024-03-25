#include "LanczosResampler.h"

#include <cassert>

LanczosResampler::LanczosResampler(long inputRate,
                                   long outputRate,
                                   int kernelSize,
                                   float bandwidth)
    : m_inputRate {inputRate}
    , m_outputRate {outputRate}
    , m_kernelSize {kernelSize}
{
    m_downsampling.bandwidth = bandwidth;

    setup();
    reset();
}

bool LanczosResampler::setInputRate(long rate)
{
    if (rate == m_inputRate)
        return true;

    m_inputRate = rate;
    setup();
    reset();

    return true;
}

bool LanczosResampler::setOutputRate(long rate)
{
    if (rate == m_outputRate)
        return true;

    m_outputRate = rate;
    setup();
    reset();

    return true;
}

int LanczosResampler::getInputSize(int outputSize) const
{
    if (!m_isUpsampling && !m_isDownsampling) // doing nothing
        return outputSize;

    if (m_isUpsampling)
    {
        if (m_isDownsampling) // upsampling and downsampling
            return static_cast<int>(std::floor(outputSize / m_upsampling.factor * m_downsampling.factor));

        // only upsampling
        return static_cast<int>(std::floor(outputSize / m_upsampling.factor));
    }

    // only downsampling
    return static_cast<int>(std::floor((float) outputSize * m_downsampling.factor));
}

int LanczosResampler::getOutputSize(int inputSize) const
{
    if (!m_isUpsampling && !m_isDownsampling) // doing nothing
        return inputSize;

    if (m_isUpsampling)
    {
        if (m_isDownsampling) // upsampling and downsampling
            return static_cast<int>(std::ceil(inputSize * m_upsampling.factor / m_downsampling.factor));

        // only upsampling
        return static_cast<int>(std::ceil(inputSize * m_upsampling.factor));
    }

    // only downsampling
    return static_cast<int>(std::ceil((float) inputSize / m_downsampling.factor));
}

int LanczosResampler::getLatencyInSamples() const
{
    return static_cast<int>(std::floor(getLatencyTimeInSeconds() * m_outputRate));
}

float LanczosResampler::getLatencyTimeInSeconds() const
{
    float upsamplingLatencyTime = static_cast<float>(m_kernelSize) / m_inputRate;
    float downsamplingLatencyTime = static_cast<float>(m_kernelSize) / m_outputRate;

    return m_isUpsampling * upsamplingLatencyTime
           + m_isDownsampling * downsamplingLatencyTime;
}

void LanczosResampler::setup()
{
    // decimation by an integer factor
    m_downsampling.factor = std::ceil(static_cast<float>(m_inputRate) / m_outputRate);
    m_downsampling.stepSize = 1.0f / m_downsampling.factor;
    m_downsampling.window.resize(static_cast<size_t>(2 * m_kernelSize * m_downsampling.factor));
    m_downsampling.overlap = m_downsampling.window.size() - 1;
    m_isDownsampling = m_downsampling.factor > 1;

    float j = -m_kernelSize;

    for (auto& w: m_downsampling.window)
    {
        j += m_downsampling.stepSize;
        w = kernel(-j, m_kernelSize, m_downsampling.bandwidth);
    }

    // internal rate
    m_internalRate = m_outputRate * m_downsampling.factor;

    // interpolation by a real factor
    m_upsampling.factor = static_cast<float>(m_internalRate) / m_inputRate;
    m_upsampling.stepSize = 1 / m_upsampling.factor;
    m_upsampling.windowLength = 2 * m_kernelSize;
    m_upsampling.overlap = m_upsampling.windowLength - 1;
    m_isUpsampling = m_upsampling.factor > 1.0f;

    // internal buffers
    m_buffer.clear();
    m_interimBuffer.clear();

    size_t bufferSize = BUFFER_SIZE;
    size_t interimSize;

    if (m_isUpsampling)
    {
        interimSize = m_upsampling.size = static_cast<int>(std::ceil(bufferSize * m_upsampling.factor));
        bufferSize += m_upsampling.overlap;

        if (m_isDownsampling)
        {
            interimSize += m_downsampling.overlap;
        }
    }
    else
    {
        interimSize = m_upsampling.size = 0;

        if (m_isDownsampling)
        {
            bufferSize += m_downsampling.overlap;
        }
    }

    m_buffer.resize(bufferSize, 0.0f);
    m_interimBuffer.resize(interimSize, 0.0f);
}

void LanczosResampler::reset()
{
    std::fill(m_buffer.begin(), m_buffer.end(), 0.0f);

    if (m_isUpsampling)
    {
        std::fill(m_interimBuffer.begin(), m_interimBuffer.end(), 0.0f);
    }

    m_upsampling.offset = 0.0f; // m_kernelSize;
    m_downsampling.offset = 0; // m_kernelSize * m_downsampling.factor;
}

inline float LanczosResampler::kernel(float x, int size)
{
    if (x == 0.0f)
        return 1.0f;

    if (x <= -size || x >= size)
        return 0.0f;

    x *= static_cast<float>(M_PI);

    return size * std::sin(x) * std::sin(x / size) / (x * x);
}

inline float LanczosResampler::kernel(float x, int size, float B)
{
    if (x == 0.0f)
        return 1.0f;

    if (x <= -size || x >= size)
        return 0.0f;

    x *= static_cast<float>(M_PI);

    return size * std::sin(B * x) * std::sin(x / size) / (x * x);
}

int LanczosResampler::interpolate(const float* samples,
                                  int numSamples,
                                  float* buffer,
                                  int size __unused)
{
    if (!m_isUpsampling)
        return -1;

    float t = m_upsampling.offset;
    int n = static_cast<int>(t);
    float* u = buffer; // upsampled signal

    while (n < numSamples)
    {
        float phi = static_cast<float>(t - n); // phase
        int j = -m_kernelSize;
        const float* s = samples + n;
        *u = 0.0f;

        for (size_t i = 0; i < m_upsampling.windowLength; i++)
        {
            *u += *s++ * kernel(phi - ++j, m_kernelSize);
        }

        t += m_upsampling.stepSize;
        n = static_cast<int>(t);

        assert(static_cast<int>(u - buffer) < size);
        u++;
    }

    m_upsampling.offset = t - numSamples;

    return static_cast<int>(u - buffer); // number of upsampled samples
}

int LanczosResampler::decimate(const float* samples,
                               int numSamples,
                               float* buffer,
                               int size __unused)
{
    if (!m_isDownsampling)
        return -1;

    int n = m_downsampling.offset;
    float* d = buffer; // downsampled signal

    while (n < numSamples)
    {
        const float* w = m_downsampling.window.data();
        const float* s = samples + n;
        *d = 0.0f;

        for (size_t i = 0; i < m_downsampling.window.size(); i++)
        {
            *d += *s++ * *w++;
        }

        n += m_downsampling.factor;

        assert(static_cast<int>(d - buffer) < size);
        *d++ /= m_downsampling.factor;
    };

    m_downsampling.offset = n - numSamples;

    return static_cast<int>(d - buffer); // number of downsampled samples
}

int LanczosResampler::process(const float* monoInput,
                              int inputSize,
                              float* monoOutput,
                              int outputSize)
{
    if (monoInput == nullptr)
        return -1;

    if (inputSize <= 0)
        return 0;

    if (monoOutput == nullptr)
        return -1;

    if (!m_isUpsampling && !m_isDownsampling)
    {
        assert(outputSize >= inputSize);
        memcpy(monoOutput, monoInput, inputSize * sizeof(float));
        return inputSize;
    }

    assert(inputSize <= m_buffer.size());
    int numProcessed = 0;

    if (m_isUpsampling)
    {
        memcpy(
            m_buffer.data() + m_upsampling.overlap, monoInput, inputSize * sizeof(float));

        if (m_isDownsampling)
        { // upsampling and downsampling
            int numUpsampled =
                interpolate(m_buffer.data(),
                            inputSize,
                            m_interimBuffer.data() + m_downsampling.overlap,
                            m_upsampling.size);
            numProcessed =
                decimate(m_interimBuffer.data(), numUpsampled, monoOutput, outputSize);
            memmove(m_interimBuffer.data(),
                    m_interimBuffer.data() + numUpsampled,
                    m_downsampling.overlap * sizeof(float));
        }
        else
        { // only upsampling
            numProcessed =
                interpolate(m_buffer.data(), inputSize, monoOutput, outputSize);
        }

        memmove(m_buffer.data(),
                m_buffer.data() + inputSize,
                m_upsampling.overlap * sizeof(float));
    }
    else
    { // only downsampling
        memcpy(m_buffer.data() + m_downsampling.overlap,
               monoInput,
               inputSize * sizeof(float));
        numProcessed = decimate(m_buffer.data(), inputSize, monoOutput, outputSize);
        memmove(m_buffer.data(),
                m_buffer.data() + inputSize,
                m_downsampling.overlap * sizeof(float));
    }

    return numProcessed;
}