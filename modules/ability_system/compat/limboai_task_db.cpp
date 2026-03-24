/**************************************************************************/
/*  limboai_task_db.cpp                                                   */
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

#include "limboai_task_db.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;
#endif

#if !defined(LIMBOAI_MODULE) && !defined(LIMBOAI_GDEXTENSION)

LimboTaskDB *LimboTaskDB::singleton = nullptr;

void LimboTaskDB::_bind_methods() {
	ClassDB::bind_method(D_METHOD("register_task", "class_name"), &LimboTaskDB::register_task);
	ClassDB::bind_method(D_METHOD("is_task_registered", "class_name"), &LimboTaskDB::is_task_registered);
	ClassDB::bind_method(D_METHOD("get_registered_tasks"), &LimboTaskDB::get_registered_tasks);
	ClassDB::bind_method(D_METHOD("is_available"), &LimboTaskDB::is_available);
}

void LimboTaskDB::register_task(const StringName &p_class_name) {
#ifdef LIMBOAI_MODULE
	// Real implementation when LimboAI module is available
	// This would call the actual LimboAI TaskDB registration
	// using Godot core print_line
#elif defined(LIMBOAI_GDEXTENSION)
	// GDExtension implementation
	if (Engine::get_singleton()->has_singleton("LimboAI")) {
		// Call LimboAI GDExtension registration
		godot::UtilityFunctions::print(vformat("LimboTaskDB: Registering task '%s' (GDExtension)", p_class_name));
	} else {
		godot::UtilityFunctions::print(vformat("LimboTaskDB: Cannot register '%s' - LimboAI not available", p_class_name));
	}
#else
	// Stub implementation - just log
	godot::UtilityFunctions::print(vformat("LimboTaskDB: Stub registration for '%s'", p_class_name));
#endif
}

bool LimboTaskDB::is_task_registered(const StringName &p_class_name) const {
#ifdef LIMBOAI_MODULE
	// Real implementation would check actual TaskDB
	return false; // Placeholder
#elif defined(LIMBOAI_GDEXTENSION)
	if (Engine::get_singleton()->has_singleton("LimboAI")) {
		// Check with GDExtension LimboAI
		return false; // Placeholder
	}
	return false;
#else
	return false; // Stub - no tasks registered
#endif
}

Array LimboTaskDB::get_registered_tasks() const {
	Array tasks;
#ifdef LIMBOAI_MODULE
	// Real implementation would return actual registered tasks
	tasks.push_back("BTActionAS_ActivateAbility");
	tasks.push_back("BTActionAS_DispatchEvent");
	tasks.push_back("BTActionAS_WaitForEvent");
	tasks.push_back("BTConditionAS_HasTag");
	tasks.push_back("BTConditionAS_CanActivate");
	tasks.push_back("BTConditionAS_EventOccurred");
	tasks.push_back("ASBridgeState");
#endif
	return tasks;
}

bool LimboTaskDB::is_available() const {
#ifdef LIMBOAI_MODULE
	return true;
#elif defined(LIMBOAI_GDEXTENSION)
	return Engine::get_singleton()->has_singleton("LimboAI");
#else
	return false;
#endif
}

LimboTaskDB::LimboTaskDB() {
	if (singleton == nullptr) {
		singleton = this;
	}
}

LimboTaskDB::~LimboTaskDB() {
	if (singleton == this) {
		singleton = nullptr;
	}
}

#endif // !LIMBOAI_MODULE && !LIMBOAI_GDEXTENSION
