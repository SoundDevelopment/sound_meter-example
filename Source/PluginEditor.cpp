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
   meterOptions.faderEnabled = true;  // Enable or disable the 'fader' overlay. Use the sd::SoundMeter::MetersComponent::FadersChangeListener to get the fader value updates.
   meterOptions.headerEnabled         = false;           // Enable the 'header' part above the meter, displaying the channel ID.
   meterOptions.valueEnabled          = false;           // Enable the 'value' part below the level, displaying the peak level.
   meterOptions.refreshRate           = refreshRate_hz;  // Frequency of the meter updates (when using the internal timer).
   meterOptions.useGradient           = true;            // Use gradients to fill the meter (hard segment boundaries otherwise).
   meterOptions.showPeakHoldIndicator = false;           // Show the peak hold indicator (double click value to reset).
   meterOptions.tickMarksEnabled      = true;            // Enable tick-marks. Divider lines at certain levels on the meter and label strip.
   meterOptions.tickMarksOnTop        = true;            // Put the tick-marks above the level readout.
   meterOptions.tickMarks             = { -1.0f, -3.0f, -6.0f, -12.0f, -18.0f, -30.0f, -40.0f, -50.0f };  // Positions (in decibels) of the tick-marks.
   meterOptions.decayTime_ms          = 1000.0f;                                          // The meter will take 1000 ms to decay to 0.

   // Set the meter's segment options ...
   m_inputMeters.setOptions (meterOptions);
   std::vector<sd::SoundMeter::SegmentOptions> segmentOptions = { { { -60.0f, -18.0f }, { 0.0f, 0.5f }, juce::Colours::white, juce::Colours::green },
                                                                  { { -18.0f, -3.0f }, { 0.5f, 0.90f }, juce::Colours::green, juce::Colours::yellow },
                                                                  { { -3.0f, 0.0f }, { 0.90f, 1.0f }, juce::Colours::yellow, juce::Colours::red } };
   m_inputMeters.setMeterSegments (segmentOptions);

   // Use (or don't use) the label strip on the side of the meters.
   // This label-strip doubles as a master fader when faders are enabled...
   m_inputMeters.setLabelStripPosition (sd::SoundMeter::LabelStripPosition::right);

   // Set the meter CHANNEL FORMAT (determines number of meter and type description)...
   m_inputMeters.setChannelFormat (p.getOutputFormat());

   // Add the meters to the editor and make them visible...
   addAndMakeVisible (m_inputMeters);

   // Setup and add the corner re-sizer...
   m_sizeConstrainer.setSizeLimits (10, 10, 1300, 2000);
   addAndMakeVisible (m_cornerResizer);

   // Make sure that before the constructor has finished, you've set the
   // editor's size to whatever you need it to be.
   setSize (130, 700);

   // Start the 'polling' timer, that checks the audio processor for peak levels...
   startTimerHz (refreshRate_hz);
}

SoundMeterExampleAudioProcessorEditor::~SoundMeterExampleAudioProcessorEditor() { }


void SoundMeterExampleAudioProcessorEditor::resized()
{
   // Position the meters...
   m_inputMeters.setBounds (getLocalBounds());

   // Position the corner re-sizer...
   const int cornerResizerSize = 15;
   m_cornerResizer.setBounds (getLocalBounds().removeFromBottom (cornerResizerSize).removeFromRight (cornerResizerSize));
}

// The 'polling' timer.
void SoundMeterExampleAudioProcessorEditor::timerCallback()
{
   // Loop through all meters (channels)...
   for (int meterIndex = 0; meterIndex < m_inputMeters.getNumChannels(); ++meterIndex)
   {
      // Get the level, of the specified meter (channel), from the audio processor...
      m_inputMeters.setInputLevel (meterIndex, m_audioProcessor.getPeakLevel (meterIndex));
   }

   m_inputMeters.refresh();
}
