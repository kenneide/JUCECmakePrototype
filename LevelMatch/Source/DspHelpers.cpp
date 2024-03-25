#pragma once

#include "DspHelpers.h"
#include <cmath>

float DspHelpers::dbToLinear(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

float DspHelpers::linearToDb(float linear)
{
    return 20.0f * std::log10(linear);
}

float DspHelpers::powerToDb(float power)
{
    return 10.0f * std::log10(power);
}

float DspHelpers::msToCoefficient(float ms, float samplerate)
{
    return 1.0f - std::exp(-1.0f / (ms / 1000.0f * samplerate));
}