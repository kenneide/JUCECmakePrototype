#pragma once

#include "Parameters.h"

class LevelMatch : public PluginHelpers::ProcessorBase
{
public:
    LevelMatch();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    float getMatchingGain() const { return m_matchingGain; }
    void setMatchingGain(float gain) { m_matchingGain = gain; }

private:
    Parameters parameters;
    float m_matchingGain;
};
