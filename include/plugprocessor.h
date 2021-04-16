//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace Steinberg {
namespace TransientShaper {

//-----------------------------------------------------------------------------
class PlugProcessor : public Vst::AudioEffect
{
public:
	PlugProcessor ();

	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
	                                       Vst::SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;

	tresult PLUGIN_API setupProcessing (Vst::ProcessSetup& setup) SMTG_OVERRIDE;
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;
	tresult PLUGIN_API process (Vst::ProcessData& data) SMTG_OVERRIDE;

//------------------------------------------------------------------------
	tresult PLUGIN_API setState (IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API getState (IBStream* state) SMTG_OVERRIDE;

	static FUnknown* createInstance (void*)
	{
		return (Vst::IAudioProcessor*)new PlugProcessor ();
	}

protected:
	bool mBypass = false;
	Vst::ParamValue mAttackFast = .0;
	Vst::ParamValue mAttackSlow = .0;
	Vst::ParamValue mRelease = .0;
	Vst::ParamValue mTransientFactor = .0;
	Vst::ParamValue mGain = .0;
	//Vst::ParamValue mMode = .0;
	//Vst::ParamValue mEnvFastMeter = .0;
	//Vst::ParamValue mEnvSlowMeter = .0;
	bool mTransientSolo = false;
	bool mAutoGain = false;

	// Output variables
	Vst::ParamValue mTransientMeterL = .0;
	Vst::ParamValue mTransientMeterR = .0;
	Vst::ParamValue mTransientMeterNegL = .0;
	Vst::ParamValue mTransientMeterNegR = .0;

	Vst::SampleRate mSampleRate = .0;
	float mEnvFast[2] = { .0f,.0f };
	float mEnvSlow[2] = { .0f,.0f };
};

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
