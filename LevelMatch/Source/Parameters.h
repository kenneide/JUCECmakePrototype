#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>

struct Parameters
{
    void add(juce::AudioProcessor& processor) const
    {
        processor.addParameter(applyGain);
        processor.addParameter(measureReference);
        processor.addParameter(measureInput);
    }

    juce::AudioParameterBool* applyGain =
        new juce::AudioParameterBool({"applyGain", 1}, "Apply Gain", true);

    juce::AudioParameterBool* measureReference = new juce::AudioParameterBool(
        {"measureReferenceLufs", 1}, "Measure Reference LUFS", true);

    juce::AudioParameterBool* measureInput =
        new juce::AudioParameterBool({"measureInputLufs", 1}, "Measure Input LUFS", true);
};