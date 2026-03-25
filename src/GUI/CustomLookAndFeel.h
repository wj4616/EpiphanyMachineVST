#pragma once
#include <JuceHeader.h>

//==============================================================================
// CustomLookAndFeel — cyberpunk dark purple theme with gold accents.
//
// JUCE's LookAndFeel system uses the "Strategy" design pattern: you subclass
// LookAndFeel_V4 and override only the draw methods you want to change.
// Everything else falls back to the base class defaults.
//==============================================================================
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
                          float sliderPos, float startAngle, float endAngle,
                          juce::Slider&) override;

    void drawButtonBackground(juce::Graphics&, juce::Button&,
                              const juce::Colour& bg,
                              bool highlighted, bool down) override;

    void drawButtonText(juce::Graphics&, juce::TextButton&,
                        bool highlighted, bool down) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;

    // ---- Cyberpunk colour palette ----
    static juce::Colour bg()           { return juce::Colour(0xff0d0a1a); }
    static juce::Colour surface()      { return juce::Colour(0xff1a1430); }
    static juce::Colour surfaceRaised(){ return juce::Colour(0xff241e3d); }
    static juce::Colour neonPrimary()  { return juce::Colour(0xffb44aff); }
    static juce::Colour neonSecondary(){ return juce::Colour(0xff7b2ff2); }
    static juce::Colour neonGlow()     { return juce::Colour(0xffd88aff); }
    static juce::Colour gold()         { return juce::Colour(0xfff0c040); }
    static juce::Colour goldDim()      { return juce::Colour(0xffa08030); }
    static juce::Colour textPrimary()  { return juce::Colour(0xffe8e0f0); }
    static juce::Colour textSecondary(){ return juce::Colour(0xff8878a0); }
    static juce::Colour danger()       { return juce::Colour(0xffff3060); }
    static juce::Colour filterAccent() { return juce::Colour(0xff6a3aff); }

    // MORPH ring colour: neonSecondary(0) → neonPrimary(0.5) → gold(1.0)
    static juce::Colour morphRingColour(float morph);

    // Draw a segmented button group in place of individual TextButtons
    static void drawSegmentedGroup(juce::Graphics& g,
                                   juce::Rectangle<int> bounds,
                                   const juce::StringArray& labels,
                                   int activeIndex,
                                   juce::Colour accentColour);
};
