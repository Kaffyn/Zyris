/**************************************************************************/
/*  as_bridge_task.cpp                                                    */
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

#include "as_bridge_task.h"

#include "../compat/limboai_bt.h"
#include "../core/ability_system.h"

ASComponent *ASBridgeTask::resolve_asc(Node *p_agent, const NodePath &p_asc_path) const {
	return ASBridge::resolve_asc(p_agent, p_asc_path);
}

bool ASBridgeTask::validate_tag(const StringName &p_tag) const {
	if (p_tag.is_empty()) {
		return false;
	}

	AbilitySystem *as = AbilitySystem::get_singleton();
	if (!as) {
		return false;
	}

	return as->is_tag_registered(p_tag);
}

bool ASBridgeTask::validate_event_tag(const StringName &p_tag) const {
	if (p_tag.is_empty()) {
		return false;
	}

	AbilitySystem *as = AbilitySystem::get_singleton();
	if (!as) {
		return false;
	}

	// Check that tag exists and is of type EVENT
	if (!as->is_tag_registered(p_tag)) {
		return false;
	}

	// Validate that the tag follows EVENT naming convention (Event.*)
	String tag_str = String(p_tag);
	return tag_str.begins_with("Event.");
}
