/**************************************************************************/
/*  as_bridge_action_wait_event.h                                         */
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
 * as_bridge_action_wait_event.h
 * =============================================================================
 * BTAction: Wait for AS Event
 *
 * Enters RUNNING state and waits for a specific ASEventTag to occur.
 * Uses ASComponent.has_event_occurred() to check the Events Historical buffer.
 *
 * Configuration:
 * - event_tag: StringName of event to wait for
 * - time_window: How far back to check (seconds)
 * - asc_node_path: Optional direct path to ASComponent
 *
 * Returns SUCCESS when event detected, RUNNING while waiting.
 * =============================================================================
 */

#ifndef AS_BRIDGE_ACTION_WAIT_EVENT_H
#define AS_BRIDGE_ACTION_WAIT_EVENT_H

#include "as_bridge_task.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

class BTActionAS_WaitForEvent : public BTAction, protected ASBridgeTask {
	GDCLASS(BTActionAS_WaitForEvent, BTAction)
	TASK_CATEGORY(Ability System)

private:
	StringName event_tag;
	float time_window = 0.5f;
	NodePath asc_node_path;
	bool triggered = false;

protected:
	static void _bind_methods();

	virtual void _enter() override;
	virtual BT::Status _tick(double p_delta) override;
	virtual void _exit() override;

public:
	void set_event_tag(const StringName &p_tag) { event_tag = p_tag; }
	StringName get_event_tag() const { return event_tag; }

	void set_time_window(float p_seconds) { time_window = p_seconds; }
	float get_time_window() const { return time_window; }

	void set_asc_node_path(const NodePath &p_path) { asc_node_path = p_path; }
	NodePath get_asc_node_path() const { return asc_node_path; }
};

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE

#endif // AS_BRIDGE_ACTION_WAIT_EVENT_H
