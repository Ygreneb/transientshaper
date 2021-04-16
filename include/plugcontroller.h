///------------------------------------------------------------------------

#pragma once

#define VSTGUI_LIVE_EDITING 1

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"

namespace Steinberg {
namespace TransientShaper {

//-----------------------------------------------------------------------------
class PlugController : public Vst::EditController
{
public:
//------------------------------------------------------------------------
	// create function required for Plug-in factory,
	// it will be called to create new instances of this controller
//------------------------------------------------------------------------
	static FUnknown* createInstance (void*)
	{
		return (Vst::IEditController*)new PlugController ();
	}

	//---from IPluginBase--------
	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;

	//---from EditController-----
	tresult PLUGIN_API setComponentState (IBStream* state) SMTG_OVERRIDE;
	IPlugView* PLUGIN_API createView(FIDString name) SMTG_OVERRIDE;
};

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
