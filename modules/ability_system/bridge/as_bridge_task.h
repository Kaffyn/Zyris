/**************************************************************************/
/*  as_bridge_task.h                                                      */
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

#include "../compat/limboai_bt.h"
#include "as_bridge.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

// LimboAI Task Macros
#ifndef TASK_CATEGORY
#define TASK_CATEGORY(m_category)                  \
	virtual String get_category() const override { \
		return m_category;                         \
	}
#endif

/**
 * Base class for all AS Bridge BT tasks.
 *
 * Provides common functionality for resolving ASComponents from agent nodes
 * and ASC node paths. All bridge tasks inherit from this base.
 */
class ASBridgeTask {
protected:
	/**
	 * Resolves the ASComponent for this task.
	 *
	 * Priority order:
	 * 1. If asc_node_path is set, resolve that path
	 * 2. If agent has ASC directly, use it
	 * 3. Search agent's children for ASC
	 * 4. Return nullptr if not found
	 */
	ASComponent *resolve_asc(Node *p_agent, const NodePath &p_asc_path) const;

	/**
	 * Validates that an event tag is registered as ASTagType::EVENT.
	 * Emits warning if not properly registered.
	 */
	bool validate_event_tag(const StringName &p_tag) const;

	/**
	 * Validates that a tag is registered (any type).
	 * Emits warning if tag doesn't exist in AbilitySystem.
	 */
	bool validate_tag(const StringName &p_tag) const;

public:
	virtual ~ASBridgeTask() = default;
};
