/**************************************************************************/
/*  as_bridge_state.cpp                                                   */
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

#include "as_bridge_state.h"

#include "as_bridge.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

void ASBridgeState::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_asc_node_path", "path"), &ASBridgeState::set_asc_node_path);
	ClassDB::bind_method(D_METHOD("get_asc_node_path"), &ASBridgeState::get_asc_node_path);

	ClassDB::bind_method(D_METHOD("set_required_tags", "tags"), &ASBridgeState::set_required_tags);
	ClassDB::bind_method(D_METHOD("get_required_tags"), &ASBridgeState::get_required_tags);

	ClassDB::bind_method(D_METHOD("set_check_tags_on_enter", "check"), &ASBridgeState::set_check_tags_on_enter);
	ClassDB::bind_method(D_METHOD("get_check_tags_on_enter"), &ASBridgeState::get_check_tags_on_enter);

	ClassDB::bind_method(D_METHOD("set_enter_events", "events"), &ASBridgeState::set_enter_events);
	ClassDB::bind_method(D_METHOD("get_enter_events"), &ASBridgeState::get_enter_events);

	ClassDB::bind_method(D_METHOD("set_exit_events", "events"), &ASBridgeState::set_exit_events);
	ClassDB::bind_method(D_METHOD("get_exit_events"), &ASBridgeState::get_exit_events);

	ClassDB::bind_method(D_METHOD("set_update_events", "events"), &ASBridgeState::set_update_events);
	ClassDB::bind_method(D_METHOD("get_update_events"), &ASBridgeState::get_update_events);

	ClassDB::bind_method(D_METHOD("set_listen_events", "events"), &ASBridgeState::set_listen_events);
	ClassDB::bind_method(D_METHOD("get_listen_events"), &ASBridgeState::get_listen_events);

	ClassDB::bind_method(D_METHOD("set_transition_event", "event"), &ASBridgeState::set_transition_event);
	ClassDB::bind_method(D_METHOD("get_transition_event"), &ASBridgeState::get_transition_event);

	ClassDB::bind_method(D_METHOD("can_enter_state", "agent"), &ASBridgeState::can_enter_state);

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "asc_node_path"), "set_asc_node_path", "get_asc_node_path");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "required_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_required_tags", "get_required_tags");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "check_tags_on_enter"), "set_check_tags_on_enter", "get_check_tags_on_enter");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "enter_events", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_enter_events", "get_enter_events");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "exit_events", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_exit_events", "get_exit_events");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "update_events", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_update_events", "get_update_events");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "listen_events", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_listen_events", "get_listen_events");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "transition_event"), "set_transition_event", "get_transition_event");
}

void ASBridgeState::_setup() {
	// Called once when state is initialized
}

void ASBridgeState::_enter() {
	Node *agent = get_agent();
	if (!agent) {
		return;
	}

	ASComponent *asc = ASBridge::resolve_asc(agent, asc_node_path);
	if (!asc) {
		return;
	}

	// Dispatch enter events
	for (int i = 0; i < enter_events.size(); i++) {
		StringName event = enter_events[i];
		if (!event.is_empty()) {
			asc->dispatch_event(event, agent, 1.0f, Dictionary());
		}
	}
}

void ASBridgeState::_exit() {
	Node *agent = get_agent();
	if (!agent) {
		return;
	}

	ASComponent *asc = ASBridge::resolve_asc(agent, asc_node_path);
	if (!asc) {
		return;
	}

	// Dispatch exit events
	for (int i = 0; i < exit_events.size(); i++) {
		StringName event = exit_events[i];
		if (!event.is_empty()) {
			asc->dispatch_event(event, agent, 1.0f, Dictionary());
		}
	}
}

void ASBridgeState::_update(double p_delta) {
	Node *agent = get_agent();
	if (!agent) {
		return;
	}

	ASComponent *asc = ASBridge::resolve_asc(agent, asc_node_path);
	if (!asc) {
		return;
	}

	// Check if we should transition based on listened events
	if (!transition_event.is_empty()) {
		// Check if transition event occurred in the last frame window
		if (asc->has_event_occurred(transition_event, (float)p_delta * 2.0f)) {
			event_finished();
			return;
		}
	}

	// Note: update_events are intentionally NOT dispatched every frame.
	// Per BUSINESS_RULES.md §2.6 — Events are transient occurrences, not
	// continuous streams. Flooding the historical buffer defeats its purpose.
	// If continuous event dispatch is needed, use a dedicated ticker mechanism.
}

void ASBridgeState::_on_as_event_received(const StringName &p_event, const Dictionary &p_payload) {
	// Handle listened events and trigger transitions if needed
	if (p_event == transition_event) {
		// Trigger state transition via LimboState lifecycle
		event_finished();
	}
}

bool ASBridgeState::can_enter_state(Node *p_agent) const {
	if (!check_tags_on_enter || required_tags.is_empty()) {
		return true;
	}

	ASComponent *asc = ASBridge::resolve_asc(p_agent, asc_node_path);
	if (!asc) {
		return false;
	}

	// Check all required tags
	// Note: ASComponent::has_tag currently only supports exact match (1 arg)
	for (int i = 0; i < required_tags.size(); i++) {
		StringName tag = required_tags[i];
		if (!asc->has_tag(tag)) {
			return false;
		}
	}

	return true;
}

void ASBridgeState::dispatch_event(const StringName &p_event) {
	Node *agent = get_agent();
	if (!agent) {
		return;
	}

	ASComponent *asc = ASBridge::resolve_asc(agent, asc_node_path);
	if (!asc) {
		return;
	}

	asc->dispatch_event(p_event, agent, 1.0f, Dictionary());
}

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE
