/**************************************************************************/
/*  limboai_hsm.cpp                                                       */
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

#include "limboai_hsm.h"

#if !defined(LIMBOAI_MODULE) && !defined(LIMBOAI_GDEXTENSION)

// ============================================================================
// LimboState Implementation
// ============================================================================

void LimboState::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_active", "active"), &LimboState::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &LimboState::is_active);

	ClassDB::bind_method(D_METHOD("set_agent", "agent"), &LimboState::set_agent);
	ClassDB::bind_method(D_METHOD("get_agent"), &LimboState::get_agent);

	ClassDB::bind_method(D_METHOD("add_substate", "substate"), &LimboState::add_substate);
	ClassDB::bind_method(D_METHOD("remove_substate", "substate"), &LimboState::remove_substate);
	ClassDB::bind_method(D_METHOD("get_substates"), &LimboState::get_substates);

	ClassDB::bind_method(D_METHOD("transition_to", "state"), &LimboState::transition_to);
	ClassDB::bind_method(D_METHOD("event_finished"), &LimboState::event_finished);

	ClassDB::bind_method(D_METHOD("set_state_name", "name"), &LimboState::set_state_name);
	ClassDB::bind_method(D_METHOD("get_state_name"), &LimboState::get_state_name);

	ClassDB::bind_method(D_METHOD("set_update_mode", "mode"), &LimboState::set_update_mode);
	ClassDB::bind_method(D_METHOD("get_update_mode"), &LimboState::get_update_mode);
}

void LimboState::set_active(bool p_active) {
	if (active == p_active) {
		return;
	}

	active = p_active;

	if (p_active) {
		_enter();
	} else {
		_exit();
	}
}

void LimboState::add_substate(const Ref<LimboState> &p_substate) {
	if (p_substate.is_valid() && !substates.has(p_substate)) {
		p_substate->set_parent_state(Ref<LimboState>(this));
		p_substate->set_agent(agent);
		substates.push_back(p_substate);
	}
}

void LimboState::remove_substate(const Ref<LimboState> &p_substate) {
	if (p_substate.is_valid()) {
		substates.erase(p_substate);
		p_substate->set_parent_state(Ref<LimboState>());

		// Exit the substate if it was current
		if (current_substate == p_substate) {
			current_substate = Ref<LimboState>();
		}
	}
}

void LimboState::transition_to(const Ref<LimboState> &p_state) {
	if (!p_state.is_valid()) {
		return;
	}

	// Exit current substate
	if (current_substate.is_valid()) {
		current_substate->set_active(false);
		current_substate = Ref<LimboState>();
	}

	// Enter new substate
	if (substates.has(p_state)) {
		current_substate = p_state;
		current_substate->set_active(true);
	} else {
		// Delegate to parent state or HSM
		if (parent_state.is_valid()) {
			parent_state->transition_to(p_state);
		}
	}
}

void LimboState::event_finished() {
	if (current_substate.is_valid()) {
		current_substate->set_active(false);
		current_substate = Ref<LimboState>();
	}
}

// ============================================================================
// LimboHSM Implementation
// ============================================================================

void LimboHSM::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "agent"), &LimboHSM::initialize);
	ClassDB::bind_method(D_METHOD("set_active", "active"), &LimboHSM::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &LimboHSM::is_active);

	ClassDB::bind_method(D_METHOD("tick", "delta"), &LimboHSM::tick);

	ClassDB::bind_method(D_METHOD("add_state", "state"), &LimboHSM::add_state);
	ClassDB::bind_method(D_METHOD("remove_state", "state"), &LimboHSM::remove_state);
	ClassDB::bind_method(D_METHOD("get_states"), &LimboHSM::get_states);

	ClassDB::bind_method(D_METHOD("transition_to", "state"), &LimboHSM::transition_to);
	ClassDB::bind_method(D_METHOD("transition_to_by_name", "state_name"), &LimboHSM::transition_to_by_name);

	ClassDB::bind_method(D_METHOD("set_initial_state", "state"), &LimboHSM::set_initial_state);
	ClassDB::bind_method(D_METHOD("get_initial_state"), &LimboHSM::get_initial_state);

	ClassDB::bind_method(D_METHOD("get_current_state"), &LimboHSM::get_current_state);
	ClassDB::bind_method(D_METHOD("get_current_state_name"), &LimboHSM::get_current_state_name);

	ClassDB::bind_method(D_METHOD("set_update_mode", "mode"), &LimboHSM::set_update_mode);
	ClassDB::bind_method(D_METHOD("get_update_mode"), &LimboHSM::get_update_mode);

	ClassDB::bind_method(D_METHOD("set_agent", "agent"), &LimboHSM::set_agent);
	ClassDB::bind_method(D_METHOD("get_agent"), &LimboHSM::get_agent);

	ClassDB::bind_method(D_METHOD("find_state", "name"), &LimboHSM::find_state);
	ClassDB::bind_method(D_METHOD("reset"), &LimboHSM::reset);
}

void LimboHSM::initialize(Node *p_agent) {
	agent = p_agent;

	// Set agent for all states
	for (int i = 0; i < states.size(); i++) {
		states[i]->set_agent(p_agent);
		states[i]->_setup();
	}

	// Enter initial state if set
	if (initial_state.is_valid()) {
		transition_to(initial_state);
	}
}

void LimboHSM::set_active(bool p_active) {
	if (active == p_active) {
		return;
	}

	active = p_active;

	if (p_active && initial_state.is_valid() && !current_state.is_valid()) {
		transition_to(initial_state);
	} else if (!p_active && current_state.is_valid()) {
		current_state->set_active(false);
		current_state = Ref<LimboState>();
	}
}

void LimboHSM::tick(double p_delta) {
	if (!active || !current_state.is_valid()) {
		return;
	}

	current_state->_update(p_delta);
}

void LimboHSM::add_state(const Ref<LimboState> &p_state) {
	if (p_state.is_valid() && !states.has(p_state)) {
		states.push_back(p_state);
		if (agent) {
			p_state->set_agent(agent);
			p_state->_setup();
		}
	}
}

void LimboHSM::remove_state(const Ref<LimboState> &p_state) {
	if (p_state.is_valid()) {
		states.erase(p_state);

		if (current_state == p_state) {
			current_state->set_active(false);
			current_state = Ref<LimboState>();
		}

		if (initial_state == p_state) {
			initial_state = Ref<LimboState>();
		}
	}
}

void LimboHSM::transition_to(const Ref<LimboState> &p_state) {
	if (!p_state.is_valid()) {
		return;
	}

	// Exit current state
	if (current_state.is_valid()) {
		current_state->set_active(false);
	}

	// Enter new state
	current_state = p_state;
	current_state->set_active(true);
}

void LimboHSM::transition_to_by_name(const String &p_state_name) {
	Ref<LimboState> state = find_state(p_state_name);
	if (state.is_valid()) {
		transition_to(state);
	}
}

void LimboHSM::set_initial_state(const Ref<LimboState> &p_state) {
	initial_state = p_state;

	// If no current state and we're active, transition immediately
	if (active && !current_state.is_valid() && initial_state.is_valid()) {
		transition_to(initial_state);
	}
}

String LimboHSM::get_current_state_name() const {
	return current_state.is_valid() ? current_state->get_state_name() : String();
}

Ref<LimboState> LimboHSM::find_state(const String &p_name) const {
	for (int i = 0; i < states.size(); i++) {
		if (states[i]->get_state_name() == p_name) {
			return states[i];
		}
	}
	return Ref<LimboState>();
}

void LimboHSM::reset() {
	if (current_state.is_valid()) {
		current_state->set_active(false);
		current_state = Ref<LimboState>();
	}

	if (active && initial_state.is_valid()) {
		transition_to(initial_state);
	}
}

#endif // !LIMBOAI_MODULE && !LIMBOAI_GDEXTENSION
