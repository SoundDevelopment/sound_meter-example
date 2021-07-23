/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
*/
class SoundMeterExampleAudioProcessorEditor
  : public juce::AudioProcessorEditor
  , private juce::Timer
{
public:
   SoundMeterExampleAudioProcessorEditor (SoundMeterExampleAudioProcessor&);
   ~SoundMeterExampleAudioProcessorEditor() override;

   //==============================================================================
   void resized() override;

   void setChannelFormat (juce::AudioChannelSet metersFormat) { m_inputMeters.setChannelFormat (metersFormat); }

private:
   // This reference is provided as a quick way for your editor to
   // access the processor object that created it.
   SoundMeterExampleAudioProcessor& m_audioProcessor;
   sd::SoundMeter::MetersComponent  m_inputMeters;
   juce::ResizableCornerComponent   m_cornerResizer;
   juce::ComponentBoundsConstrainer m_sizeConstrainer;

   void timerCallback() override;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundMeterExampleAudioProcessorEditor)
};
