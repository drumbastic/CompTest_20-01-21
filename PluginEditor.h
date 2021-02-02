/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void oversampleChanged();

private:
    enum OversampleOptions
    {
        Off = 1,
        x2,
        x4,
        x8,
        x16,
        x32
    };
    // Create sliders as pointers to the object
    std::unique_ptr<juce::Slider> thresholdKnob;
    std::unique_ptr<juce::Slider> attackKnob;
    std::unique_ptr<juce::Slider> ratioKnob;
    std::unique_ptr<juce::Slider> releaseKnob;
    std::unique_ptr<juce::Slider> gainKnob;

    // Create the slider attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Create drop down menu
    std::unique_ptr<juce::ComboBox> oversampMenu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment > oversampMenuAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessorEditor)
};
