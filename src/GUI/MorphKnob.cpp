#include "MorphKnob.h"

MorphKnob::MorphKnob()
{
    setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
}

void MorphKnob::paint(juce::Graphics& g)
{
    const float morph   = (float)getNormalisableRange().convertTo0to1(getValue());
    const auto  ringCol = CustomLookAndFeel::morphRingColour(morph);
    const float cx = (float)getWidth()  * 0.5f;
    const float cy = (float)getHeight() * 0.5f;
    const float ringR = (float)juce::jmin(getWidth(), getHeight()) * 0.5f - 1.0f;

    // Layered glow effect (draw wider/dimmer rings outward first)
    for (int layer = 4; layer >= 1; --layer)
    {
        g.setColour(ringCol.withAlpha(0.06f * layer));
        g.drawEllipse(cx - ringR, cy - ringR, ringR*2, ringR*2,
                      2.0f + layer * 2.0f);
    }
    // Solid ring
    g.setColour(ringCol.withAlpha(0.8f));
    g.drawEllipse(cx - ringR, cy - ringR, ringR*2, ringR*2, 2.0f);

    // Draw the knob itself via the LookAndFeel
    juce::Slider::paint(g);
}
