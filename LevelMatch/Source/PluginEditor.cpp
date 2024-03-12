#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatchEditor::LevelMatchEditor(LevelMatch& p)
    : AudioProcessorEditor(&p)
    , m_appliedGainDb("Applied Gain", 0, "dB")
    , m_inputPowerDb("Input Power", 0, "dB")
    , m_referencePowerDb("Reference Power", 0, "dB")
    , value(0.0f)
    , m_debugMessage("Debug Message", "STATUS: OK")

{
    addAndMakeVisible(m_appliedGainDb);
    addAndMakeVisible(m_inputPowerDb);
    addAndMakeVisible(m_referencePowerDb);
    addAndMakeVisible(m_debugMessage);

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
    case LevelMatch::Status::ONE_CHANNEL:
        m_debugMessage.setText("STATUS: ONE_CHANNEL", juce::dontSendNotification);
        break;
    case LevelMatch::Status::TWO_CHANNELS:
        m_debugMessage.setText("STATUS: TWO_CHANNELS", juce::dontSendNotification);
        break;
    case LevelMatch::Status::THREE_CHANNELS:
        m_debugMessage.setText("STATUS: THREE_CHANNELS", juce::dontSendNotification);
        break;
    default:
        jassertfalse;
    }
}

void LevelMatchEditor::resized()
{
    auto area = getLocalBounds();
    auto quarterHeight = area.getHeight() / 4;

    m_appliedGainDb.setBounds(area.removeFromTop(quarterHeight));
    m_inputPowerDb.setBounds(area.removeFromTop(quarterHeight));
    m_referencePowerDb.setBounds(area.removeFromTop(quarterHeight));

    m_debugMessage.setBounds(area.removeFromTop(quarterHeight));
}
