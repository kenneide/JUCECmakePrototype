#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatchEditor::LevelMatchEditor(LevelMatch& p)
    : AudioProcessorEditor(&p)
    , m_appliedGainDb("Applied Gain", 0, "dB")
    , m_inputPowerDb("Input Power", 0, "dB")
    , m_referencePowerDb("Reference Power", 0, "dB")
    , value(0.0f)

{
    addAndMakeVisible(m_appliedGainDb);
    addAndMakeVisible(m_inputPowerDb);
    addAndMakeVisible(m_referencePowerDb);

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
    LevelMatch& processor = static_cast<LevelMatch&>(processor);

    m_appliedGainDb.setValue(processor.getAppliedGainDb());
    m_inputPowerDb.setValue(processor.getInputPowerDb());
    m_referencePowerDb.setValue(processor.getReferencePowerDb());
}

void LevelMatchEditor::resized()
{
    auto area = getLocalBounds();
    auto quarterHeight = area.getHeight() / 4;

    m_appliedGainDb.setBounds(area.removeFromTop(quarterHeight));
    m_inputPowerDb.setBounds(area.removeFromTop(quarterHeight));
    m_referencePowerDb.setBounds(area.removeFromTop(quarterHeight));
}
