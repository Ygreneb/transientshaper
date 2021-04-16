//------------------------------------------------------------------------

#pragma once

namespace Steinberg {
namespace TransientShaper {

// HERE are defined the parameter Ids which are exported to the host
enum TransientShaperParams : Vst::ParamID
{
	kBypassId				= 100,

	kAttackFastId			= 101,
	kAttackSlowId			= 102,
	kReleaseId				= 103,
	kTransientFactorId		= 104,
	kGainId					= 105,
	//kModeId					= 106,
	//kEnvFastMeterId			= 107,
	//kEnvSlowMeterId			= 108,
	kTransientMeterLId		= 109,
	kTransientMeterRId		= 110,
	kTransientSoloId		= 111,
	kAutoGainId				= 112,
	kTransientMeterNegLId	= 113,
	kTransientMeterNegRId	= 114
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID	(0x61F08419, 0x928447E1, 0xB46E6BFC, 0x26548C8E);
static const FUID MyControllerUID	(0xA1A44866, 0x58714D34, 0x8442409F, 0x6674120B);

//------------------------------------------------------------------------
} // namespace HelloWorld
} // namespace Steinberg
