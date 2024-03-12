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

    float getMatchingGainDb() const { return m_matchingGainDb; }

    bool isBusesLayoutSupported (const BusesLayout&) const override;

private:
    static BusesProperties getIoLayout();

    static constexpr auto MAX_GAIN_DB = 24.0f;
    static constexpr auto MIN_GAIN_DB = -24.0f;

    Parameters parameters;

    float m_matchingGain;

    float m_instantInputPower;
    float m_instantReferencePower;
    float m_alpha;

    float m_measureInputPowerDb;
    float m_measureReferencePowerDb;
    float m_matchingGainDb;
};
