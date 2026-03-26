/**************************************************************************/
/*  limboai_blackboard.h                                                  */
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

/**
 * compat/limboai_blackboard.h
 * =============================================================================
 * Compatibility wrapper for LimboAI Blackboard
 *
 * When LIMBOAI_MODULE is defined, includes the real LimboAI headers.
 * When not available, provides stub implementations for compilation.
 * =============================================================================
 */

#ifndef COMPAT_LIMBOAI_BLACKBOARD_H
#define COMPAT_LIMBOAI_BLACKBOARD_H

#if defined(LIMBOAI_MODULE) || defined(LIMBOAI_GDEXTENSION)
// Real LimboAI available — use actual headers
#ifdef LIMBOAI_GDEXTENSION
#include "blackboard/bb_variable.h"
#include "blackboard/blackboard.h"
#include "blackboard/blackboard_plan.h"
#else
#include "modules/limboai/blackboard/bb_variable.h"
#include "modules/limboai/blackboard/blackboard.h"
#include "modules/limboai/blackboard/blackboard_plan.h"
#endif
#else
// Stub implementations follow

#include "../scene/as_component.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/variant.hpp>
using namespace godot;
#else
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/string/string_name.h"
#include "core/templates/hash_map.h"
#include "core/variant/dictionary.h"
#include "core/variant/node_path.h"
#include "core/variant/variant.h"
#include "scene/main/node.h"
#endif

/**
 * BBVariable - Typed variable storage with binding support
 *
 * This is a compatibility wrapper for LimboAI's BBVariable.
 * When LimboAI is available, uses the actual implementation.
 * When not available, provides a simplified version for AS integration.
 */
class BBVariable {
private:
	Variant value;
	Variant::Type type = Variant::NIL;
	PropertyHint hint = PROPERTY_HINT_NONE;
	String hint_string;

	// Binding support
	ObjectID bound_object_id;
	StringName bound_property;
	bool is_bound = false;

public:
	BBVariable() = default;
	BBVariable(const Variant &p_value);

	void set_value(const Variant &p_value);
	Variant get_value() const { return value; }

	void set_type(Variant::Type p_type) { type = p_type; }
	Variant::Type get_type() const { return type; }

	void set_hint(PropertyHint p_hint) { hint = p_hint; }
	PropertyHint get_hint() const { return hint; }

	void set_hint_string(const String &p_string) { hint_string = p_string; }
	String get_hint_string() const { return hint_string; }

	// Binding methods
	void bind_to_property(Object *p_object, const StringName &p_property);
	void unbind();
	bool is_bound_to_property() const { return is_bound; }

	void update_from_bound_property();
};

/**
 * Blackboard - Hierarchical typed memory system
 *
 * Compatibility wrapper for LimboAI's Blackboard.
 * Provides variable storage with hierarchical scoping and property binding.
 */
class Blackboard : public RefCounted {
	GDCLASS(Blackboard, RefCounted)

private:
	HashMap<StringName, BBVariable> variables;
	Ref<Blackboard> parent_blackboard; // Hierarchical scoping

	// Auto-binding support
	Node *agent_node = nullptr;

protected:
	static void _bind_methods();

public:
	// Variable management
	void set_var(const StringName &p_name, const Variant &p_value);
	Variant get_var(const StringName &p_name, const Variant &p_default = Variant()) const;
	bool has_var(const StringName &p_name) const;
	void erase_var(const StringName &p_name);

	// Binding to Node properties
	void bind_var_to_property(const StringName &p_var_name, Object *p_object, const StringName &p_property);

	// Hierarchical scoping
	void set_parent_blackboard(const Ref<Blackboard> &p_parent);
	Ref<Blackboard> get_parent_blackboard() const { return parent_blackboard; }

	// Agent association
	void set_agent(Node *p_agent) { agent_node = p_agent; }
	Node *get_agent() const { return agent_node; }

	// Utility methods
	void clear();
	Array get_variable_names() const;

	Blackboard() = default;
	~Blackboard() = default;
};

/**
 * BlackboardPlan - Blueprint for Blackboard configuration
 *
 * Compatibility wrapper for LimboAI's BlackboardPlan.
 * Defines variable structure and inheritance for Blackboard instances.
 */
class BlackboardPlan : public Resource {
	GDCLASS(BlackboardPlan, Resource)

private:
	HashMap<StringName, BBVariable> variables;
	Array parent_plans; // Inheritance
	HashMap<StringName, StringName> variable_mappings; // Map to parent scopes

protected:
	static void _bind_methods();

public:
	// Variable definition
	void add_variable(const StringName &p_name, const Variant &p_default_value,
			Variant::Type p_type = Variant::NIL,
			PropertyHint p_hint = PROPERTY_HINT_NONE,
			const String &p_hint_string = "");
	void remove_variable(const StringName &p_name);

	// Variable queries
	bool has_variable(const StringName &p_name) const;
	BBVariable get_variable_definition(const StringName &p_name) const;
	Array get_variable_names() const;

	// Inheritance
	void add_parent_plan(const Ref<BlackboardPlan> &p_plan);
	void remove_parent_plan(const Ref<BlackboardPlan> &p_plan);
	Array get_parent_plans() const { return parent_plans; }

	// Variable mapping to parent scopes
	void set_variable_mapping(const StringName &p_var_name, const StringName &p_parent_var);
	StringName get_variable_mapping(const StringName &p_var_name) const;

	// Blackboard creation
	Ref<Blackboard> create_blackboard(Node *p_agent = nullptr);

	BlackboardPlan() = default;
	~BlackboardPlan() = default;
};

#endif // !LIMBOAI_MODULE

#endif // COMPAT_LIMBOAI_BLACKBOARD_H
