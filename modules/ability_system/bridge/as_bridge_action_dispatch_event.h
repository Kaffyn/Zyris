/**************************************************************************/
/*  as_bridge_action_dispatch_event.h                                     */
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
 * as_bridge_action_dispatch_event.h
 * =============================================================================
 * BTAction: Dispatch AS Event
 *
 * Dispatches an ASEventTag via ASComponent.dispatch_event().
 * Uses the AS Event system natively (Split Registry Pattern).
 *
 * Configuration:
 * - event_tag: StringName (must be ASTagType::EVENT)
 * - magnitude: Float value for event intensity
 * - custom_payload: Dictionary with additional data
 * - asc_node_path: Optional direct path to ASComponent
 * =============================================================================
 */

#ifndef AS_BRIDGE_ACTION_DISPATCH_EVENT_H
#define AS_BRIDGE_ACTION_DISPATCH_EVENT_H

#include "as_bridge_task.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

class BTActionAS_DispatchEvent : public BTAction, protected ASBridgeTask {
	GDCLASS(BTActionAS_DispatchEvent, BTAction)
	TASK_CATEGORY(Ability System)

private:
	StringName event_tag;
	float magnitude = 1.0f;
	Dictionary custom_payload;
	NodePath asc_node_path;

protected:
	static void _bind_methods();

	virtual BT::Status _tick(double p_delta) override;

public:
	void set_event_tag(const StringName &p_tag) { event_tag = p_tag; }
	StringName get_event_tag() const { return event_tag; }

	void set_magnitude(float p_value) { magnitude = p_value; }
	float get_magnitude() const { return magnitude; }

	void set_custom_payload(const Dictionary &p_payload) { custom_payload = p_payload; }
	Dictionary get_custom_payload() const { return custom_payload; }

	void set_asc_node_path(const NodePath &p_path) { asc_node_path = p_path; }
	NodePath get_asc_node_path() const { return asc_node_path; }
};

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE

#endif // AS_BRIDGE_ACTION_DISPATCH_EVENT_H
