/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SoundMeterExampleAudioProcessor::SoundMeterExampleAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
  : AudioProcessor (BusesProperties()
   #if ! JucePlugin_IsMidiEffect
      #if ! JucePlugin_IsSynth
                      .withInput ("Input", juce::AudioChannelSet::stereo(), true)
      #endif
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
   #endif
  )
#endif
{
}

SoundMeterExampleAudioProcessor::~SoundMeterExampleAudioProcessor() { }

//==============================================================================
const juce::String SoundMeterExampleAudioProcessor::getName() const
{
   return JucePlugin_Name;
}

bool SoundMeterExampleAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
   return true;
#else
   return false;
#endif
}

bool SoundMeterExampleAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
   return true;
#else
   return false;
#endif
}

bool SoundMeterExampleAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
   return true;
#else
   return false;
#endif
}

double SoundMeterExampleAudioProcessor::getTailLengthSeconds() const
{
   return 0.0;
}

int SoundMeterExampleAudioProcessor::getNumPrograms()
{
   return 1;  // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int SoundMeterExampleAudioProcessor::getCurrentProgram()
{
   return 0;
}

void SoundMeterExampleAudioProcessor::setCurrentProgram (int /*index*/) { }

const juce::String SoundMeterExampleAudioProcessor::getProgramName (int /*index*/)
{
   return {};
}

void SoundMeterExampleAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/) { }

//==============================================================================
void SoundMeterExampleAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
   // Use this method as the place to do any pre-playback
   // initialisation that you need..

   auto numOutputChannels = getTotalNumOutputChannels();
   m_outputFormat         = juce::AudioChannelSet::canonicalChannelSet (numOutputChannels);
   auto* editor           = dynamic_cast<SoundMeterExampleAudioProcessorEditor*> (getActiveEditor());
   if (editor) editor->setChannelFormat (m_outputFormat);
}

void SoundMeterExampleAudioProcessor::releaseResources()
{
   // When playback stops, you can use this as an opportunity to free up any
   // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SoundMeterExampleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
   #if JucePlugin_IsMidiEffect
   juce::ignoreUnused (layouts);
   return true;
   #else
   // This is the place where you check if the layout is supported.
   // In this template code we only support mono or stereo.
   // Some plugin hosts, such as certain GarageBand versions, will only
   // load plugins that support stereo bus layouts.
   if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;

         // This checks if the input layout matches the output layout
      #if ! JucePlugin_IsSynth
   if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;
      #endif

   return true;
   #endif
}
#endif

void SoundMeterExampleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
   juce::ScopedNoDenormals noDenormals;
   auto                    totalNumInputChannels  = getTotalNumInputChannels();
   auto                    totalNumOutputChannels = getTotalNumOutputChannels();

   // In case we have more outputs than inputs, this code clears any output
   // channels that didn't contain input data, (because these aren't
   // guaranteed to be empty - they may contain garbage).
   // This is here to avoid people getting screaming feedback
   // when they first compile a plugin, but obviously you don't need to keep
   // this code if your algorithm always overwrites all the output channels.
   for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
      buffer.clear (i, 0, buffer.getNumSamples());

   // This is the place where you'd normally do the guts of your plugin's
   // audio processing...
   // Make sure to reset the state if your inner loop is processing
   // the samples and the outer loop is handling the channels.
   // Alternatively, you can process the samples with the channels
   // interleaved by keeping the same state.
   for (int channel = 0; channel < totalNumInputChannels; ++channel)
      setPeakLevel (channel, buffer.getMagnitude (channel, 0, buffer.getNumSamples()));

   buffer.clear();

}

//==============================================================================
bool SoundMeterExampleAudioProcessor::hasEditor() const
{
   return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SoundMeterExampleAudioProcessor::createEditor()
{
   return new SoundMeterExampleAudioProcessorEditor (*this);
}

//==============================================================================
void SoundMeterExampleAudioProcessor::getStateInformation (juce::MemoryBlock& /*destData*/)
{
   // You should use this method to store your parameters in the memory block.
   // You could do that either as raw data, or use the XML or ValueTree classes
   // as intermediaries to make it easy to save and load complex data.
}

void SoundMeterExampleAudioProcessor::setStateInformation (const void* /*data*/, int /*sizeInBytes*/)
{
   // You should use this method to restore your parameters from this memory block,
   // whose contents will have been created by the getStateInformation() call.
}

void SoundMeterExampleAudioProcessor::setPeakLevel (int channelIndex, float peakLevel)
{
   if (! juce::isPositiveAndBelow (channelIndex, m_peakLevels.size())) return;

   m_peakLevels[channelIndex].store (std::max (peakLevel, m_peakLevels[channelIndex].load()));
}

float SoundMeterExampleAudioProcessor::getPeakLevel (int channelIndex)
{
   if (! juce::isPositiveAndBelow (channelIndex, m_peakLevels.size())) return 0.0f;

   return m_peakLevels[channelIndex].exchange (0.0f);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
   return new SoundMeterExampleAudioProcessor();
}
