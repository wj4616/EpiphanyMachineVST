#pragma once
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"
#include "../PluginProcessor.h"

//==============================================================================
// PresetBar — shows current preset name with prev/next arrows and morph time.
//
// juce::Button::Listener is an interface (like a Python ABC) that requires
// implementing buttonClicked(). We inherit it to get arrow button callbacks.
//==============================================================================
class PresetBar : public juce::Component,
                  private juce::Button::Listener,
                  private juce::Timer
{
public:
    explicit PresetBar(EpiphanyMachineProcessor&);
    ~PresetBar() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void buttonClicked(juce::Button*) override;
    void timerCallback() override;   // repaint to animate morph pulse

    EpiphanyMachineProcessor& processor;

    juce::TextButton prevBtn { "<" };
    juce::TextButton nextBtn { ">" };
    juce::Label      presetNameLabel;
    juce::Slider     morphTimeSlider;  // small knob, far right
    juce::Label      morphTimeLabel;
    juce::Label      morphTimeValue;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphTimeAttach;

    float pulsePhase = 0.0f;   // used to animate the preset name while morphing
    bool  isMorphing = false;

    int getCurrentPresetIdx() const {
        return (int)processor.apvts.getRawParameterValue("preset")->load();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBar)
};
