#pragma once

#include "PluginProcessor.h"

class DisplayedFloatingPointValue : public juce::Component
{
public:
    DisplayedFloatingPointValue(juce::String name, float value, const char* unit)
        : m_unit(unit)
    {
        m_nameLabel.setText(name, juce::dontSendNotification);
        m_valueLabel.setText(juce::String::formatted("%4.1f [%s]", value, unit),
                             juce::dontSendNotification);

        addAndMakeVisible(m_nameLabel);
        addAndMakeVisible(m_valueLabel);
    }

    void setValue(float value)
    {
        m_valueLabel.setText(juce::String::formatted("%4.1f [%s]", value, m_unit),
                             juce::dontSendNotification);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        m_nameLabel.setBounds(area.removeFromLeft(area.getWidth() / 2));
        m_valueLabel.setBounds(area);
    }

private:
    juce::Label m_nameLabel;
    juce::Label m_valueLabel;
    const char* m_unit;
};

class LevelMatchEditor
    : public juce::AudioProcessorEditor
    , public juce::Timer
{
public:
    explicit LevelMatchEditor(LevelMatch&);

private:
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    DisplayedFloatingPointValue m_appliedGainDb;
    DisplayedFloatingPointValue m_inputPowerDb;
    DisplayedFloatingPointValue m_referencePowerDb;

    float value;
};
