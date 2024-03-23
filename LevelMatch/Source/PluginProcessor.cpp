#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatch::LevelMatch()
    : PluginHelpers::ProcessorBase(getIoLayout())
    , m_parameterState(*this,
                       nullptr,
                       "PARAMETERS",
                       {std::make_unique<juce::AudioParameterBool>(
                            juce::ParameterID {kMeasureReference, VERSION_HINT},
                            "Measure Reference LUFS",
                            false),
                        std::make_unique<juce::AudioParameterBool>(
                            juce::ParameterID {kMeasureInput, VERSION_HINT},
                            "Measure Input LUFS",
                            false),
                        std::make_unique<juce::AudioParameterBool>(
                            juce::ParameterID {kApplyGain, VERSION_HINT},
                            "Apply Matching Gain",
                            false)})
    , m_status {Status::OK}
    , m_inputLoudness {0.0f}
    , m_referenceLoudness {0.0f}
{
}

void LevelMatch::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_loudnessEstimators.clear();
    m_gains.clear();

    if (getNumInputChannels() == 0 || getNumOutputChannels() == 0)
    //|| getNumInputChannels() != 2 * getNumOutputChannels())
    {
        m_status = Status::INCORRECT_IO_LAYOUT;
        return;
    }

    const auto POWER_TIME_CONSTANT_S = 5.0f;
    const auto GAIN_TIME_CONSTANT_S = 0.05f;

    auto loudnessAlpha = 1.0f - std::exp(-1.0 / (sampleRate * POWER_TIME_CONSTANT_S));
    auto gainAlpha = 1.0f - std::exp(-1.0 / (sampleRate * GAIN_TIME_CONSTANT_S));

    for (int i = 0; i < getNumInputChannels(); ++i)
    {
        m_loudnessEstimators.push_back(std::make_unique<PowerEstimator>(loudnessAlpha));
    }

    for (int i = 0; i < getNumOutputChannels(); ++i)
    {
        m_gains.push_back(std::make_unique<Gain>());
        m_gains.back()->setAlpha(gainAlpha);
    }

    m_status = Status::OK;
}

void LevelMatch::processBlock(juce::AudioBuffer<float>& buffer,
                              juce::MidiBuffer& midiMessages)

{
    juce::ignoreUnused(midiMessages);

    if (m_status != Status::OK)
    {
        return;
    }

    // get number of input channels
    auto numberOfChannels = buffer.getNumChannels();
    auto numberOfSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numberOfChannels; ++channel)
    {
        m_loudnessEstimators[channel]->processBlock(buffer.getReadPointer(channel),
                                                    numberOfSamples);
    }

    if (m_parameterState.getRawParameterValue(kMeasureInput)->load() == 1.0f)
    {
        updateInputLoudness();
        updateAppliedGain();
    }

    if (m_parameterState.getRawParameterValue(kMeasureReference)->load() == 1.0f)
    {
        updateReferenceLoudness();
        updateAppliedGain();
    }

    if (m_parameterState.getRawParameterValue(kApplyGain)->load() == 0.0f)
    {
        return;
    }

    for (size_t channel = 0; channel < getNumOutputChannels(); ++channel)
    {
        m_gains[channel]->processBlock(buffer.getWritePointer(channel), numberOfSamples);
    }
}

void LevelMatch::updateInputLoudness()
{
    m_inputLoudness = 0.0f;

    size_t inputChannelStart = 0;
    size_t inputChannelEnd = getMainBusNumInputChannels() / 2;

    for (size_t channel = inputChannelStart; channel < inputChannelEnd; ++channel)
    {
        m_inputLoudness += m_loudnessEstimators[channel]->getLoudness();
    }
}

void LevelMatch::updateReferenceLoudness()
{
    m_referenceLoudness = 0.0f;

    size_t referenceChannelStart = getMainBusNumInputChannels() / 2;
    size_t referenceChannelEnd = getMainBusNumInputChannels();

    for (size_t channel = referenceChannelStart; channel < referenceChannelEnd; ++channel)
    {
        m_referenceLoudness += m_loudnessEstimators[channel]->getLoudness();
    }
}

void LevelMatch::updateAppliedGain()
{
    m_appliedGainDb = m_referenceLoudness - m_inputLoudness;
    m_appliedGainDb = std::clamp(m_appliedGainDb, MIN_GAIN_DB, MAX_GAIN_DB);

    for (size_t channel = 0; channel < getMainBusNumOutputChannels(); ++channel)
    {
        m_gains[channel]->setGainDb(m_appliedGainDb);
    }
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

juce::AudioProcessor::BusesProperties LevelMatch::getIoLayout()
{
    return BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withInput("Reference", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
}