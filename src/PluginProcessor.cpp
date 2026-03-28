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

    addFloat("morph",    "Morph",    0.0f,  1.0f,   0.3f);
    addFloat("mix",      "Mix",      0.0f,  1.0f,   0.5f);
    addFloat("density",  "Density",  0.0f,  1.0f,   0.3f);
    addFloat("chaos",    "Chaos",    0.0f,  1.0f,   0.2f);
    addFloat("decay",    "Decay",    0.0f,  20.0f,  2.0f,  0.5f);  // seconds
    addFloat("damping",  "Damping",  0.0f,  1.0f,   0.5f);
    addFloat("drift",    "Drift",    0.0f,  24.0f,  0.0f);          // semitones up
    addFloat("width",    "Width",    0.0f,  1.0f,   0.6f);
    addFloat("feedback", "Feedback", 0.0f,  0.95f,  0.3f,  0.5f);
    addFloat("drive",    "Drive",    0.0f,  1.0f,   0.0f);

    addBool("bypass", "Bypass");
    addBool("freeze", "Freeze");
    addFloat("glitchFocus",  "Glitch Focus",  0.0f, 1.0f, 0.5f);
    addFloat("sensitivity",  "Sensitivity",   0.0f, 1.0f, 0.5f);

    addChoice("filterPos",    "Filter Position", {"Off","Wet","Final","Feedback"}, 0);
    addChoice("texFilterType","Tex Filter Type",  {"LP","HP","BP"}, 0);
    addFloat("texFilterCutoff", "Tex Cutoff",   20.0f, 20000.0f, 2000.0f, 0.3f);
    addFloat("texFilterReso",   "Tex Reso",     0.0f,  1.0f,     0.3f);
    addFloat("texLfoRate",      "Tex LFO Rate", 0.01f, 10.0f,    0.5f,  0.3f);
    addFloat("texLfoDepth",     "Tex LFO Depth",0.0f,  1.0f,     0.0f);
    addChoice("bodyFilterType","Body Filter Type", {"LP","HP","BP"}, 0);
    addFloat("bodyFilterCutoff","Body Cutoff",  20.0f, 20000.0f, 1200.0f, 0.3f);
    addFloat("bodyFilterReso",  "Body Reso",    0.0f,  1.0f,     0.2f);

    addChoice("preset", "Preset",
              {"INIT","Quantum Foam","Superposition","Wave Collapse","Entanglement",
               "Heisenberg Blur","Planck Wall","Bose-Einstein","Hawking Radiation",
               "Dark Matter","Antimatter","String Theory","Quantum Tunneling",
               "Decoherence","Zero Point Energy","Singularity","Multiverse","Schrodinger"}, 0);
    addFloat("morphTime", "Morph Time", 0.1f, 30.0f, 2.0f);

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
    inputDetector.prepare(sr, spb);
    filterDetector.prepare(sr, spb);
    dualFilter.prepare(sr, spb);

    dryBuffer.setSize(2, spb);
    glitchInputBuffer.setSize(2, spb);
    cleanPassBuffer.setSize(2, spb);
    wetBuffer.setSize(2, spb);
    filteredBuffer.setSize(2, spb);
    processedBuffer.setSize(2, spb);
    feedbackBuffer.setSize(2, spb);
    feedbackBuffer.clear();

    const double sm = 0.02; // 20 ms ramp for most params
    morphSmoothed.reset(sr, sm);
    mixSmoothed.reset(sr, sm);
    feedbackSmoothed.reset(sr, sm);
    glitchFocusSmoothed.reset(sr, sm);
    driveSmoothed.reset(sr, sm);
    bypassSmoothed.reset(sr, 512.0 / sr);

    auto getV = [&](const char* id) { return apvts.getRawParameterValue(id)->load(); };
    morphSmoothed.setCurrentAndTargetValue(getV("morph"));
    mixSmoothed.setCurrentAndTargetValue(getV("mix"));
    feedbackSmoothed.setCurrentAndTargetValue(getV("feedback"));
    glitchFocusSmoothed.setCurrentAndTargetValue(getV("glitchFocus"));
    driveSmoothed.setCurrentAndTargetValue(getV("drive"));
    bypassSmoothed.setCurrentAndTargetValue(getV("bypass") > 0.5f ? 1.0f : 0.0f);
}

//==============================================================================
void EpiphanyMachineProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int N = buffer.getNumSamples();

    if (buffer.getNumChannels() == 1)
        buffer.copyFrom(1, 0, buffer, 0, 0, N);

    dryBuffer.copyFrom(0, 0, buffer, 0, 0, N);
    dryBuffer.copyFrom(1, 0, buffer, 1, 0, N);

    auto getV = [&](const char* id) { return apvts.getRawParameterValue(id)->load(); };

    const bool  isBypassed   = getV("bypass") > 0.5f;
    const bool  isFrozen     = getV("freeze") > 0.5f;
    const int   filterPos    = (int)getV("filterPos");
    const int   presetIdx    = (int)getV("preset");
    const float morphTimeSec = getV("morphTime");

    bypassSmoothed.setTargetValue(isBypassed ? 1.0f : 0.0f);
    morphSmoothed.setTargetValue(getV("morph"));
    mixSmoothed.setTargetValue(getV("mix"));
    feedbackSmoothed.setTargetValue(getV("feedback"));
    glitchFocusSmoothed.setTargetValue(getV("glitchFocus"));
    driveSmoothed.setTargetValue(getV("drive"));

    glitchEngine.setDensity(getV("density"));
    glitchEngine.setChaos(getV("chaos"));
    glitchEngine.setDrift(getV("drift"));
    diffusionNetwork.setDecaySeconds(getV("decay"));
    diffusionNetwork.setDamping(getV("damping"));
    diffusionNetwork.setWidth(getV("width"));
    diffusionNetwork.setFreezeActive(isFrozen);
    pitchDrifter.setDrift(getV("drift"));
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

    if (presetIdx != lastPresetIdx) {
        presetManager.selectPreset(presetIdx, morphTimeSec);
        lastPresetIdx = presetIdx;
    }
    presetManager.advance(getSampleRate(), N);

    //--- Fast bypass ---
    if (bypassSmoothed.getCurrentValue() >= 0.999f && isBypassed)
    {
        for (int i = 0; i < N; ++i) {
            morphSmoothed.getNextValue(); mixSmoothed.getNextValue();
            feedbackSmoothed.getNextValue(); glitchFocusSmoothed.getNextValue();
            driveSmoothed.getNextValue(); bypassSmoothed.getNextValue();
        }
        buffer.copyFrom(0, 0, dryBuffer, 0, 0, N);
        buffer.copyFrom(1, 0, dryBuffer, 1, 0, N);
        return;
    }

    //--- Step 1: Input detect ---
    inputDetector.process(buffer, N);
    const auto& texBuf = inputDetector.getTextureBuffer();
    const auto& bodBuf = inputDetector.getBodyBuffer();

    //--- Step 2: Glitch focus routing ---
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
            if (gf <= 0.5f) {
                float t = gf * 2.0f;
                giL[i] = bL[i]*(1.0f-t) + inL[i]*t; giR[i] = bR[i]*(1.0f-t) + inR[i]*t;
                cpL[i] = tL[i]*(1.0f-t);             cpR[i] = tR[i]*(1.0f-t);
            } else {
                float t = (gf-0.5f)*2.0f;
                giL[i] = inL[i]*(1.0f-t) + tL[i]*t; giR[i] = inR[i]*(1.0f-t) + tR[i]*t;
                cpL[i] = bL[i]*t;                     cpR[i] = bR[i]*t;
            }
        }
    }

    //--- Step 3: Glitch Engine ---
    glitchEngine.process(glitchInputBuffer);

    //--- Step 4: Morph blend + add full-block feedback from previous block ---
    {
        auto* outL = buffer.getWritePointer(0);
        auto* outR = buffer.getWritePointer(1);
        const auto* giL = glitchInputBuffer.getReadPointer(0);
        const auto* giR = glitchInputBuffer.getReadPointer(1);
        const auto* cpL = cleanPassBuffer.getReadPointer(0);
        const auto* cpR = cleanPassBuffer.getReadPointer(1);
        const auto* fbL = feedbackBuffer.getReadPointer(0);
        const auto* fbR = feedbackBuffer.getReadPointer(1);
        for (int i = 0; i < N; ++i)
        {
            const float morph = morphSmoothed.getNextValue();
            feedbackSmoothed.getNextValue();  // advance, read via getCurrentValue() later
            driveSmoothed.getNextValue();     // advance, read via getCurrentValue() in Step 10
            outL[i] = cpL[i] + giL[i] * morph + fbL[i];
            outR[i] = cpR[i] + giR[i] * morph + fbR[i];
        }
    }

    //--- Step 5: DiffusionNetwork ---
    diffusionNetwork.process(buffer);
    wetBuffer.copyFrom(0, 0, buffer, 0, 0, N);
    wetBuffer.copyFrom(1, 0, buffer, 1, 0, N);

    //--- Step 6: Filter in WET position ---
    if (filterPos == 1)
    {
        filterDetector.process(wetBuffer, N);
        dualFilter.process(filterDetector.getTextureBuffer(),
                           filterDetector.getBodyBuffer(), wetBuffer, N);
    }

    //--- Step 7: Dry/Wet MIX + Bypass blend ---
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
            // Equal-power mixing: keeps perceived level constant as mix sweeps 0→1
            const float dryGain  = std::cos(mix * juce::MathConstants<float>::halfPi);
            const float wetGain  = std::sin(mix * juce::MathConstants<float>::halfPi);
            float procL = dryL[i]*dryGain + wetL[i]*wetGain;
            float procR = dryR[i]*dryGain + wetR[i]*wetGain;
            outL[i] = dryL[i]*bypassed + procL*(1.0f-bypassed);
            outR[i] = dryR[i]*bypassed + procR*(1.0f-bypassed);
            processedBuffer.setSample(0, i, procL);
            processedBuffer.setSample(1, i, procR);
        }
    }

    //--- Step 8: Filter in FINAL position ---
    if (filterPos == 2)
    {
        filterDetector.process(processedBuffer, N);
        dualFilter.process(filterDetector.getTextureBuffer(),
                           filterDetector.getBodyBuffer(), filteredBuffer, N);
        auto* outL = buffer.getWritePointer(0);
        auto* outR = buffer.getWritePointer(1);
        const auto* dryL  = dryBuffer.getReadPointer(0);
        const auto* dryR  = dryBuffer.getReadPointer(1);
        const auto* filtL = filteredBuffer.getReadPointer(0);
        const auto* filtR = filteredBuffer.getReadPointer(1);
        // Use block-constant bypass gain (smoother already advanced in Step 7); acceptable 1-block transition approximation
        const float bypassGain = bypassSmoothed.getCurrentValue();
        for (int i = 0; i < N; ++i)
        {
            outL[i] = dryL[i]*bypassGain + filtL[i]*(1.0f-bypassGain);
            outR[i] = dryR[i]*bypassGain + filtR[i]*(1.0f-bypassGain);
        }
    }

    //--- Step 9: tanh output limiter ---
    {
        auto* outL = buffer.getWritePointer(0);
        auto* outR = buffer.getWritePointer(1);
        for (int i = 0; i < N; ++i)
        {
            outL[i] = std::tanh(outL[i]);
            outR[i] = std::tanh(outR[i]);
        }
    }

    //--- Step 10: Compute new feedbackBuffer for next block ---
    {
        const float fbGain = feedbackSmoothed.getCurrentValue();
        const auto* wL = wetBuffer.getReadPointer(0);
        const auto* wR = wetBuffer.getReadPointer(1);
        auto* fbL = feedbackBuffer.getWritePointer(0);
        auto* fbR = feedbackBuffer.getWritePointer(1);
        for (int i = 0; i < N; ++i)
        {
            fbL[i] = wL[i] * fbGain;
            fbR[i] = wR[i] * fbGain;
        }

        // PitchDrifter operates in feedback loop
        pitchDrifter.process(feedbackBuffer);

        // Optional filter in feedback loop
        if (filterPos == 3)
        {
            filterDetector.process(feedbackBuffer, N);
            dualFilter.process(filterDetector.getTextureBuffer(),
                               filterDetector.getBodyBuffer(), feedbackBuffer, N);
        }

        // Wavefolder (true bypass at drive=0)
        const float drive = driveSmoothed.getCurrentValue();
        if (drive > 0.001f)
        {
            const float foldAmt = 1.0f + drive * 4.0f;
            const float piC     = juce::MathConstants<float>::pi;
            auto* fL = feedbackBuffer.getWritePointer(0);
            auto* fR = feedbackBuffer.getWritePointer(1);
            for (int i = 0; i < N; ++i)
            {
                float fx = fL[i] * foldAmt * piC;
                fL[i]    = fL[i] + drive * (std::sin(fx) / foldAmt - fL[i]);
                fx       = fR[i] * foldAmt * piC;
                fR[i]    = fR[i] + drive * (std::sin(fx) / foldAmt - fR[i]);
            }
        }

        // Soft-clip feedback to prevent runaway at high feedback+drive (applied unconditionally, including freeze mode)
        {
            auto* fL = feedbackBuffer.getWritePointer(0);
            auto* fR = feedbackBuffer.getWritePointer(1);
            for (int i = 0; i < N; ++i)
            {
                fL[i] = fL[i] / (1.0f + std::abs(fL[i]));
                fR[i] = fR[i] / (1.0f + std::abs(fR[i]));
            }
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
