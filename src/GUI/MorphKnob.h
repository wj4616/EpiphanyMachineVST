#pragma once
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

// Large MORPH knob with a colour-reactive outer glow ring.
class MorphKnob : public juce::Slider
{
public:
    MorphKnob();
    void paint(juce::Graphics&) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MorphKnob)
};
