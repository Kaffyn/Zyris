/**************************************************************************/
/*  as_attribute_set.h                                                    */
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

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include "src/resources/as_ability.h"
#include "src/resources/as_attribute.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#else
#include "core/io/resource.h"
#include "core/templates/hash_map.h"
#include "core/variant/typed_array.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_attribute.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

/**
 * ASAttributeSet
 *
 * Responsible for initializing the actor. Defines:
 * - Attribute definitions
 * - Which abilities from the catalog are already unlocked
 *
 * The Container stores this set; the ASC uses it to initialize the entity.
 */
class ASAttributeSet : public Resource {
	GDCLASS(ASAttributeSet, Resource);

public:
	struct AttributeValue {
		float base_value = 0.0f;
		float current_value = 0.0f;
	};

protected:
	static void _bind_methods();

	// Attribute definitions (configurable Resources)
	TypedArray<ASAttribute> attribute_definitions;

	// Abilities already unlocked from the catalog
	TypedArray<ASAbility> unlocked_abilities;

	// Runtime values (maintained for performance)
	HashMap<StringName, AttributeValue> attributes;

	// Attribute drivers: [{source, destination, ratio}]
	TypedArray<Dictionary> attribute_drivers;

	void _apply_drivers_for_source(const StringName &p_source, float p_new_base);

public:
	// Resource-based API
	void add_attribute_definition(Ref<ASAttribute> p_attribute);
	void remove_attribute_definition(const StringName &p_name);
	Ref<ASAttribute> get_attribute_definition(const StringName &p_name) const;
	void set_attribute_definitions(const TypedArray<ASAttribute> &p_definitions);
	TypedArray<ASAttribute> get_attribute_definitions() const;

	// Unlocked abilities (permitted for use from the catalog)
	void set_unlocked_abilities(const TypedArray<ASAbility> &p_abilities) { unlocked_abilities = p_abilities; }
	TypedArray<ASAbility> get_unlocked_abilities() const { return unlocked_abilities; }

	void set_attribute_drivers(const TypedArray<Dictionary> &p_drivers);
	TypedArray<Dictionary> get_attribute_drivers() const { return attribute_drivers; }

	// Value API (validated via Resource)
	void set_attribute_base_value(const StringName &p_name, float p_value);
	float get_attribute_base_value(const StringName &p_name) const;

	void set_attribute_current_value(const StringName &p_name, float p_value);
	float get_attribute_current_value(const StringName &p_name) const;

	bool has_attribute(const StringName &p_name) const;
	TypedArray<StringName> get_attribute_list() const;
	void reset_current_values();

	enum ModifierType {
		MODIFIER_ADD,
		MODIFIER_MULTIPLY,
	};

	void add_modifier(const StringName &p_name, float p_value, ModifierType p_type = MODIFIER_ADD);
	void remove_modifier(const StringName &p_name, float p_value, ModifierType p_type = MODIFIER_ADD);

	float get_attribute_value(const StringName &p_name) const;

	// Signal handlers for attribute changes
	void _on_attribute_value_changed(float p_old_value, float p_new_value, const StringName &p_name);
	void _on_attribute_limits_changed(float p_min_value, float p_max_value, const StringName &p_name);

	// Helper to find attribute by name
	Ref<ASAttribute> _find_attribute_by_name(const StringName &p_name) const;

	// Signal binding helpers
	void _bind_attribute_signals(Ref<ASAttribute> p_attribute);
	void _unbind_attribute_signals(Ref<ASAttribute> p_attribute);

	ASAttributeSet();
	~ASAttributeSet();
};

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

VARIANT_ENUM_CAST(ASAttributeSet::ModifierType);
