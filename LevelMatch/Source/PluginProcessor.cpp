#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatch::LevelMatch()
    : PluginHelpers::ProcessorBase(getIoLayout())
    , m_matchingGain(1.0f)
    , m_instantInputPower(0.0f)
    , m_instantReferencePower(0.0f)
    , m_alpha(0.99f)
{
    parameters.add(*this);
}

void LevelMatch::processBlock(juce::AudioBuffer<float>& buffer,
                              juce::MidiBuffer& midiMessages)

{
    juce::ignoreUnused(midiMessages);

    auto inputPtr = buffer.getArrayOfReadPointers();

    auto numberOfSamples = buffer.getNumSamples();
    auto numberOfChannels = buffer.getNumChannels();
    auto refererencePtr = inputPtr + 2; // stereo assumed

    for (int channel = 0; channel < numberOfChannels; ++channel)
    {
        auto* input = inputPtr[channel];
        auto* reference = refererencePtr[channel];

        for (int sample = 0; sample < numberOfSamples; ++sample)
        {
            m_instantInputPower +=
                m_alpha * (input[sample] * input[sample] - m_instantInputPower);
            m_instantReferencePower +=
                m_alpha
                * (reference[sample] * reference[sample] - m_instantReferencePower);
        }
    }

    if (parameters.measureInput->get())
    {
        m_measureInputPowerDb = 10.0f * std::log10(m_instantInputPower);
        m_matchingGainDb = m_measureReferencePowerDb - m_measureInputPowerDb;

        m_matchingGainDb = std::clamp(m_matchingGainDb, MIN_GAIN_DB, MAX_GAIN_DB);
        m_matchingGain = std::pow(10.0f, m_matchingGainDb / 20.0f);
    }

    if (parameters.measureReference->get())
    {
        m_measureReferencePowerDb = 10.0f * std::log10(m_instantReferencePower);
        m_matchingGainDb = m_measureReferencePowerDb - m_measureInputPowerDb;
        m_matchingGainDb = std::clamp(m_matchingGainDb, MIN_GAIN_DB, MAX_GAIN_DB);
        m_matchingGain = std::pow(10.0f, m_matchingGainDb / 20.0f);
    }

    if (!parameters.applyGain->get())
    {
        return;
    }

    // apply the gain to the input signal to match the lufs of the reference signal
    buffer.applyGain(m_matchingGain);
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