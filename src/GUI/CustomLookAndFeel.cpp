#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    setColour(juce::Slider::backgroundColourId,          bg());
    setColour(juce::Slider::thumbColourId,               gold());
    setColour(juce::Slider::trackColourId,               neonPrimary());
    setColour(juce::Label::textColourId,                 textPrimary());
    setColour(juce::Label::backgroundColourId,           juce::Colours::transparentBlack);
    setColour(juce::TextButton::buttonColourId,          surfaceRaised());
    setColour(juce::TextButton::buttonOnColourId,        neonPrimary());
    setColour(juce::TextButton::textColourOffId,         textSecondary());
    setColour(juce::TextButton::textColourOnId,          textPrimary());
    setColour(juce::ResizableWindow::backgroundColourId, bg());
}

juce::Colour CustomLookAndFeel::morphRingColour(float morph)
{
    // Two-segment: [0..0.5] neonSecondary→neonPrimary, [0.5..1] neonPrimary→gold
    if (morph <= 0.5f)
        return neonSecondary().interpolatedWith(neonPrimary(), morph * 2.0f);
    return neonPrimary().interpolatedWith(gold(), (morph - 0.5f) * 2.0f);
}

//==============================================================================
void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos, float startAngle, float endAngle,
    juce::Slider& /*slider*/)
{
    const float cx = (float)x + (float)width  * 0.5f;
    const float cy = (float)y + (float)height * 0.5f;
    const float r  = (float)juce::jmin(width, height) * 0.5f - 4.0f;

    // Background disc
    g.setColour(surfaceRaised());
    g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);

    // Track arc (dim)
    juce::Path track;
    track.addArc(cx - r + 4, cy - r + 4, (r-4)*2, (r-4)*2, startAngle, endAngle, true);
    g.setColour(neonPrimary().withAlpha(0.25f));
    g.strokePath(track, juce::PathStrokeType(2.0f));

    // Value arc (gold, from start to current)
    const float angle = startAngle + sliderPos * (endAngle - startAngle);
    if (sliderPos > 0.001f)
    {
        juce::Path arc;
        arc.addArc(cx - r + 4, cy - r + 4, (r-4)*2, (r-4)*2, startAngle, angle, true);
        g.setColour(gold());
        g.strokePath(arc, juce::PathStrokeType(2.5f));
    }

    // Outer ring (neon purple glow)
    g.setColour(neonPrimary().withAlpha(0.4f));
    g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.5f);

    // Indicator dot (gold)
    const float dotR = 3.0f;
    const float dotX = cx + (r - 9.0f) * std::sin(angle);
    const float dotY = cy - (r - 9.0f) * std::cos(angle);
    g.setColour(gold());
    g.fillEllipse(dotX - dotR, dotY - dotR, dotR*2, dotR*2);
}

//==============================================================================
void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& btn,
    const juce::Colour& /*bg*/, bool highlighted, bool /*down*/)
{
    auto b = btn.getLocalBounds().toFloat().reduced(0.5f);
    juce::Colour fill = btn.getToggleState() ? neonPrimary() : surfaceRaised();
    if (highlighted) fill = fill.brighter(0.15f);
    g.setColour(fill);
    g.fillRoundedRectangle(b, 3.0f);
    g.setColour(btn.getToggleState() ? neonGlow().withAlpha(0.6f)
                                     : neonPrimary().withAlpha(0.3f));
    g.drawRoundedRectangle(b, 3.0f, 1.0f);
}

void CustomLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& btn,
    bool /*highlighted*/, bool /*down*/)
{
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(btn.getToggleState() ? textPrimary() : textSecondary());
    g.drawText(btn.getButtonText(), btn.getLocalBounds(),
               juce::Justification::centred, false);
}

void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& lbl)
{
    g.setFont(lbl.getFont());
    g.setColour(lbl.findColour(juce::Label::textColourId));
    g.drawText(lbl.getText(), lbl.getLocalBounds().reduced(2, 0),
               lbl.getJustificationType(), true);
}

void CustomLookAndFeel::drawSegmentedGroup(juce::Graphics& g,
    juce::Rectangle<int> bounds, const juce::StringArray& labels,
    int activeIndex, juce::Colour accentColour)
{
    const int n     = labels.size();
    const float segW = (float)bounds.getWidth() / (float)n;
    const float h    = (float)bounds.getHeight();

    for (int i = 0; i < n; ++i)
    {
        juce::Rectangle<float> seg(bounds.getX() + i * segW, bounds.getY(), segW, h);
        bool active = (i == activeIndex);
        g.setColour(active ? accentColour : CustomLookAndFeel::surfaceRaised());
        g.fillRoundedRectangle(seg.reduced(1.0f), 3.0f);
        g.setColour(active ? CustomLookAndFeel::textPrimary()
                           : CustomLookAndFeel::textSecondary());
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        g.drawText(labels[i], seg.toNearestInt(), juce::Justification::centred, false);
    }
    // Outer border
    g.setColour(accentColour.withAlpha(0.4f));
    g.drawRoundedRectangle(bounds.toFloat(), 3.0f, 1.0f);
}
