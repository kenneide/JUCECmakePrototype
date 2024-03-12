#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMatchEditor::LevelMatchEditor(LevelMatch& p)
    : AudioProcessorEditor(&p)
    , editor(p)
    , m_gainLabel("gainLabel", "Applied Gain:")
    , m_gainValueLabel("gainValueLabel", "")
{

    auto levelMatch = dynamic_cast<LevelMatch*>(getAudioProcessor());

    addAndMakeVisible(m_gainValueLabel);
    m_gainValueLabel.setText(
        juce::String::formatted("%4.1f [dB]", levelMatch->getMatchingGainDb()),
        juce::dontSendNotification);
    m_gainValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible(m_gainLabel);
    m_gainValueLabel.attachToComponent(&m_gainLabel, true);

    addAndMakeVisible(editor);

    //addAndMakeVisible(m_button);
    //m_button.setButtonText("Reset");


    setSize(400, 300);
}

void LevelMatchEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    auto levelMatch = dynamic_cast<LevelMatch*>(getAudioProcessor());
    g.drawFittedText (juce::String(levelMatch->getMatchingGainDb()), getLocalBounds(), juce::Justification::centred, 1);
}

void LevelMatchEditor::resized()
{
    editor.setBounds(getLocalBounds());
    m_gainValueLabel.setBounds(0, 10, 100, 20);
    m_button.setBounds(0, 40, 100, 20);
    m_gainLabel.setBounds (0, 40, 100, 20);
}
