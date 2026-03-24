/**************************************************************************/
/*  as_bridge_action_dispatch_event.cpp                                   */
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

#include "as_bridge_action_dispatch_event.h"

#include "as_bridge.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

void BTActionAS_DispatchEvent::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_event_tag", "tag"), &BTActionAS_DispatchEvent::set_event_tag);
	ClassDB::bind_method(D_METHOD("get_event_tag"), &BTActionAS_DispatchEvent::get_event_tag);

	ClassDB::bind_method(D_METHOD("set_magnitude", "value"), &BTActionAS_DispatchEvent::set_magnitude);
	ClassDB::bind_method(D_METHOD("get_magnitude"), &BTActionAS_DispatchEvent::get_magnitude);

	ClassDB::bind_method(D_METHOD("set_custom_payload", "payload"), &BTActionAS_DispatchEvent::set_custom_payload);
	ClassDB::bind_method(D_METHOD("get_custom_payload"), &BTActionAS_DispatchEvent::get_custom_payload);

	ClassDB::bind_method(D_METHOD("set_asc_node_path", "path"), &BTActionAS_DispatchEvent::set_asc_node_path);
	ClassDB::bind_method(D_METHOD("get_asc_node_path"), &BTActionAS_DispatchEvent::get_asc_node_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "event_tag"), "set_event_tag", "get_event_tag");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "magnitude"), "set_magnitude", "get_magnitude");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "custom_payload"), "set_custom_payload", "get_custom_payload");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "asc_node_path"), "set_asc_node_path", "get_asc_node_path");
}

BT::Status BTActionAS_DispatchEvent::_tick(double p_delta) {
	Node *agent = get_agent();
	if (!agent) {
		WARN_PRINT("BTActionAS_DispatchEvent: No agent assigned");
		return BT::FAILURE;
	}

	ASComponent *asc = resolve_asc(agent, asc_node_path);
	if (!asc) {
		WARN_PRINT("BTActionAS_DispatchEvent: No ASComponent found on agent");
		return BT::FAILURE;
	}

	if (event_tag.is_empty()) {
		WARN_PRINT("BTActionAS_DispatchEvent: No event_tag specified");
		return BT::FAILURE;
	}

	// Validate event tag
	if (!validate_event_tag(event_tag)) {
		WARN_PRINT(vformat("BTActionAS_DispatchEvent: Event tag '%s' not registered as ASTagType::EVENT", event_tag));
		return BT::FAILURE;
	}

	// Dispatch event using AS Event system
	// See BUSINESS_RULES.md section 2.6 - Events are imperative
	asc->dispatch_event(event_tag, agent, magnitude, custom_payload);

	return BT::SUCCESS; // Dispatch is always immediate
}

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE
