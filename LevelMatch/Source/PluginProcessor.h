#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>

class LevelMatch : public PluginHelpers::ProcessorBase
{
public:
    enum class Status
    {
        OK = 0,
        ONE_CHANNEL,
        TWO_CHANNELS,
        THREE_CHANNELS,
        FOUR_CHANNELS
    };

    LevelMatch();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    bool isBusesLayoutSupported (const BusesLayout&) const override;

    float getAppliedGainDb() const { return m_appliedGainDb; }
    float getInputPowerDb() const { return m_measureInputPowerDb; }
    float getReferencePowerDb() const { return m_measureReferencePowerDb; }

    Status getStatus() const { return m_status; }

    juce::AudioProcessorValueTreeState& getValueStateTree() { return m_parameterState; }

private:
    static constexpr auto kMeasureReference = "measureReference";
    static constexpr auto kMeasureInput = "measureInput";
    static constexpr auto kApplyGain = "applyGain";

    static BusesProperties getIoLayout();

    static constexpr auto MAX_GAIN_DB = 24.0f;
    static constexpr auto MIN_GAIN_DB = -24.0f;

    juce::AudioProcessorValueTreeState m_parameterState;

    float m_appliedGain;

    float m_instantInputPower;
    float m_instantReferencePower;
    float m_alpha;

    float m_measureInputPowerDb;
    float m_measureReferencePowerDb;
    float m_appliedGainDb;

    Status m_status;
};
