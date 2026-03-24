/**************************************************************************/
/*  limboai_blackboard.cpp                                                */
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

#include "limboai_blackboard.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;
#endif

#if !defined(LIMBOAI_MODULE) && !defined(LIMBOAI_GDEXTENSION)

// ============================================================================
// BBVariable Implementation
// ============================================================================

BBVariable::BBVariable(const Variant &p_value) {
	value = p_value;
	type = value.get_type();
}

void BBVariable::set_value(const Variant &p_value) {
	value = p_value;
	if (type == Variant::NIL) {
		type = value.get_type();
	}
}

void BBVariable::bind_to_property(Object *p_object, const StringName &p_property) {
	if (!p_object) {
		return;
	}

	bound_object_id = p_object->get_instance_id();
	bound_property = p_property;
	is_bound = true;

	// Update initial value from bound property
	update_from_bound_property();
}

void BBVariable::unbind() {
	is_bound = false;
	bound_object_id = ObjectID();
	bound_property = StringName();
}

void BBVariable::update_from_bound_property() {
	if (!is_bound) {
		return;
	}

	Object *object = ObjectDB::get_instance(bound_object_id);
	if (!object) {
		unbind();
		return;
	}

	bool valid = false;
	Variant bound_value = object->get(bound_property, &valid);
	if (valid) {
		set_value(bound_value);
	}
}

// ============================================================================
// Blackboard Implementation
// ============================================================================

void Blackboard::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_var", "name", "value"), &Blackboard::set_var);
	ClassDB::bind_method(D_METHOD("get_var", "name", "default"), &Blackboard::get_var);
	ClassDB::bind_method(D_METHOD("has_var", "name"), &Blackboard::has_var);
	ClassDB::bind_method(D_METHOD("erase_var", "name"), &Blackboard::erase_var);

	ClassDB::bind_method(D_METHOD("bind_var_to_property", "var_name", "object", "property"), &Blackboard::bind_var_to_property);

	ClassDB::bind_method(D_METHOD("set_parent_blackboard", "parent"), &Blackboard::set_parent_blackboard);
	ClassDB::bind_method(D_METHOD("get_parent_blackboard"), &Blackboard::get_parent_blackboard);

	ClassDB::bind_method(D_METHOD("clear"), &Blackboard::clear);
	ClassDB::bind_method(D_METHOD("get_variable_names"), &Blackboard::get_variable_names);
}

void Blackboard::set_var(const StringName &p_name, const Variant &p_value) {
	BBVariable var(p_value);
	variables[p_name] = var;
}

Variant Blackboard::get_var(const StringName &p_name, const Variant &p_default) const {
	// Check local variables first
	const BBVariable *local_var = variables.getptr(p_name);
	if (local_var) {
		return local_var->get_value();
	}

	// Check parent blackboard (hierarchical scoping)
	if (parent_blackboard.is_valid()) {
		return parent_blackboard->get_var(p_name, p_default);
	}

	return p_default;
}

bool Blackboard::has_var(const StringName &p_name) const {
	if (variables.has(p_name)) {
		return true;
	}

	if (parent_blackboard.is_valid()) {
		return parent_blackboard->has_var(p_name);
	}

	return false;
}

void Blackboard::erase_var(const StringName &p_name) {
	variables.erase(p_name);
}

void Blackboard::bind_var_to_property(const StringName &p_var_name, Object *p_object, const StringName &p_property) {
	BBVariable *var = variables.getptr(p_var_name);
	if (var) {
		var->bind_to_property(p_object, p_property);
	}
}

void Blackboard::set_parent_blackboard(const Ref<Blackboard> &p_parent) {
	parent_blackboard = p_parent;
}

Array Blackboard::get_variable_names() const {
	Array names;
	for (const auto &E : variables) {
		names.push_back(E.key);
	}
	return names;
}

void Blackboard::clear() {
	variables.clear();
}

// ============================================================================
// BlackboardPlan Implementation
// ============================================================================

void BlackboardPlan::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_variable", "name", "default_value", "type", "hint", "hint_string"),
			&BlackboardPlan::add_variable, DEFVAL(Variant::NIL), DEFVAL(PROPERTY_HINT_NONE), DEFVAL(""));
	ClassDB::bind_method(D_METHOD("remove_variable", "name"), &BlackboardPlan::remove_variable);

	ClassDB::bind_method(D_METHOD("has_variable", "name"), &BlackboardPlan::has_variable);
	ClassDB::bind_method(D_METHOD("get_variable_names"), &BlackboardPlan::get_variable_names);

	ClassDB::bind_method(D_METHOD("add_parent_plan", "plan"), &BlackboardPlan::add_parent_plan);
	ClassDB::bind_method(D_METHOD("remove_parent_plan", "plan"), &BlackboardPlan::remove_parent_plan);
	ClassDB::bind_method(D_METHOD("get_parent_plans"), &BlackboardPlan::get_parent_plans);

	ClassDB::bind_method(D_METHOD("set_variable_mapping", "var_name", "parent_var"), &BlackboardPlan::set_variable_mapping);
	ClassDB::bind_method(D_METHOD("get_variable_mapping", "var_name"), &BlackboardPlan::get_variable_mapping);

	ClassDB::bind_method(D_METHOD("create_blackboard", "agent"), &BlackboardPlan::create_blackboard, DEFVAL(nullptr));
}

void BlackboardPlan::add_variable(const StringName &p_name, const Variant &p_default_value,
		Variant::Type p_type, PropertyHint p_hint, const String &p_hint_string) {
	BBVariable var(p_default_value);
	var.set_type(p_type);
	var.set_hint(p_hint);
	var.set_hint_string(p_hint_string);
	variables[p_name] = var;
}

void BlackboardPlan::remove_variable(const StringName &p_name) {
	variables.erase(p_name);
}

bool BlackboardPlan::has_variable(const StringName &p_name) const {
	return variables.has(p_name);
}

BBVariable BlackboardPlan::get_variable_definition(const StringName &p_name) const {
	const BBVariable *var = variables.getptr(p_name);
	if (var) {
		return *var;
	}
	return BBVariable();
}

Array BlackboardPlan::get_variable_names() const {
	Array names;
	for (const auto &E : variables) {
		names.push_back(E.key);
	}
	return names;
}

void BlackboardPlan::add_parent_plan(const Ref<BlackboardPlan> &p_plan) {
	if (p_plan.is_valid() && !parent_plans.has(p_plan)) {
		parent_plans.push_back(p_plan);
	}
}

void BlackboardPlan::remove_parent_plan(const Ref<BlackboardPlan> &p_plan) {
	parent_plans.erase(p_plan);
}

void BlackboardPlan::set_variable_mapping(const StringName &p_var_name, const StringName &p_parent_var) {
	variable_mappings[p_var_name] = p_parent_var;
}

StringName BlackboardPlan::get_variable_mapping(const StringName &p_var_name) const {
	const StringName *mapping = variable_mappings.getptr(p_var_name);
	return mapping ? *mapping : StringName();
}

Ref<Blackboard> BlackboardPlan::create_blackboard(Node *p_agent) {
	Ref<Blackboard> blackboard;
	blackboard.instantiate();
	blackboard->set_agent(p_agent);

	// Set up parent blackboards from parent plans
	for (int i = 0; i < parent_plans.size(); i++) {
		Ref<BlackboardPlan> parent_plan = parent_plans[i];
		if (parent_plan.is_valid()) {
			Ref<Blackboard> parent_bb = parent_plan->create_blackboard(p_agent);
			blackboard->set_parent_blackboard(parent_bb);
			break; // For simplicity, use first parent
		}
	}

	// Initialize variables with default values
	for (const auto &E : variables) {
		blackboard->set_var(E.key, E.second.get_value());
	}

	return blackboard;
}

#endif // !LIMBOAI_MODULE && !LIMBOAI_GDEXTENSION
