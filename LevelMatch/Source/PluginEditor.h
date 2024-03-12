#pragma once

#include "PluginProcessor.h"

class LevelMatchEditor : public juce::AudioProcessorEditor
{
public:
    explicit LevelMatchEditor(LevelMatch&);

private:
    void paint(juce::Graphics&) override;
    void resized() override;

    juce::GenericAudioProcessorEditor editor;
    juce::Label m_gainLabel;
    juce::Label m_gainValueLabel;
    juce::TextButton m_button;
};
