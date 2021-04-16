//------------------------------------------------------------------------

#include "../include/plugcontroller.h"
#include "../include/plugids.h"
#include "../include/parameters.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"

namespace Steinberg {
namespace TransientShaper {

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugController::initialize (FUnknown* context)
{
	tresult result = EditController::initialize (context);
	if (result == kResultTrue)
	{
		//---Create Parameters------------
		parameters.addParameter (STR16 ("Bypass"), 0, 1, 0,
		                         Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
								 TransientShaperParams::kBypassId);

		parameters.addParameter (new Vst::RangeParameter(
			STR16 ("Attack Fast"), TransientShaperParams::kAttackFastId, STR16 ("ms"),
			kAttackMin, kAttackMax, kAttackMin, 0, Vst::ParameterInfo::kCanAutomate, 0, STR16 ("AtkFst")));
		parameters.addParameter(new Vst::RangeParameter(
			STR16("Attack Slow"), TransientShaperParams::kAttackSlowId, STR16("ms"),
			kAttackMin, kAttackMax, kAttackMin, 0, Vst::ParameterInfo::kCanAutomate, 0, STR16("AtkSlw")));
		//parameters.addParameter(new Vst::RangeParameter(
		//	STR16("Release"), TransientShaperParams::kReleaseId, STR16("ms"),
		//	kReleaseMin, kReleaseMax, kReleaseMin, 0, Vst::ParameterInfo::kCanAutomate, 0, STR16("Rls")));
		parameters.addParameter(new Vst::RangeParameter(
			STR16("Transient Factor"), TransientShaperParams::kTransientFactorId, STR16(""),
			kTransientFactorMin, kTransientFactorMax, 0.0, 0, Vst::ParameterInfo::kCanAutomate, 0, STR16("TransF")));
		parameters.addParameter(new Vst::RangeParameter(
			STR16("Gain"), TransientShaperParams::kGainId, STR16("dB"),
			kGainMin, kGainMax, 0.0, 0, Vst::ParameterInfo::kCanAutomate, 0, STR16("Gain")));
		parameters.addParameter(STR16("Transient Solo"), nullptr, 1, 0,
								Vst::ParameterInfo::kNoFlags, TransientShaperParams::kTransientSoloId,
								0, STR16("TransSolo"));
		parameters.addParameter(STR16("Auto Gain"), nullptr, 1, 0,
								Vst::ParameterInfo::kNoFlags, TransientShaperParams::kAutoGainId,
								0, STR16("AutoGain"));
		parameters.addParameter(STR16("Transient Meter L"), nullptr, 0, 0,
								Vst::ParameterInfo::kIsReadOnly, TransientShaperParams::kTransientMeterLId,
								0, STR16("TransMtrL"));
		parameters.addParameter(STR16("Transient Meter R"), nullptr, 0, 0,
								Vst::ParameterInfo::kIsReadOnly, TransientShaperParams::kTransientMeterRId,
								0, STR16("TransMtrR"));
		parameters.addParameter(STR16("Transient Meter Neg L"), nullptr, 0, 0,
								Vst::ParameterInfo::kIsReadOnly, TransientShaperParams::kTransientMeterNegLId,
								0, STR16("TransMtrNegL"));
		parameters.addParameter(STR16("Transient Meter Neg R"), nullptr, 0, 0,
								Vst::ParameterInfo::kIsReadOnly, TransientShaperParams::kTransientMeterNegRId,
								0, STR16("TransMtrNegR"));

		//parameters.addParameter(STR16("Mode"), 0, 2, 0,
		//	Vst::ParameterInfo::kNoFlags, TransientShaperParams::kModeId, 0, STR16("Mode"));

		//parameters.addParameter(STR16("Env Fast Meter"), nullptr, 0, 0,
		//						Vst::ParameterInfo::kIsReadOnly, TransientShaperParams::kEnvFastMeterId,
		//						0, STR16("EnvFstMtr"));
		//parameters.addParameter(STR16("Env Slow Meter"), nullptr, 0, 0,
		//						Vst::ParameterInfo::kIsReadOnly, TransientShaperParams::kEnvSlowMeterId,
		//						0, STR16("EnvSlwMtr"));
	}
	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API PlugController::createView(FIDString name)
{
	// someone wants my editor
	if (name && strcmp(name, Vst::ViewType::kEditor) == 0)
	{
		return new VSTGUI::VST3Editor(this, "view", "plug.uidesc");;
	}
	return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setComponentState (IBStream* state)
{
	// we receive the current state of the component (processor part)
	// we read our parameters and bypass value...
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	bool boolval;
	double doubleval;

	if (streamer.readBool(boolval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kBypassId, boolval ? 1.0 : 0.0);

	if (streamer.readDouble(doubleval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kAttackFastId, doubleval);

	if (streamer.readDouble(doubleval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kAttackSlowId, doubleval);

	if (streamer.readDouble(doubleval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kReleaseId, doubleval);
	
	if (streamer.readDouble(doubleval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kTransientFactorId, doubleval);

	if (streamer.readDouble(doubleval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kGainId, doubleval);

	if (streamer.readBool(boolval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kTransientSoloId, boolval ? 1.0 : 0.0);

	if (streamer.readBool(boolval) == false)
		return kResultFalse;
	setParamNormalized(TransientShaperParams::kAutoGainId, boolval ? 1.0 : 0.0);

	//if (streamer.readDouble(doubleval) == false)
	//	return kResultFalse;
	//setParamNormalized(TransientShaperParams::kModeId, doubleval);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
