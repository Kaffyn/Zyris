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

/**
 * as_bridge.h
 * =============================================================================
 * AS Bridge - Integration between Ability System and LimboAI
 *
 * This module provides seamless integration between Ability System (AS) and
 * LimboAI Behavior Trees / Hierarchical State Machines.
 *
 * When LimboAI is detected at runtime, the bridge registers custom BT tasks
 * and LimboStates that interact with ASComponent, enabling AI-driven ability
 * activation, event dispatching, and state synchronization.
 *
 * Compilation Strategy:
 * - The compat layer always provides base class stubs (BTTask, BTAction,
 *   BTCondition, LimboState) so bridge classes always compile.
 * - At runtime, ASBridge::initialize() detects LimboAI presence and only
 *   registers tasks if LimboAI is actually loaded.
 * =============================================================================
 */

#ifndef AS_BRIDGE_H
#define AS_BRIDGE_H

// The compat layer guarantees that LimboAI base classes (BTTask, BTAction,
// BTCondition, LimboState, etc.) are always available — either as real
// LimboAI classes (when the module is present) or as compat stubs.
// Therefore, bridge classes can always compile.
#define AS_BRIDGE_LIMBOAI_AVAILABLE 1

#include "../scene/as_component.h"

// Compat wrappers for LimboAI integration
#include "../compat/limboai_task_db.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
using namespace godot;
#else
#include "core/config/engine.h"
#include "core/object/ref_counted.h"
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

#endif // AS_BRIDGE_H
