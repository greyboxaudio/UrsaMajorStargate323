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
    programBox.addItem ("Plate 1", 4);
    programBox.addItem ("Plate 2", 3);
    programBox.addItem ("Chamber", 1);
    programBox.addItem ("Small Hall", 2);
    programBox.addItem ("Hall", 6);
    programBox.addItem ("Large Hall", 5);
    programBox.addItem ("Cathedral", 7);
    programBox.addItem ("Canyon", 8);
    programBox.setJustificationType (Justification::centred);
    addAndMakeVisible (programBox);

    programBoxAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "PROGRAM", programBox);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
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
    //g.drawFittedText ("Hello Wolrd!", getLocalBounds(), juce::Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    //const int componentSize(80);
    programBox.setBounds(bounds.removeFromTop (200).withSizeKeepingCentre (120, 60));
}
