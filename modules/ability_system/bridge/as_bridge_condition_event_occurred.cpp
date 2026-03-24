/**************************************************************************/
/*  as_bridge_condition_event_occurred.cpp                                */
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

#include "as_bridge_condition_event_occurred.h"

#include "as_bridge.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

void BTConditionAS_EventOccurred::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_event_tag", "tag"), &BTConditionAS_EventOccurred::set_event_tag);
	ClassDB::bind_method(D_METHOD("get_event_tag"), &BTConditionAS_EventOccurred::get_event_tag);

	ClassDB::bind_method(D_METHOD("set_time_window", "seconds"), &BTConditionAS_EventOccurred::set_time_window);
	ClassDB::bind_method(D_METHOD("get_time_window"), &BTConditionAS_EventOccurred::get_time_window);

	ClassDB::bind_method(D_METHOD("set_asc_node_path", "path"), &BTConditionAS_EventOccurred::set_asc_node_path);
	ClassDB::bind_method(D_METHOD("get_asc_node_path"), &BTConditionAS_EventOccurred::get_asc_node_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "event_tag"), "set_event_tag", "get_event_tag");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time_window"), "set_time_window", "get_time_window");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "asc_node_path"), "set_asc_node_path", "get_asc_node_path");
}

BT::Status BTConditionAS_EventOccurred::_tick(double p_delta) {
	Node *agent = get_agent();
	if (!agent) {
		return BT::FAILURE;
	}

	ASComponent *asc = resolve_asc(agent, asc_node_path);
	if (!asc) {
		return BT::FAILURE;
	}

	if (event_tag.is_empty()) {
		return BT::FAILURE;
	}

	// Query the Events Historical buffer
	// See BUSINESS_RULES.md section 2.6.3
	bool occurred = asc->has_event_occurred(event_tag, time_window);
	return occurred ? BT::SUCCESS : BT::FAILURE;
}

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE
