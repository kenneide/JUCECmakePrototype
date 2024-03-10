#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatch::LevelMatch()
    : m_matchingGain(1.0f)
{
    parameters.add(*this);
}

void LevelMatch::processBlock(juce::AudioBuffer<float>& buffer,
                              juce::MidiBuffer& midiMessages)

{
    juce::ignoreUnused(midiMessages);

    auto inputPtr = buffer.getArrayOfReadPointers();
    auto numberOfSamples = buffer.getNumSamples();
    auto refererencePtr = inputPtr + 2; // stereo assumed

    if (parameters.measureInputLufs->get())
    {
        // measure the lufs of the input signal

        // display the lufs of the input signal
        
    }

    if (parameters.measureReferenceLufs->get())
    {
        // measure the lufs of the reference signal
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
