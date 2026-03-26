/**************************************************************************/
/*  as_bridge_state.h                                                     */
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

#include "../compat/limboai_hsm.h"
#include "as_bridge_task.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

#if AS_BRIDGE_LIMBOAI_AVAILABLE

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <limboai/hsm/limbo_state.h>
#else
#include "modules/limboai/hsm/limbo_state.h"
#endif

/**
 * ASBridgeState
 *
 * A LimboState that integrates with Ability System. Provides automatic
 * event handling and tag synchronization for HSM states.
 */
class ASBridgeState : public LimboState {
	GDCLASS(ASBridgeState, LimboState)

private:
	NodePath asc_node_path;
	TypedArray<StringName> required_tags;
	bool check_tags_on_enter = true;

	TypedArray<StringName> enter_events;
	TypedArray<StringName> exit_events;
	TypedArray<StringName> update_events;
	TypedArray<StringName> listen_events;

	StringName transition_event;

protected:
	static void _bind_methods();

public:
	void set_asc_node_path(const NodePath &p_path) { asc_node_path = p_path; }
	NodePath get_asc_node_path() const { return asc_node_path; }

	void set_required_tags(const TypedArray<StringName> &p_tags) { required_tags = p_tags; }
	TypedArray<StringName> get_required_tags() const { return required_tags; }

	void set_check_tags_on_enter(bool p_check) { check_tags_on_enter = p_check; }
	bool get_check_tags_on_enter() const { return check_tags_on_enter; }

	void set_enter_events(const TypedArray<StringName> &p_events) { enter_events = p_events; }
	TypedArray<StringName> get_enter_events() const { return enter_events; }

	void set_exit_events(const TypedArray<StringName> &p_events) { exit_events = p_events; }
	TypedArray<StringName> get_exit_events() const { return exit_events; }

	void set_update_events(const TypedArray<StringName> &p_events) { update_events = p_events; }
	TypedArray<StringName> get_update_events() const { return update_events; }

	void set_listen_events(const TypedArray<StringName> &p_events) { listen_events = p_events; }
	TypedArray<StringName> get_listen_events() const { return listen_events; }

	void set_transition_event(const StringName &p_event) { transition_event = p_event; }
	StringName get_transition_event() const { return transition_event; }

	/**
	 * HSM Lifecycle overrides
	 */
	virtual void _setup() override;
	virtual void _enter() override;
	virtual void _exit() override;
	virtual void _update(double p_delta) override;

	/**
	 * Event listeners
	 */
	void _on_as_event_received(const StringName &p_event, const Dictionary &p_payload);

	/**
	 * Utilities
	 */
	bool can_enter_state(Node *p_agent) const;
	void dispatch_event(const StringName &p_event);

	ASBridgeState() = default;
	~ASBridgeState() = default;
};

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE
