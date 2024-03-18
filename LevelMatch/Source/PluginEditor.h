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

class LabelledToggle : public juce::Component
{
public:
    LabelledToggle(juce::String name, bool value)
        : m_toggle(name)
    {
        m_toggle.setToggleState(value, juce::dontSendNotification);
        addAndMakeVisible(m_toggle);
        addAndMakeVisible(m_label);
    }

    void resized() override
    {
        auto area = getLocalBounds();
        m_label.setBounds(area.removeFromLeft(area.getWidth() / 2));
        m_toggle.setBounds(area);
    }

    juce::ToggleButton& getButton() { return m_toggle; }
    bool getToggleState() const { return m_toggle.getToggleState(); }
    void setToggleState(bool state, juce::NotificationType notification)
    {
        m_toggle.setToggleState(state, notification);
    }

private:
    juce::ToggleButton m_toggle;
    juce::Label m_label;
};

class LevelMatchEditor
    : public juce::AudioProcessorEditor
    , public juce::Timer
    , public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit LevelMatchEditor(LevelMatch&);

private:
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void parameterChanged(const juce::String& parameterID, float newValue);

    DisplayedFloatingPointValue m_appliedGainDb;
    DisplayedFloatingPointValue m_inputPowerDb;
    DisplayedFloatingPointValue m_referencePowerDb;

    LabelledToggle m_measureInputButton;
    LabelledToggle m_measureReferenceButton;

    LabelledToggle m_applyGainButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_inputButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_referenceButtonAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_applyGainButtonAttachment;

    juce::Label m_debugMessage;
};
