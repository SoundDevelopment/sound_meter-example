/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
SoundMeterExampleAudioProcessorEditor::SoundMeterExampleAudioProcessorEditor (SoundMeterExampleAudioProcessor& p)
  : AudioProcessorEditor (&p), m_audioProcessor (p), m_cornerResizer (this, &m_sizeConstrainer)
{
   const int refreshRate_hz = 30;

   // Set the meter OPTONS ...
   sd::SoundMeter::Options meterOptions;
   meterOptions.faderEnabled          = false;
   meterOptions.headerEnabled         = false;
   meterOptions.valueEnabled          = false;
   meterOptions.refreshRate           = refreshRate_hz;
   meterOptions.useGradient           = true;
   meterOptions.showPeakHoldIndicator = false;
   meterOptions.peakRegion_db         = -3.0f;   // -3.0 dB peak region divider.
   meterOptions.warningRegion_db      = -12.0f;  // -12.0 dB warning region indicator.
   meterOptions.tickMarksEnabled      = true;
   meterOptions.tickMarksOnTop        = true;
   meterOptions.tickMarks             = { -1.0f, -3.0f, -6.0f, -12.0f, -18.0f, -36.0f };  // Positions (in decibels) of the tick-marks.
   meterOptions.decayTime_ms          = 1000.0f;
   m_inputMeters.setOptions (meterOptions);

   // Use (or don't use) the label strip on the side of the meters.
   // This label-strip doubles as a master fader when faders are enabled...
   m_inputMeters.useLabelStrip (true);

   // Set the meter CHANNEL FORMAT (determines number of meter and type description)...
   m_inputMeters.setChannelFormat (p.getOutputFormat());

   addAndMakeVisible (m_inputMeters);

   m_sizeConstrainer.setSizeLimits (10, 10, 1300, 2000);
   addAndMakeVisible (m_cornerResizer);

   // Make sure that before the constructor has finished, you've set the
   // editor's size to whatever you need it to be.
   setSize (130, 700);

   startTimerHz (refreshRate_hz);
}

SoundMeterExampleAudioProcessorEditor::~SoundMeterExampleAudioProcessorEditor() { }


void SoundMeterExampleAudioProcessorEditor::resized()
{
   m_inputMeters.setBounds (getLocalBounds());

   const int cornerResizerSize = 15;
   m_cornerResizer.setBounds (getLocalBounds().removeFromBottom (cornerResizerSize).removeFromRight (cornerResizerSize));

   // This is generally where you'll want to lay out the positions of any
   // subcomponents in your editor..
}

void SoundMeterExampleAudioProcessorEditor::timerCallback()
{
   // Loop through all meters (channels)...
   for (int meterIndex = 0; meterIndex < m_inputMeters.getNumMeters(); ++meterIndex)
   {
      // Get the level, of the specified meter (channel), from the audio processor...
      m_inputMeters.setInputLevel (meterIndex, m_audioProcessor.getPeakLevel (meterIndex));
   }

   m_inputMeters.refresh();
}
