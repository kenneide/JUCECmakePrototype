#pragma once

#include <shared_plugin_helpers/shared_plugin_helpers.h>

#include "PowerEstimator.h"
#include "Gain.h"

class LevelMatch : public PluginHelpers::ProcessorBase
{
public:
    enum class Status
    {
        OK = 0,
        ERROR,
        INCORRECT_IO_LAYOUT
    };

    LevelMatch();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    float getAppliedGainDb() const { return m_appliedGainDb; }
    float getInputPowerDb() const { return m_inputLoudness; }
    float getReferencePowerDb() const { return m_referenceLoudness; }

    Status getStatus() const { return m_status; }

    juce::AudioProcessorValueTreeState& getValueStateTree() { return m_parameterState; }

private:
    static constexpr auto VERSION_HINT = 1;

    static constexpr auto kMeasureReference = "measureReference";
    static constexpr auto kMeasureInput = "measureInput";
    static constexpr auto kApplyGain = "applyGain";

    static BusesProperties getIoLayout();

    static constexpr auto MAX_GAIN_DB = 24.0f;
    static constexpr auto MIN_GAIN_DB = -24.0f;

    void updateInputLoudness();
    void updateReferenceLoudness();
    void updateAppliedGain();

    juce::AudioProcessorValueTreeState m_parameterState;

    Status m_status;

    float m_inputLoudness;
    float m_referenceLoudness;
    float m_appliedGainDb;

    std::vector<std::unique_ptr<PowerEstimator>> m_loudnessEstimators;
    std::vector<std::unique_ptr<Gain>> m_gains;
};
