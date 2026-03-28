#pragma once
#include <JuceHeader.h>

//==============================================================================
// PresetData — v3 adds float drive field; decay is now seconds (0-20).
//==============================================================================
struct PresetData
{
    juce::String name;
    float morph, mix, density, chaos, decay, damping, drift, width, feedback, drive;
    float glitchFocus, sensitivity;
    int   filterPos;        // 0=Off 1=Wet 2=Final 3=Feedback
    int   texFilterType;    // 0=LP 1=HP 2=BP
    float texFilterCutoff, texFilterReso, texLfoRate, texLfoDepth;
    int   bodyFilterType;
    float bodyFilterCutoff, bodyFilterReso;
};

// Each row: name, morph,mix,density,chaos, decay(s),damping, drift(st),width,feedback, drive,
//           glitchFocus,sensitivity, filterPos,
//           texType,texCut,texReso,texLfoRate,texLfoDepth,
//           bodyType,bodyCut,bodyReso
inline const PresetData kFactoryPresets[] =
{
    { "INIT",
      0.3f,0.5f,0.3f,0.2f, 4.0f,0.5f, 0.0f,0.6f,0.3f, 0.0f,
      0.5f,0.5f, 0, 0,2000.0f,0.3f,0.5f,0.0f, 0,1200.0f,0.2f },

    { "Quantum Foam",
      0.3f,0.65f,0.6f,0.4f, 8.0f,0.6f, 4.0f,0.7f,0.4f, 0.1f,
      0.7f,0.6f, 1, 1,3000.0f,0.2f,0.3f,0.2f, 0,800.0f,0.1f },

    { "Superposition",
      0.5f,0.6f,0.3f,0.2f, 12.0f,0.3f, 12.0f,0.9f,0.6f, 0.3f,
      0.5f,0.5f, 1, 0,4000.0f,0.15f,0.05f,0.25f, 0,3000.0f,0.1f },

    { "Wave Collapse",
      0.8f,0.55f,0.7f,0.7f, 5.0f,0.4f, 8.0f,0.8f,0.55f, 0.5f,
      0.9f,0.7f, 2, 2,2500.0f,0.6f,2.0f,0.5f, 0,1500.0f,0.2f },

    { "Entanglement",
      0.4f,0.7f,0.2f,0.3f, 15.0f,0.5f, 7.0f,1.0f,0.7f, 0.2f,
      0.5f,0.5f, 1, 0,2000.0f,0.2f,0.1f,0.3f, 0,2000.0f,0.15f },

    { "Heisenberg Blur",
      0.6f,0.5f,0.5f,0.9f, 6.0f,0.3f, 16.0f,0.8f,0.5f, 0.6f,
      0.8f,0.7f, 1, 2,3000.0f,0.5f,3.0f,0.4f, 0,2000.0f,0.2f },

    { "Planck Wall",
      0.9f,0.45f,0.9f,0.5f, 2.0f,0.1f, 0.0f,0.5f,0.4f, 0.8f,
      1.0f,0.9f, 1, 1,5000.0f,0.4f,5.0f,0.3f, 1,800.0f,0.2f },

    { "Bose-Einstein",
      0.15f,0.75f,0.15f,0.1f, 18.0f,0.2f, 24.0f,0.85f,0.5f, 0.0f,
      0.3f,0.4f, 1, 1,2000.0f,0.1f,0.04f,0.2f, 0,4000.0f,0.05f },

    { "Hawking Radiation",
      0.2f,0.7f,0.2f,0.4f, 16.0f,0.7f, 12.0f,0.75f,0.8f, 0.4f,
      0.4f,0.5f, 1, 0,1000.0f,0.2f,0.03f,0.3f, 0,600.0f,0.2f },

    { "Dark Matter",
      0.1f,0.8f,0.15f,0.2f, 18.0f,0.9f, 0.0f,0.4f,0.6f, 0.0f,
      0.0f,0.3f, 2, 0,400.0f,0.3f,0.02f,0.2f, 0,250.0f,0.35f },

    { "Antimatter",
      0.7f,0.5f,0.6f,0.8f, 4.0f,0.4f, 0.0f,0.9f,0.6f, 0.7f,
      0.8f,0.7f, 3, 0,6000.0f,0.15f,0.1f,0.1f, 1,3000.0f,0.1f },

    { "String Theory",
      0.3f,0.6f,0.4f,0.15f, 10.0f,0.2f, 7.0f,0.7f,0.5f, 0.3f,
      0.5f,0.6f, 1, 2,3500.0f,0.8f,0.1f,0.3f, 2,1500.0f,0.5f },

    { "Quantum Tunneling",
      0.75f,0.55f,0.8f,0.6f, 3.0f,0.3f, 6.0f,0.7f,0.45f, 0.4f,
      1.0f,0.8f, 3, 1,4000.0f,0.3f,0.5f,0.3f, 0,2500.0f,0.1f },

    { "Decoherence",
      0.65f,0.5f,0.65f,0.75f, 5.0f,0.5f, 10.0f,0.8f,0.55f, 0.5f,
      0.7f,0.7f, 1, 2,2000.0f,0.4f,1.5f,0.4f, 0,1800.0f,0.15f },

    { "Zero Point Energy",
      0.45f,0.6f,0.55f,0.5f, 8.0f,0.4f, 5.0f,0.65f,0.5f, 0.2f,
      0.6f,0.6f, 1, 0,2500.0f,0.25f,0.2f,0.2f, 0,1500.0f,0.15f },

    { "Singularity",
      0.5f,0.7f,0.4f,0.6f, 20.0f,0.4f, 20.0f,0.6f,0.9f, 0.9f,
      0.5f,0.5f, 3, 0,8000.0f,0.15f,0.05f,0.1f, 0,3000.0f,0.1f },

    { "Multiverse",
      0.85f,0.6f,0.75f,0.95f, 6.0f,0.3f, 12.0f,1.0f,0.7f, 0.7f,
      1.0f,0.9f, 1, 2,3000.0f,0.6f,4.0f,0.5f, 1,1500.0f,0.3f },

    { "Schrodinger",
      0.5f,0.55f,0.5f,0.5f, 10.0f,0.5f, 12.0f,0.75f,0.5f, 0.5f,
      0.5f,0.5f, 1, 2,2500.0f,0.4f,0.25f,0.3f, 0,1600.0f,0.2f },
};

inline constexpr int kNumPresets = 18;
