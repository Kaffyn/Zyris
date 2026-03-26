/**************************************************************************/
/*  as_ability_spec.h                                                     */
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
#else
#include "modules/ability_system/resources/as_ability.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/variant.hpp>
#else
#include "core/object/ref_counted.h"
#include "core/templates/vector.h"
#include "core/variant/typed_array.h"
#include "core/variant/variant.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class ASComponent;
class ASEffectSpec;

class ASAbilitySpec : public RefCounted {
	GDCLASS(ASAbilitySpec, RefCounted);

protected:
	static void _bind_methods();

	friend class ASComponent;

private:
	Ref<ASAbility> ability;
	bool is_active = false;
	int level = 1;
	ObjectID owner_id;
	Vector<Ref<ASEffectSpec>> active_effects;

	float total_duration = 0.0f;
	float duration_remaining = 0.0f;

	// Hierarchical tracking
	ObjectID parent_id;
	Vector<Ref<ASAbilitySpec>> sub_specs;
	int current_phase_index = -1;

public:
	void init(Ref<ASAbility> p_ability, int p_lvl = 1);
	Ref<ASAbility> get_ability() const { return ability; }

	bool get_is_active() const { return is_active; }
	void set_is_active(bool p_active) { is_active = p_active; }

	int get_level() const { return level; }
	void set_level(int p_lvl) { level = p_lvl; }

	void set_owner(ASComponent *p_owner);
	ASComponent *get_owner() const;

	void set_total_duration(float p_duration) { total_duration = p_duration; }
	float get_total_duration() const { return total_duration; }

	void set_duration_remaining(float p_duration) { duration_remaining = p_duration; }
	float get_duration_remaining() const { return duration_remaining; }

	void set_parent_id(ObjectID p_id) { parent_id = p_id; }
	ObjectID get_parent_id() const { return parent_id; }

	void add_sub_spec(Ref<ASAbilitySpec> p_spec) { sub_specs.push_back(p_spec); }
	void remove_sub_spec(Ref<ASAbilitySpec> p_spec);
	TypedArray<ASAbilitySpec> get_sub_specs() const;

	void set_current_phase_index(int p_index) { current_phase_index = p_index; }
	int get_current_phase_index() const { return current_phase_index; }

	void activate(Object *p_target_node = nullptr);
	void deactivate();

	// Returns true if the ability should end.
	bool tick(float p_delta);

	// Runtime query methods
	float get_cooldown_duration() const;
	float get_cooldown_remaining() const;
	bool is_on_cooldown() const;

	float get_cost_amount(const StringName &p_attribute) const;

	TypedArray<ASEffect> get_effects_for_target(int p_target_type) const;

	// Effect tracking for cascading cancellation
	void add_active_effect(Ref<ASEffectSpec> p_spec);
	void remove_active_effect(Ref<ASEffectSpec> p_spec);
	TypedArray<ASEffectSpec> get_active_effects() const;
	void clear_active_effects();

	ASAbilitySpec();
	~ASAbilitySpec();
};
