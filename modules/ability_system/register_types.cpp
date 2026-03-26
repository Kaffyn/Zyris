/**************************************************************************/
/*  register_types.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "register_types.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "src/core/ability_system.h"
#include "src/core/as_ability_spec.h"
#include "src/core/as_cue_spec.h"
#include "src/core/as_effect_spec.h"
#include "src/core/as_tag_spec.h"
#include "src/core/as_tag_types.h"
#include "src/core/as_utils.h"
#include "src/resources/as_ability.h"
#include "src/resources/as_ability_phase.h"
#include "src/resources/as_attribute.h"
#include "src/resources/as_attribute_set.h"
#include "src/resources/as_container.h"
#include "src/resources/as_cue.h"
#include "src/resources/as_cue_animation.h"
#include "src/resources/as_cue_audio.h"
#include "src/resources/as_effect.h"
#include "src/resources/as_package.h"
#include "src/resources/as_state_snapshot.h"
#include "src/scene/as_component.h"
#include "src/scene/as_delivery.h"

// AS Bridge for LimboAI integration
#include "limboai/register_types.h"
#include "src/bridge/as_bridge.h"
#include "src/bridge/as_bridge_action_activate.h"
#include "src/bridge/as_bridge_action_dispatch_event.h"
#include "src/bridge/as_bridge_action_wait_event.h"
#include "src/bridge/as_bridge_condition_can_activate.h"
#include "src/bridge/as_bridge_condition_event_occurred.h"
#include "src/bridge/as_bridge_condition_has_tag.h"
#include "src/bridge/as_bridge_state.h"

#ifdef TOOLS_ENABLED
#include "src/editor/as_editor_plugin.h"
#include "src/editor/as_editor_property.h"
#include "src/editor/as_inspector_plugin.h"
#include "src/editor/as_tags_panel.h"
#endif
#else
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_ability_spec.h"
#include "modules/ability_system/core/as_cue_spec.h"
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/core/as_tag_spec.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_ability_phase.h"
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/resources/as_container.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_cue_animation.h"
#include "modules/ability_system/resources/as_cue_audio.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/resources/as_state_snapshot.h"
#include "modules/ability_system/scene/as_component.h"
#include "modules/ability_system/scene/as_delivery.h"

// AS Bridge for LimboAI integration
#include "modules/ability_system/bridge/as_bridge.h"
#include "modules/ability_system/bridge/as_bridge_action_activate.h"
#include "modules/ability_system/bridge/as_bridge_action_dispatch_event.h"
#include "modules/ability_system/bridge/as_bridge_action_wait_event.h"
#include "modules/ability_system/bridge/as_bridge_condition_can_activate.h"
#include "modules/ability_system/bridge/as_bridge_condition_event_occurred.h"
#include "modules/ability_system/bridge/as_bridge_condition_has_tag.h"
#include "modules/ability_system/bridge/as_bridge_state.h"

#ifdef TOOLS_ENABLED
#include "modules/ability_system/editor/as_editor_plugin.h"
#include "modules/ability_system/editor/as_editor_property.h"
#include "modules/ability_system/editor/as_inspector_plugin.h"
#include "modules/ability_system/editor/as_tags_panel.h"
#endif
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
static AbilitySystem *as_singleton = nullptr;
static ASBridge *as_bridge = nullptr;

void initialize_as_module(ModuleInitializationLevel p_level) {
#else
static AbilitySystem *as_singleton = nullptr;
static ASBridge *as_bridge = nullptr;

void initialize_ability_system_module(ModuleInitializationLevel p_level) {
#endif
	// Initialize LimboAI module for every level (it handles internally)
	initialize_limboai_module(p_level);

	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(AbilitySystem);
		GDREGISTER_CLASS(ASTagSpec);
		GDREGISTER_CLASS(ASAbility);
		GDREGISTER_CLASS(ASAbilityPhase);
		GDREGISTER_CLASS(ASAbilitySpec);
		GDREGISTER_CLASS(ASContainer);
		GDREGISTER_CLASS(ASStateSnapshot);
		GDREGISTER_CLASS(ASAttribute);
		GDREGISTER_CLASS(ASAttributeSet);
		GDREGISTER_CLASS(ASEffect);
		GDREGISTER_CLASS(ASEffectSpec);
		GDREGISTER_CLASS(ASPackage);
		GDREGISTER_CLASS(ASCue);
		GDREGISTER_CLASS(ASCueSpec);
		GDREGISTER_CLASS(ASCueAnimation);
		GDREGISTER_CLASS(ASCueAudio);
		GDREGISTER_CLASS(ASTagUtils);
		GDREGISTER_CLASS(ASStateCache);
		GDREGISTER_CLASS(ASStateUtils);
		GDREGISTER_CLASS(ASComponent);
		GDREGISTER_CLASS(ASDelivery);

		// Register AS Bridge classes (only functional if LimboAI present)
		GDREGISTER_CLASS(ASBridge);
		GDREGISTER_CLASS(BTActionAS_ActivateAbility);
		GDREGISTER_CLASS(BTActionAS_DispatchEvent);
		GDREGISTER_CLASS(BTActionAS_WaitForEvent);
		GDREGISTER_CLASS(BTConditionAS_HasTag);
		GDREGISTER_CLASS(BTConditionAS_CanActivate);
		GDREGISTER_CLASS(BTConditionAS_EventOccurred);
		GDREGISTER_CLASS(ASBridgeState);

		as_singleton = memnew(AbilitySystem);
#ifdef ABILITY_SYSTEM_GDEXTENSION
		Engine::get_singleton()->register_singleton("AbilitySystem", as_singleton);
#else
		Engine::get_singleton()->add_singleton(Engine::Singleton("AbilitySystem", as_singleton));
#endif

		// Initialize AS Bridge for LimboAI integration (if available)
		as_bridge = memnew(ASBridge);
		as_bridge->initialize();
		// Note: Bridge auto-detects LimboAI at runtime
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(ASInspectorPlugin);
		GDREGISTER_CLASS(ASEditorPropertySelector);
		GDREGISTER_CLASS(ASEditorPropertyName);
		GDREGISTER_CLASS(ASEditorPropertyTagSelector);
		GDREGISTER_CLASS(ASTagsPanel);
		GDREGISTER_CLASS(ASEditorPlugin);
		EditorPlugins::add_by_type<ASEditorPlugin>();
	}
#endif
}

#ifdef ABILITY_SYSTEM_GDEXTENSION
void uninitialize_as_module(ModuleInitializationLevel p_level) {
#else
void uninitialize_ability_system_module(ModuleInitializationLevel p_level) {
#endif
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		if (as_bridge) {
			as_bridge->shutdown();
			memdelete(as_bridge);
			as_bridge = nullptr;
		}

		if (as_singleton) {
#ifdef ABILITY_SYSTEM_GDEXTENSION
			Engine::get_singleton()->unregister_singleton("AbilitySystem");
#else
			Engine::get_singleton()->remove_singleton("AbilitySystem");
#endif
			memdelete(as_singleton);
			as_singleton = nullptr;
		}
	}

	// Uninitialize LimboAI module (it handles internally)
	uninitialize_limboai_module(p_level);
}

#ifdef ABILITY_SYSTEM_GDEXTENSION
extern "C" {
GDExtensionBool GDE_EXPORT as_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_as_module);
	init_obj.register_terminator(uninitialize_as_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
#endif
