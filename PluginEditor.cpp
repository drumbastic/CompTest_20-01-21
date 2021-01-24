/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string> 

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

    // Instantiate Text button
    addAndMakeVisible(*(oversampButton = std::make_unique<juce::TextButton>("oversamp")));
    oversampButton->addListener(this);
    oversampButton->setButtonText("0x");
    oversampButton->setClickingTogglesState("true");
    //oversampButton->setToggleState("false", juce::dontSendNotification);
    oversampButton->setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    oversampButton->setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);


    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 300);
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

    //First row
    g.drawFittedText("Gain",        ((getWidth() / 5) * 2) - (100 / 2), ((getHeight() / 2) / 2) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Oversampling", ((getWidth() / 5) * 3) - (100 / 2), ((getHeight() / 2) / 2) + 5, 100, 100, juce::Justification::centred, false);
    // print state of toggle
    //g.drawFittedText(std::to_string(oversampButton->getToggleState()),((getWidth() / 5) * 3) - (100 / 2), ((getHeight() / 2) / 2) + 5, 100, 100, juce::Justification::centred, false);

    // Second row
    g.drawFittedText("Threshold", ((getWidth() / 5) * 1) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Ratio",     ((getWidth() / 5) * 2) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Attack",    ((getWidth() / 5) * 3) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) + 5, 100, 100, juce::Justification::centred, false);
    g.drawFittedText("Release",   ((getWidth() / 5) * 4) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) + 5, 100, 100, juce::Justification::centred, false);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //First row
          gainKnob->setBounds(((getWidth() / 5) * 2) - (100 / 2), ((getHeight() / 2) / 2) - (100 / 2), 100, 100);
    oversampButton->setBounds(((getWidth() / 5) * 3) - (100 / 2), ((getHeight() / 2) / 2) - (100 / 2), 100, 100);

    // Second row
    thresholdKnob->setBounds(((getWidth() / 5) * 1) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);
        ratioKnob->setBounds(((getWidth() / 5) * 2) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);
       attackKnob->setBounds(((getWidth() / 5) * 3) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);
      releaseKnob->setBounds(((getWidth() / 5) * 4) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);


}

void NewProjectAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (oversampButton->getToggleState() == true) {
        oversampButton->setButtonText("2x");
        audioProcessor.setFilteringEnbaled(oversampButton->getToggleState());
    }
    if (oversampButton->getToggleState() == false) {
        oversampButton->setButtonText("0x");
        audioProcessor.setFilteringEnbaled(oversampButton->getToggleState());
    }
}
