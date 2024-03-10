#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatchEditor::LevelMatchEditor(LevelMatch& p)
    : AudioProcessorEditor(&p)
{
    m_gainLabel.setText("Applied Gain", juce::dontSendNotification);
    addAndMakeVisible(m_gainLabel);

    auto levelMatch = dynamic_cast<LevelMatch*>(getAudioProcessor());

    m_gainValueLabel.setText(juce::String::formatted("%4.1f [dB]", levelMatch->getMatchingGain()), juce::dontSendNotification);
    addAndMakeVisible(m_gainValueLabel);


    addAndMakeVisible(editor);
    setSize(400, 300);
}

void LevelMatchEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void LevelMatchEditor::resized()
{
    editor.setBounds(getLocalBounds());
}
