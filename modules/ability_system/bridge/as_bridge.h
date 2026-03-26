/**************************************************************************/
/*  as_bridge.h                                                           */
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

#pragma once

#define AS_BRIDGE_LIMBOAI_AVAILABLE 1

#include "../compat/limboai_task_db.h"
#include "../scene/as_component.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#else
#include "core/config/engine.h"
#include "core/object/ref_counted.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

/**
 * ASBridge is a singleton manager that detects LimboAI presence and registers
 * bridge tasks automatically. It serves as the central coordination point for
 * AS-LimboAI integration.
 */
class ASBridge : public RefCounted {
	GDCLASS(ASBridge, RefCounted)

private:
	static ASBridge *singleton;
	bool limboai_detected = false;
	bool initialized = false;

	/**
	 * Internal task registration methods.
	 * Called during initialize() if LimboAI is present.
	 */
	void _register_bt_actions();
	void _register_bt_conditions();
	void _register_limbo_states();

protected:
	static void _bind_methods();

public:
	/**
	 * Returns the global ASBridge singleton.
	 */
	static ASBridge *get_singleton() { return singleton; }

	/**
	 * Checks if LimboAI is available in the current engine instance.
	 * Returns true if LimboAI module/extension is loaded.
	 */
	static bool is_limboai_available();

	/**
	 * Returns true if LimboAI was detected and bridge tasks were registered.
	 */
	bool is_limboai_detected() const { return limboai_detected; }

	/**
	 * Initializes the bridge, detecting LimboAI and registering tasks.
	 * Called automatically during module initialization.
	 * Safe to call multiple times (idempotent).
	 */
	void initialize();

	/**
	 * Shuts down the bridge and unregisters tasks.
	 * Called during module termination.
	 */
	void shutdown();

	/**
	 * Resolves an ASComponent from various sources:
	 * - Direct ASC node path
	 * - Agent node with ASC child
	 * - Current scene context
	 *
	 * Returns nullptr if no valid ASC found.
	 */
	static ASComponent *resolve_asc(Node *p_agent, const NodePath &p_asc_path = NodePath());

	ASBridge();
	~ASBridge();
};
