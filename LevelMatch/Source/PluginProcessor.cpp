#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatch::LevelMatch()
    : PluginHelpers::ProcessorBase(getIoLayout())
    , m_parameterState(*this,
                       nullptr,
                       "PARAMETERS",
                       {std::make_unique<juce::AudioParameterBool>(
                            kMeasureReference, "Measure Reference LUFS", false),
                        std::make_unique<juce::AudioParameterBool>(
                            kMeasureInput, "Measure Input LUFS", false),
                        std::make_unique<juce::AudioParameterBool>(
                            kApplyGain, "Apply Matching Gain", false)})
    , m_status(Status::OK)
    , m_appliedGain(1.0f)
    , m_instantInputPower(0.0f)
    , m_instantReferencePower(0.0f)
    , m_alpha(1 - 0.999f)
{
}

void LevelMatch::processBlock(juce::AudioBuffer<float>& buffer,
                              juce::MidiBuffer& midiMessages)

{
    juce::ignoreUnused(midiMessages);

    auto inputPtr = buffer.getArrayOfReadPointers();

    auto numberOfSamples = buffer.getNumSamples();
    auto numberOfChannels = buffer.getNumChannels();

    static constexpr auto NUM_STEREO = 2; // stereo assumed

    // Ensure we have enough channels for reference
    if (numberOfChannels < NUM_STEREO * 2)
    {
        m_status = static_cast<Status>(numberOfChannels);
        //return;
    }

    for (int channel = 0; channel < NUM_STEREO; ++channel)
    {
        auto* input = inputPtr[channel];
        auto* reference = inputPtr[channel + NUM_STEREO];

        for (int sample = 0; sample < numberOfSamples; ++sample)
        {
            m_instantInputPower +=
                m_alpha * (input[sample] * input[sample] - m_instantInputPower);
            m_instantReferencePower +=
                m_alpha
                * (reference[sample] * reference[sample] - m_instantReferencePower);
        }
    }

    if (m_parameterState.getRawParameterValue(kMeasureInput)->load() == 1.0f)
    {
        m_measureInputPowerDb = 10.0f * std::log10(m_instantInputPower);
        m_appliedGainDb = m_measureReferencePowerDb - m_measureInputPowerDb;

        m_appliedGainDb = std::clamp(m_appliedGainDb, MIN_GAIN_DB, MAX_GAIN_DB);
        m_appliedGain = std::pow(10.0f, m_appliedGainDb / 20.0f);
    }

    if (m_parameterState.getRawParameterValue(kMeasureReference)->load() == 1.0f)
    {
        m_measureReferencePowerDb = 10.0f * std::log10(m_instantReferencePower);
        m_appliedGainDb = m_measureReferencePowerDb - m_measureInputPowerDb;
        m_appliedGainDb = std::clamp(m_appliedGainDb, MIN_GAIN_DB, MAX_GAIN_DB);
        m_appliedGain = std::pow(10.0f, m_appliedGainDb / 20.0f);
    }

    if (m_parameterState.getRawParameterValue(kApplyGain)->load() == 0.0f)
    {
        return;
    }

    // apply the gain to the input signal to match the lufs of the reference signal
    buffer.applyGain(m_appliedGain);
}

juce::AudioProcessorEditor* LevelMatch::createEditor()
{
    return new LevelMatchEditor(*this);
}

void LevelMatch::getStateInformation(juce::MemoryBlock& destData)
{
    //Serializes your parameters, and any other potential data into an XML:

    juce::ValueTree params("Params");

    for (auto& param: getParameters())
    {
        juce::ValueTree paramTree(PluginHelpers::getParamID(param));
        paramTree.setProperty("Value", param->getValue(), nullptr);
        params.appendChild(paramTree, nullptr);
    }

    juce::ValueTree pluginPreset("MyPlugin");
    pluginPreset.appendChild(params, nullptr);
    //This a good place to add any non-parameters to your preset

    copyXmlToBinary(*pluginPreset.createXml(), destData);
}

void LevelMatch::setStateInformation(const void* data, int sizeInBytes)
{
    //Loads your parameters, and any other potential data from an XML:

    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml != nullptr)
    {
        auto preset = juce::ValueTree::fromXml(*xml);
        auto params = preset.getChildWithName("Params");

        for (auto& param: getParameters())
        {
            auto paramTree = params.getChildWithName(PluginHelpers::getParamID(param));

            if (paramTree.isValid())
                param->setValueNotifyingHost(paramTree["Value"]);
        }

        //Load your non-parameter data now
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LevelMatch();
}

bool LevelMatch::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return true;

    juce::AudioChannelSet inputChannels = layouts.getMainInputChannelSet();
    juce::AudioChannelSet outputChannels = layouts.getMainOutputChannelSet();

    if (inputChannels.size() == 4 && outputChannels.size() == 2)
    {
        return true;
    }

    return false;
}

juce::AudioProcessor::BusesProperties LevelMatch::getIoLayout()
{
    return BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withInput("Reference", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
}