#pragma once
#include <JuceHeader.h>

//==============================================================================
// PresetData — plain-old-data struct holding all morphable parameter values.
//==============================================================================
struct PresetData
{
    juce::String name;
    float morph, mix, density, chaos, decay, damping, drift, width, feedback;
    float glitchFocus, sensitivity;
    int   filterPos;        // 0=Off, 1=Wet, 2=Final
    int   texFilterType;    // 0=LP, 1=HP, 2=BP
    float texFilterCutoff, texFilterReso, texLfoRate, texLfoDepth;
    int   bodyFilterType;   // 0=LP, 1=HP, 2=BP
    float bodyFilterCutoff, bodyFilterReso;
};

// Each row: name, morph,mix,density,chaos,decay,damping,drift,width,feedback,
//           glitchFocus,sensitivity, filterPos,
//           texType,texCut,texReso,texLfoRate,texLfoDepth,
//           bodyType,bodyCut,bodyReso
inline const PresetData kFactoryPresets[] =
{
    // -------------------------------------------------------------------------
    // 0 — INIT
    { "INIT",
      0.3f,0.5f,0.3f,0.2f,0.6f,0.5f,0.0f,0.6f,0.3f,  0.5f,0.5f,
      0, 0,2000.0f,0.3f,0.5f,0.0f,  0,1200.0f,0.2f },

    // -------------------------------------------------------------------------
    // DEEP AMBIENT
    { "Void Meditation",
      0.0f,0.7f,0.1f,0.1f,0.92f,0.8f,0.0f,0.7f,0.2f,  0.5f,0.5f,
      1, 0,1500.0f,0.15f,0.05f,0.2f,  0,700.0f,0.1f },

    { "Eternal Hall",
      0.0f,0.65f,0.1f,0.05f,0.95f,0.2f,0.0f,0.8f,0.15f,  0.5f,0.5f,
      0, 0,8000.0f,0.1f,0.1f,0.0f,  0,6000.0f,0.1f },

    { "Cathedral of Dust",
      0.05f,0.75f,0.15f,0.2f,0.9f,0.9f,0.0f,0.6f,0.25f,  0.5f,0.4f,
      2, 0,800.0f,0.2f,0.02f,0.15f,  0,500.0f,0.15f },

    { "Weightless",
      0.0f,0.6f,0.1f,0.1f,0.85f,0.35f,0.0f,1.0f,0.1f,  0.5f,0.5f,
      1, 1,3000.0f,0.1f,0.07f,0.15f,  0,2000.0f,0.1f },

    { "Frozen Lake",
      0.0f,0.55f,0.05f,0.05f,0.8f,0.5f,0.0f,0.5f,0.1f,  0.5f,0.3f,
      1, 0,3500.0f,0.25f,0.03f,0.1f,  0,1800.0f,0.15f },

    // -------------------------------------------------------------------------
    // SHIMMER
    { "Phantom Drift",
      0.15f,0.6f,0.2f,0.15f,0.8f,0.15f,12.0f,0.65f,0.6f,  0.5f,0.5f,
      1, 1,1800.0f,0.2f,0.08f,0.3f,  0,5000.0f,0.1f },

    { "Descending Angel",
      0.1f,0.65f,0.15f,0.1f,0.82f,0.6f,-12.0f,0.6f,0.55f,  0.5f,0.5f,
      1, 0,1200.0f,0.2f,0.06f,0.2f,  0,800.0f,0.15f },

    { "Fifth Dimension",
      0.1f,0.55f,0.15f,0.1f,0.78f,0.3f,7.0f,0.6f,0.55f,  0.5f,0.5f,
      1, 0,4000.0f,0.15f,0.1f,0.15f,  0,3000.0f,0.1f },

    { "Dissonant Cloud",
      0.2f,0.55f,0.2f,0.2f,0.75f,0.4f,3.0f,0.7f,0.5f,  0.5f,0.5f,
      1, 2,2500.0f,0.35f,0.15f,0.2f,  0,2000.0f,0.2f },

    { "Tritone Abyss",
      0.15f,0.6f,0.2f,0.25f,0.8f,0.65f,6.0f,0.55f,0.5f,  0.5f,0.5f,
      1, 0,1500.0f,0.3f,0.04f,0.25f,  0,900.0f,0.25f },

    { "Solar Flare",
      0.6f,0.5f,0.45f,0.5f,0.65f,0.05f,12.0f,1.0f,0.7f,  0.8f,0.8f,
      1, 1,2500.0f,0.4f,1.2f,0.35f,  1,400.0f,0.2f },

    // -------------------------------------------------------------------------
    // GLITCH AMBIENT
    { "Neural Storm",
      0.85f,0.55f,0.8f,0.75f,0.5f,0.25f,5.0f,0.85f,0.55f,  1.0f,0.7f,
      1, 2,2200.0f,0.55f,2.5f,0.5f,  0,3000.0f,0.15f },

    { "Broken Transmission",
      0.7f,0.5f,0.65f,0.6f,0.45f,0.35f,5.0f,0.8f,0.4f,  1.0f,0.7f,
      1, 2,1800.0f,0.5f,0.3f,0.4f,  0,2500.0f,0.15f },

    { "Scattered Memories",
      0.5f,0.55f,0.35f,0.45f,0.7f,0.55f,0.0f,0.65f,0.3f,  0.8f,0.6f,
      1, 0,2500.0f,0.2f,0.2f,0.15f,  0,1500.0f,0.15f },

    { "Stutter Prayer",
      0.6f,0.5f,0.7f,0.05f,0.6f,0.5f,0.0f,0.5f,0.25f,  0.5f,0.5f,
      1, 0,3000.0f,0.15f,0.1f,0.0f,  0,2000.0f,0.1f },

    { "Insect Choir",
      0.75f,0.45f,0.9f,0.8f,0.4f,0.1f,12.0f,0.9f,0.45f,  1.0f,0.9f,
      1, 1,4000.0f,0.5f,5.0f,0.4f,  1,2000.0f,0.2f },

    { "Living Texture",
      0.4f,0.5f,0.5f,0.4f,0.6f,0.4f,7.0f,0.6f,0.35f,  1.0f,0.6f,
      1, 2,2500.0f,0.4f,0.1f,0.3f,  0,1800.0f,0.15f },

    // -------------------------------------------------------------------------
    // DEEP & DARK
    { "Deep Submersion",
      0.25f,0.75f,0.25f,0.35f,0.88f,0.85f,-5.0f,0.55f,0.5f,  0.3f,0.6f,
      2, 0,900.0f,0.25f,0.03f,0.4f,  0,500.0f,0.3f },

    { "Tectonic Drone",
      0.15f,0.7f,0.2f,0.3f,0.85f,0.95f,-7.0f,0.4f,0.45f,  0.0f,0.4f,
      2, 0,600.0f,0.3f,0.02f,0.3f,  0,350.0f,0.35f },

    { "Obsidian Mirror",
      0.0f,0.65f,0.1f,0.05f,0.88f,0.75f,0.0f,0.5f,0.2f,  0.5f,0.4f,
      2, 0,1000.0f,0.3f,0.04f,0.1f,  0,600.0f,0.25f },

    { "Gravity Well",
      0.3f,0.7f,0.3f,0.3f,0.82f,0.8f,-12.0f,0.45f,0.55f,  0.3f,0.5f,
      2, 0,700.0f,0.2f,0.03f,0.3f,  0,400.0f,0.3f },

    { "Midnight Fog",
      0.1f,0.7f,0.15f,0.2f,0.85f,0.7f,0.0f,0.6f,0.3f,  0.5f,0.5f,
      1, 0,1200.0f,0.15f,0.05f,0.2f,  0,800.0f,0.2f },

    // -------------------------------------------------------------------------
    // CRYSTALLINE & METALLIC
    { "Crystal Lattice",
      0.45f,0.45f,0.6f,0.1f,0.55f,0.2f,7.0f,0.5f,0.35f,  0.0f,0.4f,
      2, 2,3500.0f,0.45f,0.2f,0.15f,  2,800.0f,0.35f },

    { "Bell Garden",
      0.35f,0.5f,0.5f,0.3f,0.65f,0.15f,12.0f,0.7f,0.4f,  1.0f,0.7f,
      1, 2,3000.0f,0.65f,0.15f,0.1f,  0,4000.0f,0.1f },

    { "Ice Fracture",
      0.55f,0.45f,0.7f,0.5f,0.45f,0.05f,5.0f,0.85f,0.4f,  1.0f,0.8f,
      1, 1,5000.0f,0.4f,3.0f,0.3f,  1,1500.0f,0.2f },

    { "Singing Metal",
      0.2f,0.55f,0.25f,0.15f,0.7f,0.25f,0.0f,0.6f,0.3f,  0.5f,0.5f,
      1, 2,1500.0f,0.7f,0.08f,0.4f,  2,600.0f,0.6f },

    { "Prism",
      0.3f,0.5f,0.4f,0.2f,0.6f,0.1f,7.0f,0.9f,0.4f,  0.5f,0.6f,
      1, 1,3500.0f,0.3f,0.25f,0.25f,  2,1200.0f,0.4f },

    // -------------------------------------------------------------------------
    // EXPERIMENTAL & EXTREME
    { "Event Horizon",
      0.5f,0.6f,0.4f,0.5f,0.9f,0.3f,12.0f,0.8f,0.85f,  0.5f,0.5f,
      1, 0,6000.0f,0.2f,0.1f,0.15f,  0,3000.0f,0.15f },

    { "Ghost in the Machine",
      0.65f,0.55f,0.55f,0.5f,0.6f,0.45f,3.0f,0.65f,0.35f,  0.0f,0.6f,
      1, 0,5000.0f,0.15f,0.08f,0.1f,  2,1000.0f,0.4f },

    { "Parallel Universe",
      0.5f,0.5f,0.5f,0.4f,0.65f,0.4f,5.0f,0.7f,0.4f,  1.0f,0.7f,
      2, 1,4000.0f,0.5f,1.5f,0.45f,  0,500.0f,0.4f },

    { "Temporal Collapse",
      0.9f,0.6f,0.85f,0.9f,0.7f,0.2f,6.0f,1.0f,0.65f,  0.5f,0.8f,
      1, 2,2000.0f,0.5f,4.0f,0.5f,  0,2500.0f,0.2f },

    { "The Long Exhale",
      0.1f,0.8f,0.1f,0.15f,0.95f,0.6f,0.0f,0.6f,0.35f,  0.5f,0.3f,
      1, 0,2000.0f,0.15f,0.01f,0.3f,  0,1200.0f,0.15f },

    { "Machine Awakening",
      0.0f,0.5f,0.5f,0.0f,0.65f,0.4f,7.0f,0.6f,0.4f,  1.0f,0.6f,
      1, 2,2500.0f,0.35f,0.2f,0.2f,  0,1800.0f,0.15f },
};

inline constexpr int kNumPresets = 34;
