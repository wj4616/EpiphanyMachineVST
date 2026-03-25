#include "PresetManager.h"

PresetManager::PresetManager() {}

PresetManager::~PresetManager()
{
    // Remove our listener registrations to avoid dangling pointer calls
    if (apvts != nullptr)
    {
        for (auto* param : apvts->processor.getParameters())
            param->removeListener(this);
    }
}

void PresetManager::initialise(juce::AudioProcessorValueTreeState& a)
{
    apvts = &a;
    // Register on every parameter so we can detect user gestures during morph
    for (auto* param : apvts->processor.getParameters())
        param->addListener(this);
}

void PresetManager::selectPreset(int index, float morphTime)
{
    if (index < 0 || index >= kNumPresets) return;
    morphTimeSec  = std::max(0.1f, morphTime);
    startValues   = captureCurrentValues();
    targetValues  = kFactoryPresets[index];
    morphProgress = 0.0f;
    isMorphing    = true;
    excludedParams.clear();
}

//==============================================================================
// Called by PluginProcessor::processBlock() once per block.
// Computes how far to advance the morph and writes interpolated values.
//==============================================================================
void PresetManager::advance(double sampleRate, int numSamples)
{
    if (!isMorphing) return;

    morphProgress += (float)numSamples / (morphTimeSec * (float)sampleRate);
    if (morphProgress >= 1.0f)
    {
        morphProgress = 1.0f;
        isMorphing = false;
    }

    applyInterpolated(morphProgress);
}

//==============================================================================
void PresetManager::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
    // If the user grabs a knob while we're morphing, exclude that parameter
    // from further morph interpolation so the user's value "wins".
    if (gestureIsStarting && isMorphing && !systemChanging)
        excludedParams.insert(parameterIndex);
}

//==============================================================================
PresetData PresetManager::captureCurrentValues() const
{
    auto get = [&](const char* id) {
        return apvts->getRawParameterValue(id)->load();
    };
    auto getI = [&](const char* id) {
        return (int)apvts->getRawParameterValue(id)->load();
    };

    return {
        "",                                               // name (not used for "current")
        get("morph"),    get("mix"),      get("density"), get("chaos"),
        get("decay"),    get("damping"),  get("drift"),   get("width"),   get("feedback"),
        get("glitchFocus"), get("sensitivity"),
        getI("filterPos"),
        getI("texFilterType"), get("texFilterCutoff"), get("texFilterReso"),
        get("texLfoRate"), get("texLfoDepth"),
        getI("bodyFilterType"), get("bodyFilterCutoff"), get("bodyFilterReso")
    };
}

//==============================================================================
void PresetManager::writeParam(const char* id, int listenerIdx, float value)
{
    // Only write if this parameter wasn't grabbed by the user during morph
    if (excludedParams.count(listenerIdx)) return;

    systemChanging = true;
    if (auto* param = apvts->getParameter(id))
        param->setValueNotifyingHost(param->convertTo0to1(value));
    systemChanging = false;
}

void PresetManager::writeChoice(const char* id, int value, float progress, int startVal, int endVal)
{
    // Choice parameters snap at 50% morph progress
    int target = (progress >= 0.5f) ? endVal : startVal;
    systemChanging = true;
    if (auto* param = apvts->getParameter(id))
        param->setValueNotifyingHost(param->convertTo0to1((float)target));
    systemChanging = false;
    (void)value;
}

//==============================================================================
void PresetManager::applyInterpolated(float t)
{
    const auto& s = startValues;
    const auto& e = targetValues;

    // Build parameter index map — JUCE's getParameters() order matches layout
    // We track index by manually counting (fragile but simple for our fixed param set)
    // A more robust approach would be to look up by name, but for 25 fixed params
    // this is sufficient.
    auto& params = apvts->processor.getParameters();

    // Helper to find parameter index by ID
    auto idx = [&](const char* id) -> int {
        for (int i = 0; i < params.size(); ++i)
            if (params[i]->getName(256) == apvts->getParameter(id)->getName(256))
                return i;
        return -1;
    };

    // Float params — linear interpolation
    #define WF(id, sv, ev) writeParam(id, idx(id), lerpLinear(sv, ev, t))
    // Float params — log-space (frequencies)
    #define WL(id, sv, ev) writeParam(id, idx(id), lerpLog(sv, ev, t))
    // MIX — cosine
    #define WC(id, sv, ev) writeParam(id, idx(id), lerpCosine(sv, ev, t))

    WF("morph",    s.morph,    e.morph);
    WC("mix",      s.mix,      e.mix);          // cosine for smooth MIX transition
    WF("density",  s.density,  e.density);
    WF("chaos",    s.chaos,    e.chaos);
    WF("decay",    s.decay,    e.decay);
    WF("damping",  s.damping,  e.damping);
    WF("drift",    s.drift,    e.drift);
    WF("width",    s.width,    e.width);
    WF("feedback", s.feedback, e.feedback);
    WF("glitchFocus", s.glitchFocus, e.glitchFocus);
    WF("sensitivity",  s.sensitivity,  e.sensitivity);
    WL("texFilterCutoff", s.texFilterCutoff, e.texFilterCutoff);  // log space
    WF("texFilterReso",   s.texFilterReso,   e.texFilterReso);
    WF("texLfoRate",      s.texLfoRate,      e.texLfoRate);
    WF("texLfoDepth",     s.texLfoDepth,     e.texLfoDepth);
    WL("bodyFilterCutoff", s.bodyFilterCutoff, e.bodyFilterCutoff); // log space
    WF("bodyFilterReso",   s.bodyFilterReso,   e.bodyFilterReso);

    // Choice params — snap at 50%
    writeChoice("filterPos",    0, t, s.filterPos,    e.filterPos);
    writeChoice("texFilterType",0, t, s.texFilterType, e.texFilterType);
    writeChoice("bodyFilterType",0,t, s.bodyFilterType,e.bodyFilterType);

    #undef WF
    #undef WL
    #undef WC
}
