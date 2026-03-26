/**************************************************************************/
/*  limboai_hsm.h                                                         */
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

/**
 * compat/limboai_hsm.h
 * =============================================================================
 * Compatibility wrapper for LimboAI Hierarchical State Machine core classes
 *
 * When LIMBOAI_MODULE is defined, includes the real LimboAI headers.
 * When not available, provides stub implementations for compilation.
 * =============================================================================
 */

#ifndef COMPAT_LIMBOAI_HSM_H
#define COMPAT_LIMBOAI_HSM_H

#if defined(LIMBOAI_MODULE) || defined(LIMBOAI_GDEXTENSION)
// Real LimboAI HSM available — use actual headers
#ifdef LIMBOAI_GDEXTENSION
#include "hsm/limbo_hsm.h"
#include "hsm/limbo_state.h"
#else
#include "modules/limboai/hsm/limbo_hsm.h"
#include "modules/limboai/hsm/limbo_state.h"
#endif
#else
// Stub implementations follow

#include "../scene/as_component.h"
#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/variant.hpp>
using namespace godot;
#else
#include "core/object/ref_counted.h"
#include "core/string/string_name.h"
#include "core/templates/hash_map.h"
#include "core/templates/vector.h"
#include "core/variant/variant.h"
#include "scene/main/node.h"
#endif

// Forward declarations
class LimboState;
class LimboHSM;

/**
 * Update modes for LimboState execution
 */
enum class UpdateMode {
	IDLE, // Use _process
	PHYSICS, // Use _physics_process
	MANUAL // Manual tick calls
};

/**
 * LimboState - Base class for Hierarchical State Machine states
 *
 * Compatibility wrapper for LimboAI's LimboState.
 * Provides state lifecycle management and event handling.
 */
class LimboState : public RefCounted {
	GDCLASS(LimboState, RefCounted)

private:
	String state_name;
	UpdateMode update_mode = UpdateMode::IDLE;
	bool active = false;
	Node *agent = nullptr;
	Vector<Ref<LimboState>> substates;
	Ref<LimboState> current_substate;
	Ref<LimboState> parent_state;

protected:
	static void _bind_methods();

public:
	virtual ~LimboState() = default;

	// State lifecycle (to be overridden)
	virtual void _setup() {}
	virtual void _enter() {}
	virtual void _exit() {}
	virtual void _update(double p_delta) {}

	// State management
	virtual void set_active(bool p_active);
	virtual bool is_active() const { return active; }

	// Agent association
	virtual void set_agent(Node *p_agent) { agent = p_agent; }
	virtual Node *get_agent() const { return agent; }

	// Substate management (for composite states)
	virtual void add_substate(const Ref<LimboState> &p_substate);
	virtual void remove_substate(const Ref<LimboState> &p_substate);
	virtual Vector<Ref<LimboState>> get_substates() const { return substates; }

	// Transitions
	virtual void transition_to(const Ref<LimboState> &p_state);
	virtual void event_finished();

	// Configuration
	virtual void set_state_name(const String &p_name) { state_name = p_name; }
	virtual String get_state_name() const { return state_name; }

	virtual void set_update_mode(UpdateMode p_mode) { update_mode = p_mode; }
	virtual UpdateMode get_update_mode() const { return update_mode; }

	// Hierarchy
	virtual void set_parent_state(const Ref<LimboState> &p_parent) { parent_state = p_parent; }
	virtual Ref<LimboState> get_parent_state() const { return parent_state; }
};

/**
 * LimboHSM - Hierarchical State Machine
 *
 * Compatibility wrapper for LimboAI's LimboHSM.
 * Manages state transitions and execution hierarchy.
 */
class LimboHSM : public RefCounted {
	GDCLASS(LimboHSM, RefCounted)

private:
	Vector<Ref<LimboState>> states;
	Ref<LimboState> current_state;
	Ref<LimboState> initial_state;
	Node *agent = nullptr;
	UpdateMode update_mode = UpdateMode::IDLE;
	bool active = false;

protected:
	static void _bind_methods();

public:
	virtual ~LimboHSM() = default;

	// HSM lifecycle
	virtual void initialize(Node *p_agent);
	virtual void set_active(bool p_active);
	virtual bool is_active() const { return active; }

	// Tick/update
	virtual void tick(double p_delta);

	// State management
	virtual void add_state(const Ref<LimboState> &p_state);
	virtual void remove_state(const Ref<LimboState> &p_state);
	virtual Vector<Ref<LimboState>> get_states() const { return states; }

	// Transitions
	virtual void transition_to(const Ref<LimboState> &p_state);
	virtual void transition_to_by_name(const String &p_state_name);

	// Configuration
	virtual void set_initial_state(const Ref<LimboState> &p_state);
	virtual Ref<LimboState> get_initial_state() const { return initial_state; }

	virtual Ref<LimboState> get_current_state() const { return current_state; }
	virtual String get_current_state_name() const;

	virtual void set_update_mode(UpdateMode p_mode) { update_mode = p_mode; }
	virtual UpdateMode get_update_mode() const { return update_mode; }

	// Agent
	virtual void set_agent(Node *p_agent) { agent = p_agent; }
	virtual Node *get_agent() const { return agent; }

	// Utility
	virtual Ref<LimboState> find_state(const String &p_name) const;
	virtual void reset();

	LimboHSM() = default;
	~LimboHSM() = default;
};

#endif // !LIMBOAI_MODULE

#endif // COMPAT_LIMBOAI_HSM_H
