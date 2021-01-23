/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Instaniate knobs, make visible and set styles
    addAndMakeVisible(*(thresholdKnob = std::make_unique<juce::Slider>()));
    thresholdKnob->setSliderStyle(juce::Slider::Rotary);
    thresholdKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    thresholdKnob->setPopupDisplayEnabled(false, true, this);
    thresholdKnob->setTextValueSuffix(" dB");

    addAndMakeVisible(*(ratioKnob = std::make_unique<juce::Slider>()));
    ratioKnob->setSliderStyle(juce::Slider::Rotary);
    ratioKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    ratioKnob->setPopupDisplayEnabled(false,true,this);
    ratioKnob->setTextValueSuffix(":1");

    addAndMakeVisible(*(attackKnob = std::make_unique<juce::Slider>()));
    attackKnob->setSliderStyle(juce::Slider::Rotary);
    attackKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    attackKnob->setPopupDisplayEnabled(false, true, this);
    attackKnob->setTextValueSuffix("ms");

    addAndMakeVisible(*(releaseKnob = std::make_unique<juce::Slider>()));
    releaseKnob->setSliderStyle(juce::Slider::Rotary);
    releaseKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    releaseKnob->setPopupDisplayEnabled(false, true, this);
    releaseKnob->setTextValueSuffix("ms");

    //addAndMakeVisible(gainKnob = new juce::Slider("Gain"));
    addAndMakeVisible(*(gainKnob = std::make_unique<juce::Slider>()));
    gainKnob->setSliderStyle(juce::Slider::Rotary);
    gainKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    gainKnob->setPopupDisplayEnabled(false, true, this);
    gainKnob->setTextValueSuffix("dB");


    // Attach the Knobs to the processor    
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "threshold", *thresholdKnob);
        ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "ratio",     *ratioKnob);
       attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "attack",    *attackKnob);
      releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "release",   *releaseKnob);
         gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "gain",      *gainKnob);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 200);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText("Threshold", ((getWidth() / 6) * 2) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Ratio",     ((getWidth() / 6) * 3) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Attack",    ((getWidth() / 6) * 4) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Release",   ((getWidth() / 6) * 5) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Gain",      ((getWidth() / 6) * 1) - (100 / 2), (getHeight() / 2) + 5, 100, 100, juce::Justification::centred, false);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    thresholdKnob->setBounds(((getWidth() / 6) * 2) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
        ratioKnob->setBounds(((getWidth() / 6) * 3) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
       attackKnob->setBounds(((getWidth() / 6) * 4) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
      releaseKnob->setBounds(((getWidth() / 6) * 5) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
         gainKnob->setBounds(((getWidth() / 6) * 1) - (100 / 2), (getHeight() / 2) - (100 / 2), 100, 100);
}
