/**************************************************************************/
/*  as_bridge.cpp                                                         */
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

#include "as_bridge.h"
#include "../compat/limboai_blackboard.h"
#include "../compat/limboai_bt.h"
#include "../compat/limboai_hsm.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#endif

// Include all bridge task headers
#include "as_bridge_action_activate.h"
#include "as_bridge_action_dispatch_event.h"
#include "as_bridge_action_wait_event.h"
#include "as_bridge_condition_can_activate.h"
#include "as_bridge_condition_event_occurred.h"
#include "as_bridge_condition_has_tag.h"
#include "as_bridge_state.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

ASBridge *ASBridge::singleton = nullptr;

void ASBridge::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_limboai_detected"), &ASBridge::is_limboai_detected);
	ClassDB::bind_method(D_METHOD("initialize"), &ASBridge::initialize);
	ClassDB::bind_method(D_METHOD("shutdown"), &ASBridge::shutdown);
}

bool ASBridge::is_limboai_available() {
#if defined(LIMBOAI_MODULE) || defined(LIMBOAI_GDEXTENSION)
	return true;
#else
	return false;
#endif
}

void ASBridge::initialize() {
	if (initialized) {
		return;
	}

	if (!is_limboai_available()) {
		WARN_PRINT("ASBridge: LimboAI not detected. Bridge tasks will not be available.");
		initialized = true;
		return;
	}

	limboai_detected = true;

	_register_bt_actions();
	_register_bt_conditions();
	_register_limbo_states();

	initialized = true;
#ifdef ABILITY_SYSTEM_GDEXTENSION
	UtilityFunctions::print("ASBridge: Initialized successfully with LimboAI integration.");
#endif
}

void ASBridge::shutdown() {
	if (!initialized) {
		return;
	}

	initialized = false;
	limboai_detected = false;

#ifdef ABILITY_SYSTEM_GDEXTENSION
	UtilityFunctions::print("ASBridge: Shutdown complete.");
#endif
}

void ASBridge::_register_bt_actions() {
#if defined(LIMBOAI_MODULE) || defined(LIMBOAI_GDEXTENSION)
	LIMBO_REGISTER_TASK(BTActionAS_ActivateAbility);
	LIMBO_REGISTER_TASK(BTActionAS_DispatchEvent);
	LIMBO_REGISTER_TASK(BTActionAS_WaitForEvent);
#ifdef ABILITY_SYSTEM_GDEXTENSION
	UtilityFunctions::print("ASBridge: Registered BTAction tasks");
#endif
#endif
}

void ASBridge::_register_bt_conditions() {
#if defined(LIMBOAI_MODULE) || defined(LIMBOAI_GDEXTENSION)
	LIMBO_REGISTER_TASK(BTConditionAS_HasTag);
	LIMBO_REGISTER_TASK(BTConditionAS_CanActivate);
	LIMBO_REGISTER_TASK(BTConditionAS_EventOccurred);
#ifdef ABILITY_SYSTEM_GDEXTENSION
	UtilityFunctions::print("ASBridge: Registered BTCondition tasks");
#endif
#endif
}

void ASBridge::_register_limbo_states() {
#if defined(LIMBOAI_MODULE) || defined(LIMBOAI_GDEXTENSION)
	GDREGISTER_CLASS(ASBridgeState);
#ifdef ABILITY_SYSTEM_GDEXTENSION
	UtilityFunctions::print("ASBridge: Registered ASBridgeState");
#endif
#endif
}

ASComponent *ASBridge::resolve_asc(Node *p_agent, const NodePath &p_asc_path) {
	if (!p_agent) {
		return nullptr;
	}

	// 1. If explicit path provided, try to resolve it
	if (!p_asc_path.is_empty()) {
		Node *node = p_agent->get_node_or_null(p_asc_path);
		if (node) {
			ASComponent *asc = Object::cast_to<ASComponent>(node);
			if (asc) {
				return asc;
			}
		}
	}

	// 2. Check if agent is an ASC directly
	ASComponent *asc = Object::cast_to<ASComponent>(p_agent);
	if (asc) {
		return asc;
	}

	// 3. Search agent's children for ASC
	TypedArray<Node> children = p_agent->get_children();
	for (int i = 0; i < children.size(); i++) {
		Node *child = Object::cast_to<Node>(children[i]);
		if (child) {
			asc = Object::cast_to<ASComponent>(child);
			if (asc) {
				return asc;
			}
		}
	}

	// 4. Not found
	return nullptr;
}

ASBridge::ASBridge() {
	singleton = this;
}

ASBridge::~ASBridge() {
	if (singleton == this) {
		singleton = nullptr;
	}
}
