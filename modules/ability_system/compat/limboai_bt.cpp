/**************************************************************************/
/*  limboai_bt.cpp                                                        */
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

#include "limboai_bt.h"

#if !defined(LIMBOAI_MODULE) && !defined(LIMBOAI_GDEXTENSION)

// ============================================================================
// BTTask Implementation
// ============================================================================

void BTTask::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_agent", "agent"), &BTTask::set_agent);
	ClassDB::bind_method(D_METHOD("get_agent"), &BTTask::get_agent);

	ClassDB::bind_method(D_METHOD("get_task_name"), &BTTask::get_task_name);
}

// ============================================================================
// BTAction Implementation
// ============================================================================

void BTAction::_bind_methods() {
	// Inherit BTTask bindings
}

// ============================================================================
// BTCondition Implementation
// ============================================================================

void BTCondition::_bind_methods() {
	// Inherit BTTask bindings
}

// ============================================================================
// BehaviorTree Implementation
// ============================================================================

void BehaviorTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_root_task", "task"), &BehaviorTree::set_root_task);
	ClassDB::bind_method(D_METHOD("get_root_task"), &BehaviorTree::get_root_task);

	ClassDB::bind_method(D_METHOD("set_description", "description"), &BehaviorTree::set_description);
	ClassDB::bind_method(D_METHOD("get_description"), &BehaviorTree::get_description);

	ClassDB::bind_method(D_METHOD("instantiate", "agent"), &BehaviorTree::instantiate);
}

Ref<BTInstance> BehaviorTree::instantiate(Node *p_agent) {
	if (!p_agent || !root_task.is_valid()) {
		return Ref<BTInstance>();
	}

	Ref<BTInstance> instance;
	instance.instantiate();
	instance->initialize(p_agent, this);
	return instance;
}

// ============================================================================
// BTInstance Implementation
// ============================================================================

void BTInstance::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize", "agent", "tree"), &BTInstance::initialize);
	ClassDB::bind_method(D_METHOD("tick", "delta"), &BTInstance::tick);
	ClassDB::bind_method(D_METHOD("reset"), &BTInstance::reset);

	ClassDB::bind_method(D_METHOD("get_agent"), &BTInstance::get_agent);
	ClassDB::bind_method(D_METHOD("get_behavior_tree"), &BTInstance::get_behavior_tree);
	ClassDB::bind_method(D_METHOD("get_last_status"), &BTInstance::get_last_status);
}

void BTInstance::initialize(Node *p_agent, const Ref<BehaviorTree> &p_tree) {
	agent = p_agent;
	behavior_tree = p_tree;
	current_task = p_tree->get_root_task();

	if (current_task.is_valid()) {
		current_task->set_agent(p_agent);
		current_task->_setup();
		current_task->_enter();
		last_status = BT::FRESH;
	}
}

BT::Status BTInstance::tick(double p_delta) {
	if (!current_task.is_valid()) {
		return BT::FAILURE;
	}

	last_status = current_task->_tick(p_delta);

	// Handle state transitions based on status
	if (last_status == BT::SUCCESS || last_status == BT::FAILURE) {
		current_task->_exit();
		current_task = Ref<BTTask>(); // Tree finished
	}

	return last_status;
}

void BTInstance::reset() {
	if (current_task.is_valid()) {
		current_task->_exit();
	}

	if (behavior_tree.is_valid()) {
		current_task = behavior_tree->get_root_task();
		if (current_task.is_valid()) {
			current_task->set_agent(agent);
			current_task->_setup();
			current_task->_enter();
			last_status = BT::FRESH;
		}
	}
}

#endif // !LIMBOAI_MODULE && !LIMBOAI_GDEXTENSION
