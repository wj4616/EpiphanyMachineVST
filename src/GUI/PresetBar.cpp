#include "PresetBar.h"
#include "../Presets/PresetData.h"

PresetBar::PresetBar(EpiphanyMachineProcessor& p) : processor(p)
{
    addAndMakeVisible(prevBtn);
    addAndMakeVisible(nextBtn);
    prevBtn.addListener(this);
    nextBtn.addListener(this);

    presetNameLabel.setJustificationType(juce::Justification::centred);
    presetNameLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    presetNameLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::gold());
    addAndMakeVisible(presetNameLabel);

    morphTimeLabel.setText("MORPH TIME", juce::dontSendNotification);
    morphTimeLabel.setFont(juce::Font(9.0f, juce::Font::bold));
    morphTimeLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::textSecondary());
    morphTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(morphTimeLabel);

    morphTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    morphTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(morphTimeSlider);

    morphTimeValue.setFont(juce::Font(9.0f));
    morphTimeValue.setColour(juce::Label::textColourId, CustomLookAndFeel::gold());
    morphTimeValue.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(morphTimeValue);

    morphTimeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.apvts, "morphTime", morphTimeSlider);

    startTimer(80);  // ~12fps for animations
}

PresetBar::~PresetBar()
{
    prevBtn.removeListener(this);
    nextBtn.removeListener(this);
}

void PresetBar::buttonClicked(juce::Button* btn)
{
    int idx = getCurrentPresetIdx();
    if (btn == &prevBtn) idx = (idx - 1 + kNumPresets) % kNumPresets;
    else                  idx = (idx + 1) % kNumPresets;

    // Write preset index to APVTS (processor will detect the change in processBlock)
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
            processor.apvts.getParameter("preset")))
        param->setValueNotifyingHost(param->convertTo0to1(idx));

    isMorphing = true;
    pulsePhase = 0.0f;
}

void PresetBar::timerCallback()
{
    int idx = getCurrentPresetIdx();
    presetNameLabel.setText(kFactoryPresets[idx].name, juce::dontSendNotification);

    float mtVal = processor.apvts.getRawParameterValue("morphTime")->load();
    morphTimeValue.setText(juce::String(mtVal, 1) + "s", juce::dontSendNotification);

    // Check if still morphing (simple proxy: presetManager.isMorphing is private,
    // so we fade the pulse out over time)
    if (isMorphing)
    {
        pulsePhase += 0.08f;
        if (pulsePhase >= juce::MathConstants<float>::twoPi * 3)
            isMorphing = false;
        repaint();
    }
}

void PresetBar::paint(juce::Graphics& g)
{
    g.setColour(CustomLookAndFeel::surface());
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);

    // Pulse the preset name label border during morph
    if (isMorphing)
    {
        float pulse = 0.5f + 0.5f * std::sin(pulsePhase * 2.0f);
        g.setColour(CustomLookAndFeel::gold().withAlpha(0.3f * pulse));
        g.drawRoundedRectangle(presetNameLabel.getBoundsInParent().toFloat(), 3.0f, 1.5f);
    }
}

void PresetBar::resized()
{
    auto b = getLocalBounds().reduced(4);
    prevBtn.setBounds(b.removeFromLeft(32));
    b.removeFromLeft(4);
    nextBtn.setBounds(b.removeFromRight(32));
    b.removeFromRight(4);

    // Morph time on far right: small knob + label
    auto mtArea = b.removeFromRight(80);
    morphTimeLabel.setBounds(mtArea.removeFromTop(12));
    morphTimeSlider.setBounds(mtArea.removeFromLeft(32).withSizeKeepingCentre(28, 28));
    morphTimeValue.setBounds(mtArea);

    presetNameLabel.setBounds(b);
}
