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

private:
    // Create sliders as pointers to the object
    juce::ScopedPointer<juce::Slider> thresholdKnob;
    juce::ScopedPointer<juce::Slider> attackKnob;
    juce::ScopedPointer<juce::Slider> ratioKnob;
    juce::ScopedPointer<juce::Slider> releaseKnob;

    // Create the slider attachments
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessorEditor)
};
