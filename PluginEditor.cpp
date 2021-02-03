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
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "threshold", *thresholdKnob);
    thresholdKnob->setSliderStyle(juce::Slider::Rotary);
    thresholdKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    thresholdKnob->setPopupDisplayEnabled(false, true, this);
    thresholdKnob->setTextValueSuffix(" dB");

    addAndMakeVisible(*(ratioKnob = std::make_unique<juce::Slider>()));
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "ratio", *ratioKnob);
    ratioKnob->setSliderStyle(juce::Slider::Rotary);
    ratioKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    ratioKnob->setPopupDisplayEnabled(false,true,this);
    ratioKnob->setTextValueSuffix(":1");

    addAndMakeVisible(*(attackKnob = std::make_unique<juce::Slider>()));
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "attack", *attackKnob);
    attackKnob->setSliderStyle(juce::Slider::Rotary);
    attackKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    attackKnob->setPopupDisplayEnabled(false, true, this);
    attackKnob->setTextValueSuffix("ms");

    addAndMakeVisible(*(releaseKnob = std::make_unique<juce::Slider>()));
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "release", *releaseKnob);
    releaseKnob->setSliderStyle(juce::Slider::Rotary);
    releaseKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    releaseKnob->setPopupDisplayEnabled(false, true, this);
    releaseKnob->setTextValueSuffix("ms");

    addAndMakeVisible(*(gainKnob = std::make_unique<juce::Slider>()));
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.getState(), "gain", *gainKnob);
    gainKnob->setSliderStyle(juce::Slider::Rotary);
    gainKnob->setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 20);
    gainKnob->setPopupDisplayEnabled(false, true, this);
    gainKnob->setTextValueSuffix("dB");

    // Instantiate drop down menu
    addAndMakeVisible(*(oversampMenu = std::make_unique<juce::ComboBox>("oversamp_menu")));
    oversampMenu->setJustificationType(juce::Justification::centred);
    oversampMenu->addItem("Off", Off);
    oversampMenu->addItem("x2",  x2);
    oversampMenu->addItem("x4",  x4);
    oversampMenu->addItem("x8",  x8);
    oversampMenu->addItem("x16", x16); 
    oversampMenu->addListener(this);
    oversampMenu->onChange = [this] { comboBoxChanged(oversampMenu.get()); };
    oversampMenuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.getState(), "oversamp_menu", *oversampMenu);
    oversampMenu->setSelectedId(Off);

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
    g.drawFittedText("Gain",         ((getWidth() / 5) * 2) - (100 / 2), ((getHeight() / 2) / 2) + 5, 100, 100, juce::Justification::centred, false);
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
    oversampMenu->setBounds(((getWidth() / 5) * 3) - (100 / 2), ((getHeight() / 2) / 2) - (100 / 2), 100, 100);

    // Second row
    thresholdKnob->setBounds(((getWidth() / 5) * 1) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);
        ratioKnob->setBounds(((getWidth() / 5) * 2) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);
       attackKnob->setBounds(((getWidth() / 5) * 3) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);
      releaseKnob->setBounds(((getWidth() / 5) * 4) - (100 / 2), ((getHeight() / 2) + ((getHeight() / 2) / 2)) - (100 / 2), 100, 100);
}

void NewProjectAudioProcessorEditor::comboBoxChanged(juce::ComboBox*)
{
    switch (oversampMenu->getSelectedId())
    {
    case Off:
        oversampMenu->setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        oversampMenu->setColour(juce::ComboBox::ColourIds::textColourId, juce::Colours::white);
        oversampMenu->setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::white);
        break;
    case x2:
        oversampMenu->setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::lightyellow);
        oversampMenu->setColour(juce::ComboBox::ColourIds::textColourId, juce::Colours::black);
        oversampMenu->setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::black);
        break;
    case x4:
        oversampMenu->setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::yellow);
        oversampMenu->setColour(juce::ComboBox::ColourIds::textColourId, juce::Colours::black);
        oversampMenu->setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::black);
        break;
    case x8:
        oversampMenu->setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::orange);
        oversampMenu->setColour(juce::ComboBox::ColourIds::textColourId, juce::Colours::black);
        oversampMenu->setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::black);
        break;
    case x16:
        oversampMenu->setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::orangered);
        oversampMenu->setColour(juce::ComboBox::ColourIds::textColourId, juce::Colours::white);
        oversampMenu->setColour(juce::ComboBox::ColourIds::arrowColourId, juce::Colours::white);
        break;
    }
}