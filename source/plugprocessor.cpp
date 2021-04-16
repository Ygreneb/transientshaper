//------------------------------------------------------------------------


#include "../include/plugprocessor.h"
#include "../include/plugids.h"
#include "../include/parameters.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <algorithm>

namespace Steinberg {
namespace TransientShaper {

//-----------------------------------------------------------------------------
PlugProcessor::PlugProcessor ()
{
	// register its editor class
	setControllerClass (MyControllerUID);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::initialize (FUnknown* context)
{
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	if (result != kResultTrue)
		return kResultFalse;

	//---create Audio In/Out buses------
	// we want a stereo Input and a Stereo Output
	addAudioInput (STR16 ("AudioInput"), Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("AudioOutput"), Vst::SpeakerArr::kStereo);

	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setBusArrangements (Vst::SpeakerArrangement* inputs,
                                                            int32 numIns,
                                                            Vst::SpeakerArrangement* outputs,
                                                            int32 numOuts)
{
	// we only support one in and output bus and these buses must have the same number of channels
	if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0])
	{
		return AudioEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
	}
	return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setupProcessing (Vst::ProcessSetup& setup)
{
	// here you get, with setup, information about:
	// sampleRate, processMode, maximum number of samples per audio block
	mSampleRate = setup.sampleRate;

	return AudioEffect::setupProcessing (setup);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setActive (TBool state)
{
	if (state) // Initialize
	{
		// Allocate Memory Here
		// Ex: algo.create ();
	}
	else // Release
	{
		// Free Memory if still allocated
		// Ex: if(algo.isCreated ()) { algo.destroy (); }
	}
	return AudioEffect::setActive (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::process (Vst::ProcessData& data)
{
	//--- Read inputs parameter changes-----------
	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			Vst::IParamValueQueue* paramQueue =
			    data.inputParameterChanges->getParameterData (index);
			if (paramQueue)
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				switch (paramQueue->getParameterId ())
				{
					case TransientShaperParams::kBypassId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue)
							mBypass = (value > 0.5f);
						break;
					case TransientShaperParams::kAttackFastId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
							mAttackFast = value;
						break;
					case TransientShaperParams::kAttackSlowId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
							mAttackSlow = value;
						break;
					//case TransientShaperParams::kReleaseId:
					//	if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
					//		mRelease = value;
					//	break;
					case TransientShaperParams::kTransientFactorId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
							mTransientFactor = value;
						break;
					case TransientShaperParams::kGainId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
							mGain = value;
						break;
					case TransientShaperParams::kTransientSoloId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
							mTransientSolo = (value > 0.5f);
						break;
					case TransientShaperParams::kAutoGainId:
						if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
							mAutoGain = (value > 0.5f);
						break;
				}
			}
		}
	}

	//--- Process Audio---------------------
	//--- ----------------------------------
	if (data.numInputs == 0 || data.numOutputs == 0 || mBypass)
	{
		// nothing to do
		return kResultOk;
	}

	if (data.numSamples > 0)
	{
		// Process Algorithm
		// Ex: algo.process (data.inputs[0].channelBuffers32, data.outputs[0].channelBuffers32,
		// data.numSamples);

		size_t numChannels = std::min(data.inputs[0].numChannels, data.outputs[0].numChannels);
		mRelease = std::max(mAttackFast, mAttackSlow);

		// Envelope taken from http://www.musicdsp.org/en/latest/Analysis/136-envelope-follower-with-different-attack-and-release.html
		const float attackFastPlain	= ParameterToPlain(mAttackFast, kAttackMin, kAttackMax);
		const float attackSlowPlain	= ParameterToPlain(mAttackSlow, kAttackMin, kAttackMax);
		const float releasePlain	= ParameterToPlain(mRelease, kReleaseMin, kReleaseMax);

		const float attackCoeffFast	= exp(log(0.01) / (attackFastPlain * mSampleRate * 0.001));
		const float attackCoeffSlow	= exp(log(0.01) / (attackSlowPlain * mSampleRate * 0.001));
		const float releaseCoeff	= exp(log(0.01) / (releasePlain * mSampleRate * 0.001));

		// see http://www.sengpielaudio.com/calculator-FactorRatioLevelDecibel.htm
		//const float transGain = pow(10, ParameterToPlain(mTransientGain, kTransientGainMin, kTransientGainMax) / 20.0);
		const float transFactor = ParameterToPlain(mTransientFactor, kTransientFactorMin, kTransientFactorMax);
		//const float gainFactor = mAutoGain ?
		//	1.f / (transFactor / 3.f + 1.f) : // TODO Envelope difference is never 1, but ~1/3 at peak. Perhaps an envelope of the difference with very long release could help?
		//	pow(10, ParameterToPlain(mGain, kGainMin, kGainMax) / 20.0);
		const float gainFactor = pow(10, ParameterToPlain(mGain, kGainMin, kGainMax) / 20.0);

		const size_t maxNumChannels = std::min(numChannels, static_cast<size_t>(2));

		// Stereo Mode
		for (size_t c = 0; c < maxNumChannels; ++c)
		{
			for (size_t i = 0; i < data.numSamples; ++i)
			{
				// Update fast and slow envelopes
				const float in = data.inputs[0].channelBuffers32[c][i];
				const float v = fabs(in);
				if (v > mEnvFast[c])
					mEnvFast[c] = attackCoeffFast * (mEnvFast[c] - v) + v;
				else
					mEnvFast[c] = releaseCoeff * (mEnvFast[c] - v) + v;
				if (v > mEnvSlow[c])
					mEnvSlow[c] = attackCoeffSlow * (mEnvSlow[c] - v) + v;
				else
					mEnvSlow[c] = releaseCoeff * (mEnvSlow[c] - v) + v;

				mEnvFast[c] = Clip(mEnvFast[c], 0.f, 1.f);
				mEnvSlow[c] = Clip(mEnvSlow[c], 0.f, 1.f);

				if (mTransientSolo)
				{
					data.outputs[0].channelBuffers32[c][i] = gainFactor * transFactor * in * (mEnvFast[c] - mEnvSlow[c]);
				}
				else
				{
					data.outputs[0].channelBuffers32[c][i] = gainFactor * (in + transFactor * in * (mEnvFast[c] - mEnvSlow[c]));
				}
			}
		}

		//else if (mMode < 0.66)
		//{
		//	// Mono Mode
		//	for (size_t i = 0; i < data.numSamples; ++i)
		//	{
		//		float in = 0.f;
		//		for (size_t c = 0; c < maxNumChannels; ++c)
		//		{
		//			in += data.inputs[0].channelBuffers32[c][i];
		//		}
		//		const float v = fabs(in);
		//		if (v > mEnvFast[0])
		//			mEnvFast[0] = attackCoeffFast * (mEnvFast[0] - v) + v;
		//		else
		//			mEnvFast[0] = releaseCoeff * (mEnvFast[0] - v) + v;
		//		if (v > mEnvSlow[0])
		//			mEnvSlow[0] = attackCoeffSlow * (mEnvSlow[0] - v) + v;
		//		else
		//			mEnvSlow[0] = releaseCoeff * (mEnvSlow[0] - v) + v;

		//		for (size_t c = 0; c < maxNumChannels; ++c)
		//		{
		//			data.outputs[0].channelBuffers32[c][i] = totalGain * (in + transGain * in * (mEnvFast[0] - mEnvSlow[0]));
		//		}
		//	}
		//}
		//else
		//{
		//	// M/S Mode
		//	float in_LR[] = { 0.f,0.f };
		//	float in_MS[] = { 0.f,0.f };
		//	float out_MS[] = { 0.f,0.f };

		//	for (size_t i = 0; i < data.numSamples; ++i)
		//	{
		//		in_LR[0] = data.inputs[0].channelBuffers32[0][i];
		//		in_LR[1] = data.inputs[0].channelBuffers32[1][i];
		//		in_MS[0] = (in_LR[0] + in_LR[1]) / 2.f;
		//		in_MS[1] = (in_LR[0] - in_LR[1]) / 2.f;
		//		
		//		for (size_t c = 0; c < 2; ++c)
		//		{
		//			const float v = fabs(in_LR[c]);
		//			if (v > mEnvFast[c])
		//				mEnvFast[c] = attackCoeffFast * (mEnvFast[c] - v) + v;
		//			else
		//				mEnvFast[c] = releaseCoeff * (mEnvFast[c] - v) + v;
		//			if (v > mEnvSlow[c])
		//				mEnvSlow[c] = attackCoeffSlow * (mEnvSlow[c] - v) + v;
		//			else
		//				mEnvSlow[c] = releaseCoeff * (mEnvSlow[c] - v) + v;

		//			out_MS[c] = totalGain * (in_MS[c] + transGain * in_MS[c] * (mEnvFast[c] - mEnvSlow[c]));
		//		}
		//		
		//		data.outputs[0].channelBuffers32[0][i] = out_MS[0] + out_MS[1];
		//		data.outputs[0].channelBuffers32[1][i] = out_MS[0] - out_MS[1];
		//	}
		//}
		

	}

	// a new value of VuMeter will be send to the host
	// (the host will send it back in sync to our controller for updating our editor)
	if (Vst::IParameterChanges * outParamChanges = data.outputParameterChanges)
	{
		int32 indexQueue = 0;
		int32 indexData = 0;
		Vst::IParamValueQueue* paramQueue;
		const double exp_slope = 3.0;
		
		paramQueue = outParamChanges->addParameterData(kTransientMeterLId, indexQueue);
		if (paramQueue)
		{
			paramQueue->addPoint(0, 1.0 - exp(-exp_slope * std::max(mEnvFast[0]-mEnvSlow[0], 0.f)), indexData);
		}

		paramQueue = outParamChanges->addParameterData(kTransientMeterRId, indexQueue);
		if (paramQueue)
		{
			paramQueue->addPoint(0, 1.0 - exp(-exp_slope * std::max(mEnvFast[1] - mEnvSlow[1], 0.f)), indexData);
		}

		paramQueue = outParamChanges->addParameterData(kTransientMeterNegLId, indexQueue);
		if (paramQueue)
		{
			paramQueue->addPoint(0, 1.0 - exp(-exp_slope * std::max(mEnvSlow[0] - mEnvFast[0], 0.f)), indexData);
		}

		paramQueue = outParamChanges->addParameterData(kTransientMeterNegRId, indexQueue);
		if (paramQueue)
		{
			paramQueue->addPoint(0, 1.0 - exp(-exp_slope * std::max(mEnvSlow[1] - mEnvFast[1], 0.f)), indexData);
		}
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	// called when we load a preset or project, the model has to be reloaded

	IBStreamer streamer (state, kLittleEndian);

	if (streamer.readBool(mBypass) == false)
		return kResultFalse;
	
	if (streamer.readDouble(mAttackFast) == false)
		return kResultFalse;

	if (streamer.readDouble(mAttackSlow) == false)
		return kResultFalse;

	if (streamer.readDouble(mRelease) == false)
		return kResultFalse;

	if (streamer.readDouble(mTransientFactor) == false)
		return kResultFalse;

	if (streamer.readDouble(mGain) == false)
		return kResultFalse;

	if (streamer.readBool(mTransientSolo) == false)
		return kResultFalse;

	if (streamer.readBool(mAutoGain) == false)
		return kResultFalse;

	//if (streamer.readDouble(mMode) == false)
	//	return kResultFalse;
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState (IBStream* state)
{
	// here we need to save the model (preset or project)

	IBStreamer streamer (state, kLittleEndian);
	streamer.writeBool	(mBypass);
	streamer.writeDouble(mAttackFast);
	streamer.writeDouble(mAttackSlow);
	streamer.writeDouble(mRelease);
	streamer.writeDouble(mTransientFactor);
	streamer.writeDouble(mGain);
	streamer.writeBool	(mTransientSolo);
	streamer.writeBool	(mAutoGain);
	//streamer.writeDouble(mMode);
	
	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
