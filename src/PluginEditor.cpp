#include "PluginEditor.h"

//==============================================================================
// KnobGroup helper
//==============================================================================
void EpiphanyMachineEditor::KnobGroup::setup(
    const juce::String& paramId, const juce::String& labelText,
    juce::AudioProcessorValueTreeState& apvts,
    juce::LookAndFeel& lf, juce::Component* parent)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setLookAndFeel(&lf);
    parent->addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, CustomLookAndFeel::textPrimary());
    label.setJustificationType(juce::Justification::centred);
    label.setMinimumHorizontalScale(1.0f);
    parent->addAndMakeVisible(label);

    valueLabel.setFont(juce::Font(9.0f, juce::Font::bold));
    valueLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::gold());
    valueLabel.setJustificationType(juce::Justification::centred);
    parent->addAndMakeVisible(valueLabel);

    attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, paramId, slider);
}

void EpiphanyMachineEditor::KnobGroup::place(int x, int y, int sz, juce::Component*)
{
    // Label is wider than the knob so long names ("LFO DEPTH", "GLITCH FOCUS", etc.)
    // are never truncated.  Centre the wider label over the knob centre.
    const int labelW = sz + 40;
    const int labelX = x + sz / 2 - labelW / 2;
    label.setBounds(labelX, y, labelW, 13);
    slider.setBounds(x, y + 13, sz, sz);
    valueLabel.setBounds(labelX, y + 13 + sz, labelW, 12);
}

//==============================================================================
// Constructor
//==============================================================================
EpiphanyMachineEditor::EpiphanyMachineEditor(EpiphanyMachineProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      presetBar(p),
      morphAttach(p.apvts, "morph", morphKnob)
{
    setSize(950, 580);
    setLookAndFeel(&laf);

    // ---- Preset bar ----
    presetBar.setLookAndFeel(&laf);
    addAndMakeVisible(presetBar);

    // ---- Header buttons ----
    bypassBtn.setButtonText("BYPASS");
    bypassBtn.setClickingTogglesState(true);
    bypassBtn.setColour(juce::TextButton::buttonOnColourId, CustomLookAndFeel::danger());
    addAndMakeVisible(bypassBtn);
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        p.apvts, "bypass", bypassBtn);

    freezeBtn.setButtonText("FREEZE");
    freezeBtn.setClickingTogglesState(true);
    freezeBtn.setColour(juce::TextButton::buttonColourId,  CustomLookAndFeel::goldDim());
    freezeBtn.setColour(juce::TextButton::buttonOnColourId,CustomLookAndFeel::gold());
    freezeBtn.setColour(juce::TextButton::textColourOffId, CustomLookAndFeel::textSecondary());
    freezeBtn.setColour(juce::TextButton::textColourOnId,  juce::Colour(0xff1a1430));
    addAndMakeVisible(freezeBtn);
    freezeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        p.apvts, "freeze", freezeBtn);

    // ---- Col 1: MORPH knob (special) ----
    morphKnob.setRange(0.0, 1.0);
    addAndMakeVisible(morphKnob);
    morphLabel.setText("MORPH", juce::dontSendNotification);
    morphLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    morphLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::textPrimary());
    morphLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(morphLabel);
    morphValue.setFont(juce::Font(10.0f, juce::Font::bold));
    morphValue.setColour(juce::Label::textColourId, CustomLookAndFeel::gold());
    morphValue.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(morphValue);

    // ---- Col 1: Detect ----
    sensitivity.setup("sensitivity",  "SENSITIVITY",  p.apvts, laf, this);
    glitchFocus.setup("glitchFocus",  "GLITCH FOCUS", p.apvts, laf, this);
    glitchFocusSubLabel.setText("BODY  < >  TEXTURE",
                                 juce::dontSendNotification);
    glitchFocusSubLabel.setFont(juce::Font(9.0f));
    glitchFocusSubLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::textSecondary());
    glitchFocusSubLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(glitchFocusSubLabel);

    // ---- Col 2: Glitch ----
    density.setup("density", "DENSITY", p.apvts, laf, this);
    chaos.setup("chaos",     "CHAOS",   p.apvts, laf, this);

    // ---- Col 3: Diffusion ----
    decay.setup("decay",     "DECAY",    p.apvts, laf, this);
    damping.setup("damping", "DAMPING",  p.apvts, laf, this);
    width.setup("width",     "WIDTH",    p.apvts, laf, this);
    drift.setup("drift",     "DRIFT",    p.apvts, laf, this);
    feedback.setup("feedback","FEEDBACK",p.apvts, laf, this);

    // ---- Col 4: Dual Filter ----
    // Filter position buttons
    juce::StringArray fpLabels{"OFF","WET","FINAL","FB"};
    for (int i = 0; i < 4; ++i)
    {
        filterPosBtns[i].setButtonText(fpLabels[i]);
        filterPosBtns[i].setClickingTogglesState(false);
        filterPosBtns[i].setLookAndFeel(&laf);
        filterPosBtns[i].onClick = [this, i]() {
            if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
                    processor.apvts.getParameter("filterPos")))
                param->setValueNotifyingHost(param->convertTo0to1(i));
            updateFilterButtons();
        };
        addAndMakeVisible(filterPosBtns[i]);
    }

    // ---- Col 3: Drive (in feedback loop) ----
    drive.setup("drive", "DRIVE", p.apvts, laf, this);

    // Texture type buttons
    juce::StringArray typeLabels{"LP","HP","BP"};
    for (int i = 0; i < 3; ++i)
    {
        texTypeBtns[i].setButtonText(typeLabels[i]);
        texTypeBtns[i].setClickingTogglesState(false);
        texTypeBtns[i].setLookAndFeel(&laf);
        texTypeBtns[i].onClick = [this, i]() {
            if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
                    processor.apvts.getParameter("texFilterType")))
                param->setValueNotifyingHost(param->convertTo0to1(i));
        };
        addAndMakeVisible(texTypeBtns[i]);

        bodyTypeBtns[i].setButtonText(typeLabels[i]);
        bodyTypeBtns[i].setClickingTogglesState(false);
        bodyTypeBtns[i].setLookAndFeel(&laf);
        bodyTypeBtns[i].onClick = [this, i]() {
            if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
                    processor.apvts.getParameter("bodyFilterType")))
                param->setValueNotifyingHost(param->convertTo0to1(i));
        };
        addAndMakeVisible(bodyTypeBtns[i]);
    }

    texCutoff.setup("texFilterCutoff", "CUTOFF",    p.apvts, laf, this);
    texReso.setup  ("texFilterReso",   "RESO",      p.apvts, laf, this);
    texLfoRate.setup("texLfoRate",     "LFO RATE",  p.apvts, laf, this);
    texLfoDepth.setup("texLfoDepth",   "LFO DEPTH", p.apvts, laf, this);
    bodyCutoff.setup("bodyFilterCutoff","CUTOFF",   p.apvts, laf, this);
    bodyReso.setup ("bodyFilterReso",  "RESO",      p.apvts, laf, this);

    // ---- Col 5: Output ----
    mix.setup("mix", "MIX", p.apvts, laf, this);
    dryWetLabel.setText("DRY  < >  WET",
                        juce::dontSendNotification);
    dryWetLabel.setFont(juce::Font(9.0f));
    dryWetLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::textSecondary());
    dryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryWetLabel);
    stereoOutLabel.setText(">> STEREO OUT", juce::dontSendNotification);
    stereoOutLabel.setFont(juce::Font(9.0f, juce::Font::bold));
    stereoOutLabel.setColour(juce::Label::textColourId, CustomLookAndFeel::neonPrimary());
    stereoOutLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stereoOutLabel);

    // ---- Website link ----
    websiteLink.setButtonText("Support EpiphanyMachineArt.com");
    websiteLink.setURL(juce::URL("https://epiphanymachineart.com"));
    websiteLink.setFont(juce::Font(13.0f, juce::Font::bold), false);
    websiteLink.setColour(juce::HyperlinkButton::textColourId, CustomLookAndFeel::gold());
    addAndMakeVisible(websiteLink);

    startTimer(66);  // ~15fps for value labels + button state
}

EpiphanyMachineEditor::~EpiphanyMachineEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void EpiphanyMachineEditor::timerCallback()
{
    updateValueLabels();
    updateFilterButtons();
    morphKnob.repaint();

    // Update type button states
    updateTypeButtons(texTypeBtns,  "texFilterType");
    updateTypeButtons(bodyTypeBtns, "bodyFilterType");
}

void EpiphanyMachineEditor::updateTypeButtons(juce::TextButton* btns, const char* paramId)
{
    int val = (int)processor.apvts.getRawParameterValue(paramId)->load();
    for (int i = 0; i < 3; ++i)
        btns[i].setToggleState(i == val, juce::dontSendNotification);
}

void EpiphanyMachineEditor::updateFilterButtons()
{
    int fp = (int)processor.apvts.getRawParameterValue("filterPos")->load();
    for (int i = 0; i < 4; ++i)
        filterPosBtns[i].setToggleState(i == fp, juce::dontSendNotification);
}

void EpiphanyMachineEditor::updateValueLabels()
{
    auto getV = [&](const char* id) {
        return processor.apvts.getRawParameterValue(id)->load();
    };

    auto fmtHz = [](float hz) -> juce::String {
        return hz >= 1000.0f ? juce::String(hz/1000.0f, 1) + "k"
                             : juce::String((int)hz) + "Hz";
    };

    sensitivity.valueLabel.setText(juce::String(getV("sensitivity"), 2), juce::dontSendNotification);
    glitchFocus.valueLabel.setText(juce::String(getV("glitchFocus"), 2), juce::dontSendNotification);
    morphValue.setText(juce::String(getV("morph"), 2), juce::dontSendNotification);
    density.valueLabel.setText(juce::String(getV("density"), 2), juce::dontSendNotification);
    chaos.valueLabel.setText(juce::String(getV("chaos"), 2), juce::dontSendNotification);
    decay.valueLabel.setText(juce::String(getV("decay"), 2), juce::dontSendNotification);
    damping.valueLabel.setText(juce::String(getV("damping"), 2), juce::dontSendNotification);
    width.valueLabel.setText(juce::String(getV("width"), 2), juce::dontSendNotification);
    drift.valueLabel.setText(juce::String(getV("drift"), 1), juce::dontSendNotification);
    feedback.valueLabel.setText(juce::String(getV("feedback"), 2), juce::dontSendNotification);
    drive.valueLabel.setText(juce::String(getV("drive"), 2), juce::dontSendNotification);
    texCutoff.valueLabel.setText(fmtHz(getV("texFilterCutoff")), juce::dontSendNotification);
    texReso.valueLabel.setText(juce::String(getV("texFilterReso"), 2), juce::dontSendNotification);
    texLfoRate.valueLabel.setText(juce::String(getV("texLfoRate"), 1)+"Hz", juce::dontSendNotification);
    texLfoDepth.valueLabel.setText(juce::String(getV("texLfoDepth"), 2), juce::dontSendNotification);
    bodyCutoff.valueLabel.setText(fmtHz(getV("bodyFilterCutoff")), juce::dontSendNotification);
    bodyReso.valueLabel.setText(juce::String(getV("bodyFilterReso"), 2), juce::dontSendNotification);
    mix.valueLabel.setText(juce::String(getV("mix"), 2), juce::dontSendNotification);
}

//==============================================================================
void EpiphanyMachineEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(CustomLookAndFeel::bg());

    // Subtle gradient overlay (top gets a faint purple lift)
    juce::ColourGradient grad(CustomLookAndFeel::neonSecondary().withAlpha(0.08f), 0, 0,
                              CustomLookAndFeel::bg(), 0, (float)getHeight(), false);
    g.setGradientFill(grad);
    g.fillAll();

    // Title
    g.setColour(CustomLookAndFeel::textPrimary());
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("EPIPHANY MACHINE", 12, 8, 400, 32, juce::Justification::centredLeft);
    g.setFont(juce::Font(9.0f));
    g.setColour(CustomLookAndFeel::gold().withAlpha(0.5f));
    g.drawText("v3", getWidth() - 36, 8, 28, 14, juce::Justification::centredRight);

    // Signal flow indicator strip
    const int sfY = 104;
    g.setColour(CustomLookAndFeel::textSecondary());
    g.setFont(juce::Font(9.0f));
    g.drawText("SIGNAL FLOW", 12, sfY, 90, 12, juce::Justification::centredLeft);
    g.drawText("INPUT >> DETECT >> GLITCH >> DIFFUSE >> FILTER >> OUTPUT",
               110, sfY, 700, 12, juce::Justification::centredLeft);
    g.setColour(CustomLookAndFeel::neonPrimary().withAlpha(0.2f));
    g.drawLine(12, sfY + 12, (float)getWidth() - 12, sfY + 12, 1.0f);

    // Draw column backgrounds (semi-transparent surface panels with rounded corners)
    for (int c = 0; c < 5; ++c)
    {
        juce::Colour bg = (c == 3) ? CustomLookAndFeel::filterAccent().withAlpha(0.07f)
                                   : CustomLookAndFeel::surface().withAlpha(0.8f);
        g.setColour(bg);
        g.fillRoundedRectangle(col[c].toFloat(), 6.0f);
        juce::Colour border = (c == 3) ? CustomLookAndFeel::filterAccent().withAlpha(0.3f)
                                       : CustomLookAndFeel::neonPrimary().withAlpha(0.12f);
        g.setColour(border);
        g.drawRoundedRectangle(col[c].toFloat(), 6.0f, 1.0f);
    }

    // Column header labels
    const char* headers[] = { "1  DETECT", "2  GLITCH", "3  DIFFUSE + FEEDBACK",
                               "4  DUAL FILTER", "5  OUTPUT" };
    for (int c = 0; c < 5; ++c)
    {
        juce::Colour hc = (c == 3) ? CustomLookAndFeel::filterAccent()
                                   : CustomLookAndFeel::neonPrimary();
        g.setColour(hc.withAlpha(0.8f));
        g.setFont(juce::Font(9.0f, juce::Font::bold));
        g.drawText(headers[c], col[c].getX() + 6, col[c].getY() + 6,
                   col[c].getWidth() - 8, 12, juce::Justification::centredLeft);
    }

    // "FEEDBACK LOOP" sub-panel outline in col 3 (matches resized() fbTop=+170)
    const int fbPanelY = col[2].getY() + 158;
    const int fbPanelH = 160;
    juce::Rectangle<int> fbR(col[2].getX() + 4, fbPanelY, col[2].getWidth() - 8, fbPanelH);
    g.setColour(CustomLookAndFeel::neonSecondary().withAlpha(0.15f));
    g.fillRoundedRectangle(fbR.toFloat(), 4.0f);
    g.setColour(CustomLookAndFeel::neonSecondary().withAlpha(0.4f));
    g.drawRoundedRectangle(fbR.toFloat(), 4.0f, 1.0f);
    g.setFont(juce::Font(8.0f));
    g.setColour(CustomLookAndFeel::neonSecondary().withAlpha(0.7f));
    g.drawText("FEEDBACK LOOP", fbR.getX() + 4, fbR.getY() + 3,
               fbR.getWidth() - 4, 10, juce::Justification::centredLeft);

    // Filter sub-panels  -  positions match resized() texTop=+48, compact knob rows
    const auto& fc = col[3];
    const int texPanelY = fc.getY() + 44;
    // texTop=48, kRow1=+34, ks=44, kRow2=34+44+22=100, so bottom=100+44+12=156, +margin
    const int texPanelH = 202;
    juce::Rectangle<float> texPanel(fc.getX() + 4.0f, texPanelY,
                                     fc.getWidth() - 8.0f, texPanelH);
    g.setColour(CustomLookAndFeel::filterAccent().withAlpha(0.12f));
    g.fillRoundedRectangle(texPanel, 4.0f);
    g.setColour(CustomLookAndFeel::filterAccent().withAlpha(0.4f));
    g.drawRoundedRectangle(texPanel, 4.0f, 1.0f);
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(CustomLookAndFeel::filterAccent());
    g.drawText("TEXTURE (A)", (int)texPanel.getX() + 4, (int)texPanel.getY() + 3,
               (int)texPanel.getWidth() - 4, 10, juce::Justification::centredLeft);

    // Body panel  -  starts just below texture panel
    const int bodPanelY = texPanelY + texPanelH + 4;
    const int bodPanelH = 100;
    juce::Rectangle<float> bodPanel(fc.getX() + 4.0f, bodPanelY,
                                     fc.getWidth() - 8.0f, bodPanelH);
    g.setColour(CustomLookAndFeel::neonSecondary().withAlpha(0.12f));
    g.fillRoundedRectangle(bodPanel, 4.0f);
    g.setColour(CustomLookAndFeel::neonSecondary().withAlpha(0.4f));
    g.drawRoundedRectangle(bodPanel, 4.0f, 1.0f);
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(CustomLookAndFeel::neonPrimary());
    g.drawText("BODY (B)", (int)bodPanel.getX() + 4, (int)bodPanel.getY() + 3,
               (int)bodPanel.getWidth() - 4, 10, juce::Justification::centredLeft);
}

//==============================================================================
void EpiphanyMachineEditor::resized()
{
    // Window is 950x580.  Header takes ~108px, leaving ~460px for columns.
    // Compactness comes from reducing padding/spacing  -  font and knob sizes unchanged.

    // --- Header row (tighter vertical packing) ---
    presetBar.setBounds(8, 42, getWidth() - 16, 40);  // was y=46, h=46
    bypassBtn.setBounds(getWidth() - 148, 6, 68, 26);
    freezeBtn.setBounds(getWidth() - 76,  6, 68, 26);

    // --- Column geometry ---
    const int colY = 106;                             // was 118  -  12px saved
    const int colH = getHeight() - colY - 18;        // fills to near-bottom
    const int gap  = 6;
    const int lpad = 8;

    int x = lpad;
    int widths[5] = { 162, 172, 218, 218, 142 };
    for (int c = 0; c < 5; ++c)
    {
        col[c] = { x, colY, widths[c], colH };
        x += widths[c] + gap;
    }

    // ---- Col 1: DETECT ----
    {
        const auto& c = col[0];
        const int cx = c.getX() + c.getWidth()/2;
        const int ks = 60;
        sensitivity.place(cx - ks/2, c.getY() + 22, ks, this);  // was +30
        glitchFocus.place(cx - ks/2, c.getY() + 108, ks, this); // was +125
        glitchFocusSubLabel.setBounds(c.getX() + 4, c.getY() + 178, c.getWidth() - 8, 12);
    }

    // ---- Col 2: GLITCH ----
    {
        const auto& c = col[1];
        const int cx = c.getX() + c.getWidth()/2;
        const int morphSz = 54;                      // was 80  -  still largest knob
        morphLabel.setBounds(cx - 40, c.getY() + 16, 80, 13);   // was +22
        morphKnob.setBounds(cx - morphSz/2, c.getY() + 30, morphSz, morphSz); // was +36
        morphValue.setBounds(cx - 30, c.getY() + 30 + morphSz, 60, 12);

        const int ks = 52;
        const int row2y = c.getY() + 120;           // was +148
        density.place(c.getX() + 12, row2y, ks, this);
        chaos.place(c.getX() + c.getWidth() - 12 - ks, row2y, ks, this);
    }

    // ---- Col 3: DIFFUSION + FEEDBACK ----
    {
        const auto& c = col[2];
        const int ks = 52;
        const int top = c.getY() + 20;              // was +26
        const int cx  = c.getX() + c.getWidth()/2;

        const int spacing = (c.getWidth() - 8) / 3;
        decay.place(c.getX() + 4,              top, ks, this);
        damping.place(c.getX() + 4 + spacing,  top, ks, this);
        width.place(c.getX() + 4 + spacing*2,  top, ks, this);

        const int fbTop = c.getY() + 170;           // was +200
        drift.place(c.getX() + 10, fbTop, ks, this);
        feedback.place(c.getX() + c.getWidth() - 10 - ks, fbTop, ks, this);
        drive.place(cx - ks/2, fbTop + ks + 28, ks, this);
    }

    // ---- Col 4: DUAL FILTER ----
    {
        const auto& c = col[3];
        const int btnH = 20;                         // was 22
        const int btnW = (c.getWidth() - 8) / 4;
        for (int i = 0; i < 4; ++i)
            filterPosBtns[i].setBounds(c.getX() + 4 + i*btnW, c.getY() + 20, btnW - 2, btnH);

        const int texTop = c.getY() + 48;            // was +58
        const int tyBtnW = (c.getWidth() - 12) / 3;
        for (int i = 0; i < 3; ++i)
            texTypeBtns[i].setBounds(c.getX() + 6 + i*tyBtnW, texTop + 10, tyBtnW - 2, 17);

        const int ks = 44;                           // was 48
        const int kRow1 = texTop + 34;               // was +38
        const int kRow2 = texTop + 34 + ks + 22;    // was +38+48+28
        const int hx = c.getX() + c.getWidth()/2;
        texCutoff.place(hx - ks - 4, kRow1, ks, this);
        texReso.place(hx + 4,         kRow1, ks, this);
        texLfoRate.place(hx - ks - 4, kRow2, ks, this);
        texLfoDepth.place(hx + 4,     kRow2, ks, this);

        // Body panel starts right after texture knobs
        const int bodTop = texTop + 34 + ks + 22 + ks + 26; // kRow2 + ks + gap
        for (int i = 0; i < 3; ++i)
            bodyTypeBtns[i].setBounds(c.getX() + 6 + i*tyBtnW, bodTop + 10, tyBtnW - 2, 17);
        const int bkRow = bodTop + 34;
        bodyCutoff.place(hx - ks - 4, bkRow, ks, this);
        bodyReso.place(hx + 4,         bkRow, ks, this);
    }

    // ---- Col 5: OUTPUT ----
    {
        const auto& c = col[4];
        const int cx = c.getX() + c.getWidth()/2;
        const int ks = 70;                           // MIX knob stays large
        mix.place(cx - ks/2, c.getY() + 22, ks, this);  // was +30
        dryWetLabel.setBounds(c.getX() + 4, c.getY() + 22 + ks + 22, c.getWidth() - 8, 12);
        stereoOutLabel.setBounds(c.getX() + 4, c.getBottom() - 24, c.getWidth() - 8, 16);
    }

    // Website link  -  bottom-right, larger now (13pt bold gold)
    websiteLink.setBounds(getWidth() - 294, getHeight() - 20, 286, 18);
}

