
set(plug_sources
	include/plugcontroller.h
	include/plugids.h
	include/plugprocessor.h
	include/version.h
	include/parameters.h
	source/plugfactory.cpp
	source/plugcontroller.cpp
	source/plugprocessor.cpp
)

#--- HERE change the target Name for your plug-in (for ex. set(target myDelay))-------
set(target transientshaper)

smtg_add_vst3plugin(${target} ${plug_sources})
set_target_properties(${target} PROPERTIES ${SDK_IDE_MYPLUGINS_FOLDER})
target_include_directories(${target} PUBLIC ${VSTGUI_ROOT}/vstgui4)
target_link_libraries(${target} PRIVATE base sdk vstgui_support)

smtg_add_vst3_resource(${target} "resource/plug.uidesc")
smtg_add_vst3_resource(${target} "resource/Btn_Auto.png")
smtg_add_vst3_resource(${target} "resource/Btn_Solo.png")
smtg_add_vst3_resource(${target} "resource/GUI_blank.png")
smtg_add_vst3_resource(${target} "resource/Knob_Atk_Fst_253.png")
smtg_add_vst3_resource(${target} "resource/Knob_Atk_Slw_253.png")
smtg_add_vst3_resource(${target} "resource/Knob_Factor_253.png")
smtg_add_vst3_resource(${target} "resource/Knob_Gain_253.png")
smtg_add_vst3_resource(${target} "resource/LED_full.png")
smtg_add_vst3_resource(${target} "resource/LED_neg_full.png")
smtg_add_vst3_resource(${target} "resource/LED_neg_off.png")
smtg_add_vst3_resource(${target} "resource/LED_off.png")

if(SMTG_MAC)
	smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS)
elseif(SMTG_WIN)
	target_sources(${target} PRIVATE resource/plug.rc)
endif()
