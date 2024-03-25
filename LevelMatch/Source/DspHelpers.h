#pragma once

namespace DspHelpers
{
float dbToLinear(float db);
float linearToDb(float linear);
float powerToDb(float power);
float msToCoefficient(float ms, float samplerate);
}; // namespace DspHelpers