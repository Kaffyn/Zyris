/**************************************************************************/
/*  as_ability_spec.cpp                                                   */
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

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include "src/core/as_ability_spec.h"
#include "src/core/as_effect_spec.h"
#include "src/scene/as_component.h"
#else
#include "modules/ability_system/core/as_ability_spec.h"
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/scene/as_component.h"
#endif

namespace godot {

void ASAbilitySpec::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "ability", "level"), &ASAbilitySpec::init, DEFVAL(1));
	ClassDB::bind_method(D_METHOD("get_ability"), &ASAbilitySpec::get_ability);
	ClassDB::bind_method(D_METHOD("get_active_effects"), &ASAbilitySpec::get_active_effects);
	ClassDB::bind_method(D_METHOD("get_current_phase_index"), &ASAbilitySpec::get_current_phase_index);
	ClassDB::bind_method(D_METHOD("get_is_active"), &ASAbilitySpec::get_is_active);
	ClassDB::bind_method(D_METHOD("set_is_active", "active"), &ASAbilitySpec::set_is_active);
	ClassDB::bind_method(D_METHOD("get_level"), &ASAbilitySpec::get_level);
	ClassDB::bind_method(D_METHOD("set_level", "level"), &ASAbilitySpec::set_level);
	ClassDB::bind_method(D_METHOD("get_owner"), &ASAbilitySpec::get_owner);

	ClassDB::bind_method(D_METHOD("get_total_duration"), &ASAbilitySpec::get_total_duration);
	ClassDB::bind_method(D_METHOD("set_total_duration", "duration"), &ASAbilitySpec::set_total_duration);
	ClassDB::bind_method(D_METHOD("get_duration_remaining"), &ASAbilitySpec::get_duration_remaining);
	ClassDB::bind_method(D_METHOD("set_duration_remaining", "duration"), &ASAbilitySpec::set_duration_remaining);
	ClassDB::bind_method(D_METHOD("tick", "delta"), &ASAbilitySpec::tick);

	ClassDB::bind_method(D_METHOD("get_cooldown_duration"), &ASAbilitySpec::get_cooldown_duration);
	ClassDB::bind_method(D_METHOD("get_cooldown_remaining"), &ASAbilitySpec::get_cooldown_remaining);
	ClassDB::bind_method(D_METHOD("is_on_cooldown"), &ASAbilitySpec::is_on_cooldown);
	ClassDB::bind_method(D_METHOD("get_cost_amount", "attribute"), &ASAbilitySpec::get_cost_amount);
	ClassDB::bind_method(D_METHOD("get_effects_for_target", "target_type"), &ASAbilitySpec::get_effects_for_target);

	ClassDB::bind_method(D_METHOD("add_sub_spec", "spec"), &ASAbilitySpec::add_sub_spec);
	ClassDB::bind_method(D_METHOD("remove_sub_spec", "spec"), &ASAbilitySpec::remove_sub_spec);
	ClassDB::bind_method(D_METHOD("get_sub_specs"), &ASAbilitySpec::get_sub_specs);
}

void ASAbilitySpec::init(Ref<ASAbility> p_ability, int p_lvl) {
	ability = p_ability;
	level = p_lvl;
}

void ASAbilitySpec::set_owner(ASComponent *p_owner) {
	if (p_owner) {
		owner_id = p_owner->get_instance_id();
	} else {
		owner_id = ObjectID();
	}
}

ASComponent *ASAbilitySpec::get_owner() const {
	if (owner_id.is_null()) {
		return nullptr;
	}
	return Object::cast_to<ASComponent>(ObjectDB::get_instance(owner_id));
}

bool ASAbilitySpec::tick(float p_delta) {
	if (ability.is_null()) {
		return true;
	}

	if (ability->get_duration_policy() == ASAbility::POLICY_DURATION) {
		duration_remaining -= p_delta;
		if (duration_remaining <= 0.0f) {
			duration_remaining = 0.0f;
			return true;
		}
	}

	return false;
}

TypedArray<ASAbilitySpec> ASAbilitySpec::get_sub_specs() const {
	TypedArray<ASAbilitySpec> arr;
	for (int i = 0; i < sub_specs.size(); i++) {
		arr.push_back(sub_specs[i]);
	}
	return arr;
}

TypedArray<ASEffectSpec> ASAbilitySpec::get_active_effects() const {
	TypedArray<ASEffectSpec> arr;
	for (int i = 0; i < active_effects.size(); i++) {
		arr.push_back(active_effects[i]);
	}
	return arr;
}

void ASAbilitySpec::activate(Object *p_target_node) {
	ASComponent *owner = get_owner();
	ERR_FAIL_NULL(owner);
	ERR_FAIL_COND(ability.is_null());

	// Initialize duration tracking
	if (ability->get_duration_policy() == ASAbility::POLICY_DURATION) {
		total_duration = ability->calculate_ability_duration(owner);
		duration_remaining = total_duration;
	}

	float level_f = (float)level;

	// 1. Apply Costs
	ability->apply_costs(owner, this);

	// 2. Start Cooldown
	StringName tag = ability->get_ability_tag();
	if (tag != StringName()) {
		float cd_duration = ability->get_cooldown_duration();
		if (ability->get_use_custom_cooldown()) {
			owner->GDVIRTUAL_CALL(_on_calculate_custom_magnitude, Variant(), -2, cd_duration);
		}
		if (cd_duration > 0.0f) {
			owner->start_cooldown(tag, cd_duration, ability->get_cooldown_tags());
		}
	}

	// 3. Apply Effects
	TypedArray<ASEffect> effects = ability->get_effects();

	for (int i = 0; i < effects.size(); i++) {
		Ref<ASEffect> effect = effects[i];
		if (effect.is_valid()) {
			if (effect->get_target_type() == ASEffect::TARGET_SELF) {
				Ref<ASEffectSpec> effect_spec = owner->make_outgoing_spec(effect, level_f, p_target_node);
				owner->apply_effect_spec_to_self(effect_spec);
				add_active_effect(effect_spec);
			}
		}
	}

	// 4. Add Owned Tags
	TypedArray<StringName> owned_tags = ability->get_activation_owned_tags();
	for (int i = 0; i < owned_tags.size(); i++) {
		owner->add_tag(owned_tags[i]);
	}

	// 5. Trigger Cues
	if (tag != StringName()) {
		owner->try_activate_cue_by_tag(tag, Dictionary(), p_target_node);
	}

	TypedArray<ASCue> cues = ability->get_cues();
	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid()) {
			Ref<ASCueSpec> cue_spec;
			cue_spec.instantiate();
			cue_spec->init_manual(cue, owner, Dictionary());
			cue_spec->set_target_node(p_target_node);
			owner->_execute_cue_with_spec(tag, cue_spec);
		}
	}

	ability->GDVIRTUAL_CALL(_on_activate_ability, owner, this);
}

void ASAbilitySpec::deactivate() {
	ASComponent *owner = get_owner();
	ERR_FAIL_NULL(owner);
	ERR_FAIL_COND(ability.is_null());

	// 1. Remove effects created by this ability instance
	for (int i = 0; i < active_effects.size(); i++) {
		owner->remove_active_effect(active_effects[i]);
	}
	clear_active_effects();

	// 2. Remove Owned Tags
	TypedArray<StringName> owned_tags = ability->get_activation_owned_tags();
	for (int i = 0; i < owned_tags.size(); i++) {
		owner->remove_tag(owned_tags[i]);
	}

	// 3. Trigger Cues (removal)
	StringName tag = ability->get_ability_tag();
	if (tag != StringName()) {
		owner->try_activate_cue_by_tag(tag);
	}

	TypedArray<ASCue> cues = ability->get_cues();
	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid()) {
			Ref<ASCueSpec> cue_spec;
			cue_spec.instantiate();
			cue_spec->init_manual(cue, owner, Dictionary());
			owner->_execute_cue_with_spec(tag, cue_spec);
		}
	}

	ability->GDVIRTUAL_CALL(_on_end_ability, owner, this);
}

float ASAbilitySpec::get_cooldown_duration() const {
	if (ability.is_valid()) {
		return ability->get_cooldown_duration();
	}
	return 0.0f;
}

float ASAbilitySpec::get_cooldown_remaining() const {
	ASComponent *owner = get_owner();
	if (owner && ability.is_valid()) {
		return owner->get_cooldown_remaining(ability->get_ability_tag());
	}
	return 0.0f;
}

bool ASAbilitySpec::is_on_cooldown() const {
	ASComponent *owner = get_owner();
	if (owner && ability.is_valid()) {
		return owner->is_on_cooldown(ability->get_ability_tag());
	}
	return false;
}

float ASAbilitySpec::get_cost_amount(const StringName &p_attribute) const {
	if (ability.is_valid()) {
		return ability->get_cost_amount(p_attribute);
	}
	return 0.0f;
}

TypedArray<ASEffect> ASAbilitySpec::get_effects_for_target(int p_target_type) const {
	TypedArray<ASEffect> result;
	if (ability.is_null()) {
		return result;
	}

	TypedArray<ASEffect> effects = ability->get_effects();
	for (int i = 0; i < effects.size(); i++) {
		Ref<ASEffect> effect = effects[i];
		if (effect.is_valid() && (int)effect->get_target_type() == p_target_type) {
			result.append(effect);
		}
	}
	return result;
}

void ASAbilitySpec::add_active_effect(Ref<ASEffectSpec> p_spec) {
	if (p_spec.is_valid() && active_effects.find(p_spec) == -1) {
		active_effects.push_back(p_spec);
	}
}

void ASAbilitySpec::remove_active_effect(Ref<ASEffectSpec> p_spec) {
	int idx = active_effects.find(p_spec);
	if (idx != -1) {
		active_effects.remove_at(idx);
	}
}

void ASAbilitySpec::clear_active_effects() {
	active_effects.clear();
}

void ASAbilitySpec::remove_sub_spec(Ref<ASAbilitySpec> p_spec) {
	int idx = sub_specs.find(p_spec);
	if (idx != -1) {
		sub_specs.remove_at(idx);
	}
}

ASAbilitySpec::ASAbilitySpec() {
}

ASAbilitySpec::~ASAbilitySpec() {
}
} // namespace godot
