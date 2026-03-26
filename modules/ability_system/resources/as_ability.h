/**************************************************************************/
/*  as_ability.h                                                          */
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
#include "src/resources/as_cue.h"
#include "src/resources/as_effect.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/core/type_info.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/variant.hpp>
#else
#include "core/io/resource.h"
#include "core/object/gdvirtual.gen.inc"
#include "core/object/object.h"
#include "core/variant/typed_array.h"
#include "core/variant/variant.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class ASComponent;
class ASAbilitySpec;

class ASAbility : public Resource {
	GDCLASS(ASAbility, Resource);
	friend class ASAbilitySpec;

public:
	enum DurationPolicy {
		POLICY_INSTANT,
		POLICY_DURATION,
		POLICY_INFINITE
	};

	enum TriggerType {
		TRIGGER_ON_TAG_ADDED,
		TRIGGER_ON_TAG_REMOVED,
		TRIGGER_ON_EVENT, // Triggered when a matching ASEventTag is dispatched on this component
	};

	// GDScript virtuals
	GDVIRTUAL2(_on_activate_ability, Object *, Ref<RefCounted>);
	GDVIRTUAL2RC(bool, _on_can_activate_ability, Object *, Ref<RefCounted>);
	GDVIRTUAL2(_on_end_ability, Object *, Ref<RefCounted>);

public:
	static void _bind_methods();

	void set_ability_name(const String &p_name);
	String get_ability_name() const;

	void set_ability_tag(const StringName &p_tag);
	StringName get_ability_tag() const;

	void set_activation_owned_tags(const TypedArray<StringName> &p_tags);
	TypedArray<StringName> get_activation_owned_tags() const;

	void set_activation_required_all_tags(const TypedArray<StringName> &p_tags);
	TypedArray<StringName> get_activation_required_all_tags() const;

	void set_activation_required_any_tags(const TypedArray<StringName> &p_tags);
	TypedArray<StringName> get_activation_required_any_tags() const;

	void set_activation_blocked_any_tags(const TypedArray<StringName> &p_tags);
	TypedArray<StringName> get_activation_blocked_any_tags() const;

	void set_activation_blocked_all_tags(const TypedArray<StringName> &p_tags);
	TypedArray<StringName> get_activation_blocked_all_tags() const;

	void set_activation_cancel_tags(const TypedArray<StringName> &p_tags);
	TypedArray<StringName> get_activation_cancel_tags() const;

	void set_cues(const TypedArray<ASCue> &p_cues);
	TypedArray<ASCue> get_cues() const;

	void set_events_on_activate(const TypedArray<StringName> &p_events);
	TypedArray<StringName> get_events_on_activate() const;

	void set_events_on_end(const TypedArray<StringName> &p_events);
	TypedArray<StringName> get_events_on_end() const;

	void set_effects(const TypedArray<ASEffect> &p_effects);
	TypedArray<ASEffect> get_effects() const;

protected:
	String ability_name;
	StringName ability_tag;
	TypedArray<StringName> activation_owned_tags;
	TypedArray<StringName> activation_required_all_tags;
	TypedArray<StringName> activation_required_any_tags;
	TypedArray<StringName> activation_blocked_any_tags;
	TypedArray<StringName> activation_blocked_all_tags;
	TypedArray<StringName> activation_cancel_tags;
	TypedArray<ASCue> cues;
	TypedArray<StringName> events_on_activate;
	TypedArray<StringName> events_on_end;

	// Triggers (Dictionary: {tag: StringName, type: TriggerType})
	TypedArray<Dictionary> triggers;

	// The gameplay effects (e.g., damage, status)
	TypedArray<ASEffect> effects;

	// Native costs (Dictionary: {attribute: StringName, amount: float})
	TypedArray<Dictionary> costs;
	bool use_custom_costs = false;

	// Native requirements (Dictionary: {attribute: StringName, amount: float}) - Checked but not consumed
	TypedArray<Dictionary> requirements;

	// Native duration
	DurationPolicy duration_policy = POLICY_INSTANT;
	float ability_duration = 0.0;
	bool use_custom_duration = false;

	// Native cooldown
	float cooldown_duration = 0.0;
	TypedArray<StringName> cooldown_tags;
	bool use_custom_cooldown = false;

	// Hierarchical and Phases
	TypedArray<ASAbility> sub_abilities;
	TypedArray<StringName> sub_abilities_auto_activate;
	TypedArray<ASAbility> phases;

public:
	// Duration methods
	void set_duration_policy(DurationPolicy p_policy);
	DurationPolicy get_duration_policy() const;

	void set_ability_duration(float p_duration);
	float get_ability_duration() const;
	void set_use_custom_duration(bool p_use);
	bool get_use_custom_duration() const;

	float calculate_ability_duration(ASComponent *p_owner) const;

	// Cooldown methods
	void set_cooldown_duration(float p_duration);
	float get_cooldown_duration() const;
	void set_cooldown_tags(const TypedArray<StringName> &p_tags);
	TypedArray<StringName> get_cooldown_tags() const;
	void set_use_custom_cooldown(bool p_use);
	bool get_use_custom_cooldown() const;

	// Cost methods
	void set_costs(const TypedArray<Dictionary> &p_costs);
	TypedArray<Dictionary> get_costs() const;
	void set_use_custom_costs(bool p_use);
	bool get_use_custom_costs() const;

	void set_triggers(const TypedArray<Dictionary> &p_triggers);
	TypedArray<Dictionary> get_triggers() const;
	void add_trigger(const StringName &p_tag, TriggerType p_type);

	void set_sub_abilities(const TypedArray<ASAbility> &p_abilities) { sub_abilities = p_abilities; }
	TypedArray<ASAbility> get_sub_abilities() const { return sub_abilities; }

	void set_sub_abilities_auto_activate(const TypedArray<StringName> &p_tags) { sub_abilities_auto_activate = p_tags; }
	TypedArray<StringName> get_sub_abilities_auto_activate() const { return sub_abilities_auto_activate; }

	void set_phases(const TypedArray<ASAbility> &p_phases);
	TypedArray<ASAbility> get_phases() const;

	// Requirement methods
	void set_requirements(const TypedArray<Dictionary> &p_requirements);
	TypedArray<Dictionary> get_requirements() const;
	int get_requirement_count() const { return requirements.size(); }
	void add_requirement(const StringName &p_attribute, float p_amount);
	bool remove_requirement(const StringName &p_attribute);
	float get_requirement_amount(const StringName &p_attribute) const;
	bool can_satisfy_requirements(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec = nullptr) const;

	void add_cost(const StringName &p_attribute, float p_amount);
	bool remove_cost(const StringName &p_attribute);
	float get_cost_amount(const StringName &p_attribute) const;

	bool can_afford_costs(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec = nullptr) const;
	void apply_costs(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec = nullptr) const;

	virtual bool can_activate_ability(ASComponent *p_owner, const Ref<ASAbilitySpec> &p_spec) const;
	virtual void activate_ability(ASComponent *p_owner, const Ref<ASAbilitySpec> &p_spec, Object *p_target_node = nullptr);
	virtual void end_ability(ASComponent *p_owner, const Ref<ASAbilitySpec> &p_spec);

	ASAbility();
	~ASAbility();
};

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

VARIANT_ENUM_CAST(ASAbility::DurationPolicy);
VARIANT_ENUM_CAST(ASAbility::TriggerType);
