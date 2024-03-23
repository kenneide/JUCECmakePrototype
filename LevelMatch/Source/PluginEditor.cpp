#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatchEditor::LevelMatchEditor(LevelMatch& p)
    : AudioProcessorEditor(&p)
    , m_appliedGainDb("Applied Gain", 0, "dB")
    , m_inputPowerDb("Input Loudness", 0, "dB")
    , m_referencePowerDb("Reference Loudness", 0, "dB")
    , m_measureInputButton("Measure Input", false)
    , m_measureReferenceButton("Measure Reference", false)
    , m_applyGainButton("Apply Gain", false)
    , m_debugMessage("Debug Message", "STATUS: OK")
{
    addAndMakeVisible(m_appliedGainDb);
    addAndMakeVisible(m_inputPowerDb);
    addAndMakeVisible(m_referencePowerDb);
    addAndMakeVisible(m_debugMessage);

    addAndMakeVisible(m_measureInputButton);
    addAndMakeVisible(m_measureReferenceButton);
    addAndMakeVisible(m_applyGainButton);

    LevelMatch* processor = dynamic_cast<LevelMatch*>(getAudioProcessor());

    m_inputButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            processor->getValueStateTree(),
            "measureInput",
            m_measureInputButton.getButton());

    m_referenceButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            processor->getValueStateTree(),
            "measureReference",
            m_measureReferenceButton.getButton());

    m_applyGainButtonAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            processor->getValueStateTree(),
            "applyGain",
            m_applyGainButton.getButton());

    processor->getValueStateTree().addParameterListener("applyGain", this);

    setSize(400, 300);

    static constexpr auto TIMER_INTERVAL_MS = 100;

    startTimer(TIMER_INTERVAL_MS);
}

void LevelMatchEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void LevelMatchEditor::timerCallback()
{
    LevelMatch* processor = dynamic_cast<LevelMatch*>(getAudioProcessor());

    m_appliedGainDb.setValue(processor->getAppliedGainDb());
    m_inputPowerDb.setValue(processor->getInputPowerDb());
    m_referencePowerDb.setValue(processor->getReferencePowerDb());

    switch (processor->getStatus())
    {
        case LevelMatch::Status::OK:
            m_debugMessage.setText("STATUS: OK", juce::dontSendNotification);
            break;
        case LevelMatch::Status::ERROR:
            m_debugMessage.setText("STATUS: ERROR", juce::dontSendNotification);
            break;
        case LevelMatch::Status::INCORRECT_IO_LAYOUT:
            m_debugMessage.setText("STATUS: INCORRECT IO LAYOUT", juce::dontSendNotification);
            break;
        default:
            jassertfalse;
    }
}

void LevelMatchEditor::resized()
{
    auto area = getLocalBounds();
    auto fractionHeight = area.getHeight() / 7;

    m_appliedGainDb.setBounds(area.removeFromTop(fractionHeight));
    m_inputPowerDb.setBounds(area.removeFromTop(fractionHeight));
    m_referencePowerDb.setBounds(area.removeFromTop(fractionHeight));

    m_measureInputButton.setBounds(area.removeFromTop(fractionHeight));
    m_measureReferenceButton.setBounds(area.removeFromTop(fractionHeight));
    m_applyGainButton.setBounds(area.removeFromTop(fractionHeight));

    m_debugMessage.setBounds(area.removeFromTop(fractionHeight));
}

void LevelMatchEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "applyGain")
    {
        bool applyGainEnabled = newValue == 1.0f;
        bool measurementEnabled = !applyGainEnabled;

        if (!measurementEnabled)
        {
            m_measureInputButton.getButton().setEnabled(false);
            m_measureReferenceButton.getButton().setEnabled(false);
            m_measureInputButton.setToggleState(false, juce::sendNotification);
            m_measureReferenceButton.setToggleState(false, juce::sendNotification);
        }
        else
        {
            m_measureInputButton.getButton().setEnabled(true);
            m_measureReferenceButton.getButton().setEnabled(true);
        }
    }
}
