cmake_minimum_required(VERSION 3.22)
project(EpiphanyMachine VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(EpiphanyMachine
    COMPANY_NAME              "Custom"
    PLUGIN_MANUFACTURER_CODE  Cstm
    PLUGIN_CODE               EpMa
    FORMATS                   VST3
    PRODUCT_NAME              "Epiphany Machine"
    IS_SYNTH                  FALSE
    NEEDS_MIDI_INPUT          FALSE
    NEEDS_MIDI_OUTPUT         FALSE
    IS_MIDI_EFFECT            FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD   TRUE
    VST3_COPY_DIR             "$ENV{HOME}/.vst3"
)

juce_generate_juce_header(EpiphanyMachine)

target_sources(EpiphanyMachine PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/DSP/GlitchEngine.cpp
    src/DSP/DiffusionNetwork.cpp
    src/DSP/PitchDrifter.cpp
    src/DSP/StereoWidener.cpp
    src/DSP/TextureBodyDetector.cpp
    src/DSP/DualFilter.cpp
    src/Presets/PresetManager.cpp
    src/GUI/CustomLookAndFeel.cpp
    src/GUI/MorphKnob.cpp
    src/GUI/PresetBar.cpp
)

target_include_directories(EpiphanyMachine PRIVATE src)

target_compile_definitions(EpiphanyMachine
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(EpiphanyMachine
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
