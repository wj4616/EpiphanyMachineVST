#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/CustomLookAndFeel.h"
#include "GUI/MorphKnob.h"
#include "GUI/PresetBar.h"

//==============================================================================
// EpiphanyMachineEditor — 950x700, 5-column signal-flow layout.
//
// Signal flow visually: DETECT → GLITCH → DIFFUSION → FILTER → OUTPUT
//==============================================================================
class EpiphanyMachineEditor : public juce::AudioProcessorEditor,
                              private juce::Timer
{
public:
    explicit EpiphanyMachineEditor(EpiphanyMachineProcessor&);
    ~EpiphanyMachineEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateValueLabels();
    void updateFilterButtons();
    void updateTypeButtons(juce::TextButton* btns, const char* paramId);

    EpiphanyMachineProcessor& processor;
    CustomLookAndFeel laf;
    PresetBar presetBar;

    //==========================================================================
    // Reusable knob group: slider + label above + value label below
    //==========================================================================
    struct KnobGroup
    {
        juce::Slider slider;
        juce::Label  label;
        juce::Label  valueLabel;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach;

        void setup(const juce::String& paramId, const juce::String& labelText,
                   juce::AudioProcessorValueTreeState& apvts,
                   juce::LookAndFeel& laf, juce::Component* parent);

        void place(int x, int y, int knobSize, juce::Component* parent);
    };

    // ---- Column 1: Detect + Split ----
    KnobGroup sensitivity, glitchFocus;
    juce::Label glitchFocusSubLabel;

    // ---- Column 2: Glitch Engine ----
    MorphKnob morphKnob;
    juce::Label morphLabel, morphValue;
    juce::AudioProcessorValueTreeState::SliderAttachment morphAttach;
    KnobGroup density, chaos;

    // ---- Column 3: Diffusion + Feedback ----
    KnobGroup decay, damping, width;
    KnobGroup drift, feedback, drive;

    // ---- Column 4: Dual Filter ----
    // Filter position: Off/Wet/Final/Feedback
    juce::TextButton filterPosBtns[4];
    // Texture filter
    juce::TextButton texTypeBtns[3];
    KnobGroup texCutoff, texReso, texLfoRate, texLfoDepth;
    // Body filter
    juce::TextButton bodyTypeBtns[3];
    KnobGroup bodyCutoff, bodyReso;

    // ---- Column 5: Output ----
    KnobGroup mix;
    juce::Label dryWetLabel, stereoOutLabel;

    // ---- Header buttons ----
    juce::TextButton bypassBtn, freezeBtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttach;

    // ---- Website link ----
    juce::HyperlinkButton websiteLink;

    // Column bounds computed in resized(), cached for paint()
    juce::Rectangle<int> col[5];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EpiphanyMachineEditor)
};
