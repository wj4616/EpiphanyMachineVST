#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
EpiphanyMachineProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto addFloat = [&](const char* id, const char* name,
                        float mn, float mx, float def, float skew = 1.0f)
    {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            id, name, juce::NormalisableRange<float>(mn, mx, 0.001f, skew), def));
    };

    auto addChoice = [&](const char* id, const char* name,
                         juce::StringArray choices, int def = 0)
    {
        params.push_back(std::make_unique<juce::AudioParameterChoice>(id, name, choices, def));
    };

    auto addBool = [&](const char* id, const char* name, bool def = false)
    {
        params.push_back(std::make_unique<juce::AudioParameterBool>(id, name, def));
    };

    // --- Original Alien Machine params (IDs preserved exactly) ---
    addFloat("morph",    "Morph",    0.0f,   1.0f,   0.3f);
    addFloat("mix",      "Mix",      0.0f,   1.0f,   0.5f);
    addFloat("density",  "Density",  0.0f,   1.0f,   0.3f);
    addFloat("chaos",    "Chaos",    0.0f,   1.0f,   0.2f);
    addFloat("decay",    "Decay",    0.0f,   1.0f,   0.6f, 0.5f);
    addFloat("damping",  "Damping",  0.0f,   1.0f,   0.5f);
    addFloat("drift",    "Drift",    -12.0f, 12.0f,  0.0f);
    addFloat("width",    "Width",    0.0f,   1.0f,   0.6f);
    addFloat("feedback", "Feedback", 0.0f,   0.85f,  0.3f, 0.5f);

    // --- Epiphany additions ---
    addBool("bypass",  "Bypass");
    addBool("freeze",  "Freeze");
    addFloat("glitchFocus",  "Glitch Focus",  0.0f, 1.0f,   0.5f);
    addFloat("sensitivity",  "Sensitivity",   0.0f, 1.0f,   0.5f);

    // juce::AudioParameterChoice stores values as 0-based index integers.
    addChoice("filterPos",    "Filter Position", {"Off","Wet","Final"}, 0);
    addChoice("texFilterType","Tex Filter Type",  {"LP","HP","BP"},     0);
    // 0.3 skew = logarithmic feel on frequency knobs
    addFloat("texFilterCutoff", "Tex Cutoff",  20.0f, 20000.0f, 2000.0f, 0.3f);
    addFloat("texFilterReso",   "Tex Reso",    0.0f,  1.0f,     0.3f);
    addFloat("texLfoRate",      "Tex LFO Rate",0.01f, 10.0f,    0.5f,  0.3f);
    addFloat("texLfoDepth",     "Tex LFO Depth",0.0f, 1.0f,    0.0f);
    addChoice("bodyFilterType","Body Filter Type", {"LP","HP","BP"},    0);
    addFloat("bodyFilterCutoff","Body Cutoff", 20.0f, 20000.0f, 1200.0f, 0.3f);
    addFloat("bodyFilterReso",  "Body Reso",   0.0f,  1.0f,    0.2f);

    // Preset system — 34 slots: INIT + 33 factory presets
    addChoice("preset", "Preset",
              {"INIT",
               "Void Meditation","Eternal Hall","Cathedral of Dust","Weightless","Frozen Lake",
               "Phantom Drift","Descending Angel","Fifth Dimension","Dissonant Cloud",
               "Tritone Abyss","Solar Flare",
               "Neural Storm","Broken Transmission","Scattered Memories","Stutter Prayer",
               "Insect Choir","Living Texture",
               "Deep Submersion","Tectonic Drone","Obsidian Mirror","Gravity Well","Midnight Fog",
               "Crystal Lattice","Bell Garden","Ice Fracture","Singing Metal","Prism",
               "Event Horizon","Ghost in the Machine","Parallel Universe",
               "Temporal Collapse","The Long Exhale","Machine Awakening"}, 0);
    addFloat("morphTime", "Morph Time", 0.1f, 10.0f, 2.0f);

    return { params.begin(), params.end() };
}

//==============================================================================
EpiphanyMachineProcessor::EpiphanyMachineProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "EpiphanyMachineState", createParameterLayout())
{
    presetManager.initialise(apvts);
}

//==============================================================================
bool EpiphanyMachineProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
    auto in = layouts.getMainInputChannelSet();
    return in == juce::AudioChannelSet::stereo() || in == juce::AudioChannelSet::mono();
}

//==============================================================================
void EpiphanyMachineProcessor::prepareToPlay(double sr, int spb)
{
    glitchEngine.prepare(sr, spb);
    diffusionNetwork.prepare(sr, spb);
    pitchDrifter.prepare(sr, spb);
    stereoWidener.prepare(sr, spb);
    inputDetector.prepare(sr, spb);
    filterDetector.prepare(sr, spb);
    dualFilter.prepare(sr, spb);

    // Allocate working buffers
    dryBuffer.setSize(2, spb);
    glitchInputBuffer.setSize(2, spb);
    cleanPassBuffer.setSize(2, spb);
    wetBuffer.setSize(2, spb);
    filteredBuffer.setSize(2, spb);
    processedBuffer.setSize(2, spb);
    fbBuf.setSize(2, 1);

    // Smoothers — 20ms ramp for most params
    const double sm = 0.02;
    morphSmoothed.reset(sr, sm);
    mixSmoothed.reset(sr, sm);
    feedbackSmoothed.reset(sr, sm);
    glitchFocusSmoothed.reset(sr, sm);
    bypassSmoothed.reset(sr, 512.0 / sr);  // ~10ms bypass crossfade

    auto getV = [&](const char* id) { return apvts.getRawParameterValue(id)->load(); };
    morphSmoothed.setCurrentAndTargetValue(getV("morph"));
    mixSmoothed.setCurrentAndTargetValue(getV("mix"));
    feedbackSmoothed.setCurrentAndTargetValue(getV("feedback"));
    glitchFocusSmoothed.setCurrentAndTargetValue(getV("glitchFocus"));
    bypassSmoothed.setCurrentAndTargetValue(getV("bypass") > 0.5f ? 1.0f : 0.0f);

    // Feedback path LPF ~8kHz fixed
    fbLpfCoeff = std::exp(-2.0f * juce::MathConstants<float>::pi *
                          8000.0f / (float)sr);
    feedbackL = feedbackR = fbLpfStateL = fbLpfStateR = 0.0f;
}

//==============================================================================
void EpiphanyMachineProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer&)
{
    // Flush denormal floats to zero — prevents CPU spike from near-zero values
    // in long reverb tails.  Also see the 1e-18 offset in DiffusionNetwork.
    juce::ScopedNoDenormals noDenormals;

    const int N = buffer.getNumSamples();

    // Mono-to-stereo upmix
    if (buffer.getNumChannels() == 1)
        buffer.copyFrom(1, 0, buffer, 0, 0, N);

    // Save dry signal before any processing
    dryBuffer.copyFrom(0, 0, buffer, 0, 0, N);
    dryBuffer.copyFrom(1, 0, buffer, 1, 0, N);

    //==========================================================================
    // Update parameter targets
    //==========================================================================
    auto getV = [&](const char* id) { return apvts.getRawParameterValue(id)->load(); };

    const bool  isBypassed  = getV("bypass")  > 0.5f;
    const bool  isFrozen    = getV("freeze")  > 0.5f;
    const int   filterPos   = (int)getV("filterPos");   // read ONCE per block
    const int   presetIdx   = (int)getV("preset");
    const float morphTimeSec = getV("morphTime");

    bypassSmoothed.setTargetValue(isBypassed ? 1.0f : 0.0f);
    morphSmoothed.setTargetValue(getV("morph"));
    mixSmoothed.setTargetValue(getV("mix"));
    feedbackSmoothed.setTargetValue(getV("feedback"));
    glitchFocusSmoothed.setTargetValue(getV("glitchFocus"));

    // Push single-value params to DSP objects
    glitchEngine.setDensity(getV("density"));
    glitchEngine.setChaos(getV("chaos"));
    diffusionNetwork.setDecay(getV("decay"));
    diffusionNetwork.setDamping(getV("damping"));
    diffusionNetwork.setFreezeActive(isFrozen);
    pitchDrifter.setDrift(getV("drift"));
    stereoWidener.setWidth(getV("width"));
    inputDetector.setSensitivity(getV("sensitivity"));
    filterDetector.setSensitivity(getV("sensitivity"));
    dualFilter.setTexFilterType((int)getV("texFilterType"));
    dualFilter.setTexCutoff(getV("texFilterCutoff"));
    dualFilter.setTexReso(getV("texFilterReso"));
    dualFilter.setTexLfoRate(getV("texLfoRate"));
    dualFilter.setTexLfoDepth(getV("texLfoDepth"));
    dualFilter.setBodyFilterType((int)getV("bodyFilterType"));
    dualFilter.setBodyCutoff(getV("bodyFilterCutoff"));
    dualFilter.setBodyReso(getV("bodyFilterReso"));

    // Advance preset morph (writes interpolated values to APVTS each block)
    // Note: presetManager listens for the "preset" parameter change in Editor,
    //       so we check whether a new selection was made.
    static int lastPreset = 0;
    if (presetIdx != lastPreset) {
        presetManager.selectPreset(presetIdx, morphTimeSec);
        lastPreset = presetIdx;
    }
    presetManager.advance(getSampleRate(), N);

    //==========================================================================
    // Fast bypass: if fully bypassed, skip ALL DSP and just output dry.
    // We still need to advance smoothers to keep them in sync.
    //==========================================================================
    if (bypassSmoothed.getCurrentValue() >= 0.999f && isBypassed)
    {
        // Advance smoothers silently so they don't jump when we un-bypass
        for (int i = 0; i < N; ++i)
        {
            morphSmoothed.getNextValue();
            mixSmoothed.getNextValue();
            feedbackSmoothed.getNextValue();
            glitchFocusSmoothed.getNextValue();
            bypassSmoothed.getNextValue();
        }
        // Output = dry signal unchanged
        buffer.copyFrom(0, 0, dryBuffer, 0, 0, N);
        buffer.copyFrom(1, 0, dryBuffer, 1, 0, N);
        return;
    }

    //==========================================================================
    // Step 1: Run input detector on raw input
    //==========================================================================
    inputDetector.process(buffer, N);
    const auto& texBuf  = inputDetector.getTextureBuffer();
    const auto& bodBuf  = inputDetector.getBodyBuffer();

    //==========================================================================
    // Step 2: Build Glitch Engine input via GLITCH FOCUS routing.
    //
    // glitchFocus=0.0: body feeds glitch, texture passes clean
    // glitchFocus=0.5: full signal feeds glitch (original Alien Machine)
    // glitchFocus=1.0: texture feeds glitch, body passes clean
    //==========================================================================
    {
        auto* giL = glitchInputBuffer.getWritePointer(0);
        auto* giR = glitchInputBuffer.getWritePointer(1);
        auto* cpL = cleanPassBuffer.getWritePointer(0);
        auto* cpR = cleanPassBuffer.getWritePointer(1);
        const auto* inL = buffer.getReadPointer(0);
        const auto* inR = buffer.getReadPointer(1);
        const auto* tL  = texBuf.getReadPointer(0);
        const auto* tR  = texBuf.getReadPointer(1);
        const auto* bL  = bodBuf.getReadPointer(0);
        const auto* bR  = bodBuf.getReadPointer(1);

        for (int i = 0; i < N; ++i)
        {
            const float gf = glitchFocusSmoothed.getNextValue();

            if (gf <= 0.5f)
            {
                float t = gf * 2.0f;               // 0.0–0.5 → 0–1
                giL[i] = bL[i] * (1.0f - t) + inL[i] * t;
                giR[i] = bR[i] * (1.0f - t) + inR[i] * t;
                cpL[i] = tL[i] * (1.0f - t);      // texture not glitched
                cpR[i] = tR[i] * (1.0f - t);
            }
            else
            {
                float t = (gf - 0.5f) * 2.0f;     // 0.5–1.0 → 0–1
                giL[i] = inL[i] * (1.0f - t) + tL[i] * t;
                giR[i] = inR[i] * (1.0f - t) + tR[i] * t;
                cpL[i] = bL[i] * t;               // body not glitched
                cpR[i] = bR[i] * t;
            }
        }
    }

    //==========================================================================
    // Step 3: Run Glitch Engine on the focused input
    //==========================================================================
    glitchEngine.process(glitchInputBuffer);  // in-place

    //==========================================================================
    // Step 4: Per-sample MORPH + feedback summing → diffusion input
    //==========================================================================
    {
        auto* outL = buffer.getWritePointer(0);
        auto* outR = buffer.getWritePointer(1);
        const auto* giL = glitchInputBuffer.getReadPointer(0);
        const auto* giR = glitchInputBuffer.getReadPointer(1);
        const auto* cpL = cleanPassBuffer.getReadPointer(0);
        const auto* cpR = cleanPassBuffer.getReadPointer(1);
        // glitchInputBuffer now contains the GLITCHED signal (processed in-place)
        // The pre-glitch version is gone — but the MORPH blend uses it and clean input.
        // NOTE: Because GlitchEngine processes in-place, giL/R is now glitch output.
        // The "clean" version of the glitch input (i.e. original focused input) was
        // already summed into cleanPass at the right ratio above.

        for (int i = 0; i < N; ++i)
        {
            const float morph = morphSmoothed.getNextValue();
            feedbackSmoothed.getNextValue(); // advance smoother (used in step 6)

            // cleanPass is the portion of input not sent to glitch engine
            // giL is glitch engine OUTPUT of the focused portion
            // At morph=0: use cleanPass only (no glitch effect on the focused portion)
            // At morph=1: use cleanPass + full glitch output
            float dL = cpL[i] + giL[i] * morph;
            float dR = cpR[i] + giR[i] * morph;

            // Add feedback return from previous block
            dL += feedbackL;
            dR += feedbackR;

            outL[i] = dL;
            outR[i] = dR;
        }
    }

    //==========================================================================
    // Step 5: Diffusion Network (FDN reverb) + Stereo Widener
    //==========================================================================
    diffusionNetwork.process(buffer);
    stereoWidener.process(buffer);

    // Copy wet (post-widener) into wetBuffer
    wetBuffer.copyFrom(0, 0, buffer, 0, 0, N);
    wetBuffer.copyFrom(1, 0, buffer, 1, 0, N);

    //==========================================================================
    // Step 6: Feedback path — last sample → PitchDrifter → LPF → clip → next block
    //==========================================================================
    {
        const float fbGain = feedbackSmoothed.getCurrentValue();
        float fbSendL = wetBuffer.getSample(0, N-1) * fbGain;
        float fbSendR = wetBuffer.getSample(1, N-1) * fbGain;

        // PitchDrifter processes a 1-sample buffer (granular pitch shifting)
        fbBuf.setSample(0, 0, fbSendL);
        fbBuf.setSample(1, 0, fbSendR);
        pitchDrifter.process(fbBuf);
        fbSendL = fbBuf.getSample(0, 0);
        fbSendR = fbBuf.getSample(1, 0);

        // Fixed 8kHz LPF — prevents harsh high-frequency buildup in feedback loop
        fbLpfStateL = (1.0f - fbLpfCoeff) * fbSendL + fbLpfCoeff * fbLpfStateL;
        fbLpfStateR = (1.0f - fbLpfCoeff) * fbSendR + fbLpfCoeff * fbLpfStateR;

        // Extra soft-clip during freeze: prevents runaway energy when DRIFT+FEEDBACK
        // are both high on a frozen tail
        if (isFrozen)
        {
            fbLpfStateL = fbLpfStateL / (1.0f + std::abs(fbLpfStateL));
            fbLpfStateR = fbLpfStateR / (1.0f + std::abs(fbLpfStateR));
        }

        // Normal soft-clip: maps (-∞,+∞) → (-1,+1), similar to tanh but free
        feedbackL = fbLpfStateL / (1.0f + std::abs(fbLpfStateL));
        feedbackR = fbLpfStateR / (1.0f + std::abs(fbLpfStateR));
    }

    //==========================================================================
    // Step 7: Apply DualFilter in WET position (before MIX)
    //==========================================================================
    if (filterPos == 1)
    {
        filterDetector.process(wetBuffer, N);
        dualFilter.process(filterDetector.getTextureBuffer(),
                           filterDetector.getBodyBuffer(),
                           wetBuffer, N);
    }

    //==========================================================================
    // Step 8: Dry/Wet MIX + Bypass blend (combined loop for efficiency)
    //==========================================================================
    {
        auto* outL = buffer.getWritePointer(0);
        auto* outR = buffer.getWritePointer(1);
        const auto* dryL = dryBuffer.getReadPointer(0);
        const auto* dryR = dryBuffer.getReadPointer(1);
        const auto* wetL = wetBuffer.getReadPointer(0);
        const auto* wetR = wetBuffer.getReadPointer(1);

        for (int i = 0; i < N; ++i)
        {
            const float mix      = mixSmoothed.getNextValue();
            const float bypassed = bypassSmoothed.getNextValue();

            float procL = dryL[i] * (1.0f - mix) + wetL[i] * mix;
            float procR = dryR[i] * (1.0f - mix) + wetR[i] * mix;

            // Bypass blends from processed → dry as bypassed goes 0→1
            outL[i] = dryL[i] * bypassed + procL * (1.0f - bypassed);
            outR[i] = dryR[i] * bypassed + procR * (1.0f - bypassed);

            // Store processed (pre-bypass) for FINAL filter position
            processedBuffer.setSample(0, i, procL);
            processedBuffer.setSample(1, i, procR);
        }
    }

    //==========================================================================
    // Step 9: Apply DualFilter in FINAL position (after MIX, on full signal)
    //==========================================================================
    if (filterPos == 2)
    {
        filterDetector.process(processedBuffer, N);
        dualFilter.process(filterDetector.getTextureBuffer(),
                           filterDetector.getBodyBuffer(),
                           filteredBuffer, N);

        // Re-blend with bypass
        auto* outL = buffer.getWritePointer(0);
        auto* outR = buffer.getWritePointer(1);
        const auto* dryL = dryBuffer.getReadPointer(0);
        const auto* dryR = dryBuffer.getReadPointer(1);
        const auto* filtL = filteredBuffer.getReadPointer(0);
        const auto* filtR = filteredBuffer.getReadPointer(1);

        // Reset bypass smoother to re-run the blend on filtered output
        // Simple approach: use a fresh block-constant bypass value
        const float bypassGain = bypassSmoothed.getCurrentValue();
        for (int i = 0; i < N; ++i)
        {
            outL[i] = dryL[i] * bypassGain + filtL[i] * (1.0f - bypassGain);
            outR[i] = dryR[i] * bypassGain + filtR[i] * (1.0f - bypassGain);
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* EpiphanyMachineProcessor::createEditor()
{
    return new EpiphanyMachineEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EpiphanyMachineProcessor();
}
