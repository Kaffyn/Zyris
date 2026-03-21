/**************************************************************************/
/*  as_ability.cpp                                                        */
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
#include "src/resources/as_ability.h"
#include "src/core/ability_system.h"
#include "src/core/as_ability_spec.h"
#include "src/core/as_cue_spec.h"
#include "src/core/as_effect_spec.h"
#include "src/core/as_tag_spec.h"
#include "src/resources/as_cue.h"
#include "src/resources/as_effect.h"
#include "src/scene/as_component.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_ability_spec.h"
#include "modules/ability_system/core/as_cue_spec.h"
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/core/as_tag_spec.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/scene/as_component.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASAbility::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ability_name", "name"), &ASAbility::set_ability_name);
	ClassDB::bind_method(D_METHOD("get_ability_name"), &ASAbility::get_ability_name);
	ClassDB::bind_method(D_METHOD("set_ability_tag", "tag"), &ASAbility::set_ability_tag);
	ClassDB::bind_method(D_METHOD("get_ability_tag"), &ASAbility::get_ability_tag);

	ClassDB::bind_method(D_METHOD("set_activation_owned_tags", "tags"), &ASAbility::set_activation_owned_tags);
	ClassDB::bind_method(D_METHOD("get_activation_owned_tags"), &ASAbility::get_activation_owned_tags);

	ClassDB::bind_method(D_METHOD("set_activation_required_all_tags", "tags"), &ASAbility::set_activation_required_all_tags);
	ClassDB::bind_method(D_METHOD("get_activation_required_all_tags"), &ASAbility::get_activation_required_all_tags);

	ClassDB::bind_method(D_METHOD("set_activation_required_any_tags", "tags"), &ASAbility::set_activation_required_any_tags);
	ClassDB::bind_method(D_METHOD("get_activation_required_any_tags"), &ASAbility::get_activation_required_any_tags);

	ClassDB::bind_method(D_METHOD("set_activation_blocked_any_tags", "tags"), &ASAbility::set_activation_blocked_any_tags);
	ClassDB::bind_method(D_METHOD("get_activation_blocked_any_tags"), &ASAbility::get_activation_blocked_any_tags);

	ClassDB::bind_method(D_METHOD("set_activation_blocked_all_tags", "tags"), &ASAbility::set_activation_blocked_all_tags);
	ClassDB::bind_method(D_METHOD("get_activation_blocked_all_tags"), &ASAbility::get_activation_blocked_all_tags);

	ClassDB::bind_method(D_METHOD("set_activation_cancel_tags", "tags"), &ASAbility::set_activation_cancel_tags);
	ClassDB::bind_method(D_METHOD("get_activation_cancel_tags"), &ASAbility::get_activation_cancel_tags);

	ClassDB::bind_method(D_METHOD("set_cues", "cues"), &ASAbility::set_cues);
	ClassDB::bind_method(D_METHOD("get_cues"), &ASAbility::get_cues);

	ClassDB::bind_method(D_METHOD("set_effects", "effects"), &ASAbility::set_effects);
	ClassDB::bind_method(D_METHOD("get_effects"), &ASAbility::get_effects);

	// Duration methods
	ClassDB::bind_method(D_METHOD("set_duration_policy", "policy"), &ASAbility::set_duration_policy);
	ClassDB::bind_method(D_METHOD("get_duration_policy"), &ASAbility::get_duration_policy);
	ClassDB::bind_method(D_METHOD("set_ability_duration", "duration"), &ASAbility::set_ability_duration);
	ClassDB::bind_method(D_METHOD("get_ability_duration"), &ASAbility::get_ability_duration);
	ClassDB::bind_method(D_METHOD("set_use_custom_duration", "use"), &ASAbility::set_use_custom_duration);
	ClassDB::bind_method(D_METHOD("get_use_custom_duration"), &ASAbility::get_use_custom_duration);
	ClassDB::bind_method(D_METHOD("calculate_ability_duration", "owner"), &ASAbility::calculate_ability_duration);

	// Cooldown methods
	ClassDB::bind_method(D_METHOD("set_cooldown_duration", "duration"), &ASAbility::set_cooldown_duration);
	ClassDB::bind_method(D_METHOD("get_cooldown_duration"), &ASAbility::get_cooldown_duration);
	ClassDB::bind_method(D_METHOD("set_cooldown_tags", "tags"), &ASAbility::set_cooldown_tags);
	ClassDB::bind_method(D_METHOD("get_cooldown_tags"), &ASAbility::get_cooldown_tags);
	ClassDB::bind_method(D_METHOD("set_use_custom_cooldown", "use"), &ASAbility::set_use_custom_cooldown);
	ClassDB::bind_method(D_METHOD("get_use_custom_cooldown"), &ASAbility::get_use_custom_cooldown);

	// Cost methods
	ClassDB::bind_method(D_METHOD("set_costs", "costs"), &ASAbility::set_costs);
	ClassDB::bind_method(D_METHOD("get_costs"), &ASAbility::get_costs);
	ClassDB::bind_method(D_METHOD("set_use_custom_costs", "use"), &ASAbility::set_use_custom_costs);
	ClassDB::bind_method(D_METHOD("get_use_custom_costs"), &ASAbility::get_use_custom_costs);
	ClassDB::bind_method(D_METHOD("set_triggers", "triggers"), &ASAbility::set_triggers);
	ClassDB::bind_method(D_METHOD("get_triggers"), &ASAbility::get_triggers);
	ClassDB::bind_method(D_METHOD("add_trigger", "tag", "type"), &ASAbility::add_trigger);
	ClassDB::bind_method(D_METHOD("add_cost", "attribute", "amount"), &ASAbility::add_cost);
	ClassDB::bind_method(D_METHOD("remove_cost", "attribute"), &ASAbility::remove_cost);
	ClassDB::bind_method(D_METHOD("get_cost_amount", "attribute"), &ASAbility::get_cost_amount);
	ClassDB::bind_method(D_METHOD("can_afford_costs", "owner", "spec"), &ASAbility::can_afford_costs, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("apply_costs", "owner", "spec"), &ASAbility::apply_costs, DEFVAL(Variant()));

	// Requirement methods
	ClassDB::bind_method(D_METHOD("set_requirements", "requirements"), &ASAbility::set_requirements);
	ClassDB::bind_method(D_METHOD("get_requirements"), &ASAbility::get_requirements);
	ClassDB::bind_method(D_METHOD("get_requirement_count"), &ASAbility::get_requirement_count);
	ClassDB::bind_method(D_METHOD("add_requirement", "attribute", "amount"), &ASAbility::add_requirement);
	ClassDB::bind_method(D_METHOD("remove_requirement", "attribute"), &ASAbility::remove_requirement);
	ClassDB::bind_method(D_METHOD("get_requirement_amount", "attribute"), &ASAbility::get_requirement_amount);
	ClassDB::bind_method(D_METHOD("can_satisfy_requirements", "owner", "spec"), &ASAbility::can_satisfy_requirements, DEFVAL(Variant()));

	ClassDB::bind_method(D_METHOD("can_activate_ability", "owner", "spec"), &ASAbility::can_activate_ability, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("activate_ability", "owner", "spec", "target_node"), &ASAbility::activate_ability, DEFVAL(Variant()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("end_ability", "owner", "spec"), &ASAbility::end_ability, DEFVAL(Variant()));

	GDVIRTUAL_BIND(_on_activate_ability, "owner", "spec");
	GDVIRTUAL_BIND(_on_can_activate_ability, "owner", "spec");
	GDVIRTUAL_BIND(_on_end_ability, "owner", "spec");

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "ability_name"), "set_ability_name", "get_ability_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "ability_tag"), "set_ability_tag", "get_ability_tag");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_owned_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_owned_tags", "get_activation_owned_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_required_all_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_required_all_tags", "get_activation_required_all_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_required_any_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_required_any_tags", "get_activation_required_any_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_blocked_any_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_blocked_any_tags", "get_activation_blocked_any_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_blocked_all_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_blocked_all_tags", "get_activation_blocked_all_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_cancel_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_cancel_tags", "get_activation_cancel_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues", PROPERTY_HINT_ARRAY_TYPE, "ASCue"), "set_cues", "get_cues");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "effects", PROPERTY_HINT_ARRAY_TYPE, "ASEffect"), "set_effects", "get_effects");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "ability_duration_policy", PROPERTY_HINT_ENUM, "Instant,Duration,Infinite"), "set_duration_policy", "get_duration_policy");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ability_duration"), "set_ability_duration", "get_ability_duration");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "ability_use_custom_duration", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_use_custom_duration", "get_use_custom_duration");

	BIND_ENUM_CONSTANT(POLICY_INSTANT);
	BIND_ENUM_CONSTANT(POLICY_DURATION);
	BIND_ENUM_CONSTANT(POLICY_INFINITE);

	BIND_ENUM_CONSTANT(TRIGGER_ON_TAG_ADDED);
	BIND_ENUM_CONSTANT(TRIGGER_ON_TAG_REMOVED);

	ADD_GROUP("Cooldown", "cooldown_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cooldown_duration"), "set_cooldown_duration", "get_cooldown_duration");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cooldown_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_cooldown_tags", "get_cooldown_tags");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "cooldown_use_custom", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_use_custom_cooldown", "get_use_custom_cooldown");

	ADD_GROUP("Costs", "costs_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "costs", PROPERTY_HINT_ARRAY_TYPE, "Dictionary"), "set_costs", "get_costs");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "costs_use_custom", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT), "set_use_custom_costs", "get_use_custom_costs");

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "triggers", PROPERTY_HINT_ARRAY_TYPE, "Dictionary"), "set_triggers", "get_triggers");

	ADD_GROUP("Requirements", "requirements_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "requirements", PROPERTY_HINT_ARRAY_TYPE, "Dictionary"), "set_requirements", "get_requirements");
}

bool ASAbility::can_activate_ability(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec) const {
	ERR_FAIL_NULL_V(p_owner, false);

	if (!p_owner->get_owned_tags()->has_all_tags(activation_required_all_tags)) {
		return false;
	}

	if (!activation_required_any_tags.is_empty() && !p_owner->get_owned_tags()->has_any_tags(activation_required_any_tags)) {
		return false;
	}

	if (p_owner->get_owned_tags()->has_any_tags(activation_blocked_any_tags)) {
		return false;
	}

	if (!activation_blocked_all_tags.is_empty() && p_owner->get_owned_tags()->has_all_tags(activation_blocked_all_tags)) {
		return false;
	}

	if (ability_tag != StringName() && p_owner->is_on_cooldown(ability_tag)) {
		return false;
	}

	if (!can_satisfy_requirements(p_owner, p_spec)) {
		return false;
	}

	if (!can_afford_costs(p_owner, p_spec)) {
		return false;
	}

	bool script_ret = true;
	if (GDVIRTUAL_CALL(_on_can_activate_ability, p_owner, p_spec, script_ret)) {
		if (!script_ret) {
			return false;
		}
	}

	return true;
}

void ASAbility::activate_ability(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec, Object *p_target_node) {
	ERR_FAIL_COND(p_spec.is_null());
	p_spec->activate(p_target_node);
}

void ASAbility::end_ability(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec) {
	ERR_FAIL_COND(p_spec.is_null());
	p_spec->deactivate();
}

void ASAbility::set_effects(const TypedArray<ASEffect> &p_effects) {
	effects = p_effects;
}

TypedArray<ASEffect> ASAbility::get_effects() const {
	return effects;
}

void ASAbility::set_duration_policy(DurationPolicy p_policy) {
	duration_policy = p_policy;
}

ASAbility::DurationPolicy ASAbility::get_duration_policy() const {
	return duration_policy;
}

void ASAbility::set_ability_duration(float p_duration) {
	ability_duration = p_duration;
}

float ASAbility::get_ability_duration() const {
	return ability_duration;
}

void ASAbility::set_use_custom_duration(bool p_use) {
	use_custom_duration = p_use;
}

bool ASAbility::get_use_custom_duration() const {
	return use_custom_duration;
}

float ASAbility::calculate_ability_duration(ASComponent *p_owner) const {
	float duration = ability_duration;
	if (use_custom_duration && p_owner) {
		p_owner->GDVIRTUAL_CALL(_on_calculate_custom_magnitude, Variant(), -1, duration);
	}
	return duration;
}

String ASAbility::get_ability_name() const {
	return ability_name;
}
StringName ASAbility::get_ability_tag() const {
	return ability_tag;
}
TypedArray<StringName> ASAbility::get_activation_owned_tags() const {
	return activation_owned_tags;
}
TypedArray<StringName> ASAbility::get_activation_required_all_tags() const {
	return activation_required_all_tags;
}
TypedArray<StringName> ASAbility::get_activation_required_any_tags() const {
	return activation_required_any_tags;
}
TypedArray<StringName> ASAbility::get_activation_blocked_any_tags() const {
	return activation_blocked_any_tags;
}
TypedArray<StringName> ASAbility::get_activation_blocked_all_tags() const {
	return activation_blocked_all_tags;
}
void ASAbility::set_activation_cancel_tags(const TypedArray<StringName> &p_tags) {
	activation_cancel_tags = p_tags;
}
TypedArray<StringName> ASAbility::get_activation_cancel_tags() const {
	return activation_cancel_tags;
}
TypedArray<ASCue> ASAbility::get_cues() const {
	return cues;
}

void ASAbility::set_cooldown_duration(float p_duration) {
	cooldown_duration = p_duration;
}
float ASAbility::get_cooldown_duration() const {
	return cooldown_duration;
}
void ASAbility::set_cooldown_tags(const TypedArray<StringName> &p_tags) {
	cooldown_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}
TypedArray<StringName> ASAbility::get_cooldown_tags() const {
	return cooldown_tags;
}
bool ASAbility::get_use_custom_cooldown() const {
	return use_custom_cooldown;
}
void ASAbility::set_use_custom_cooldown(bool p_use) {
	use_custom_cooldown = p_use;
}

TypedArray<Dictionary> ASAbility::get_costs() const {
	return costs;
}
void ASAbility::set_costs(const TypedArray<Dictionary> &p_costs) {
	costs = p_costs;
}
bool ASAbility::get_use_custom_costs() const {
	return use_custom_costs;
}
void ASAbility::set_use_custom_costs(bool p_use) {
	use_custom_costs = p_use;
}

void ASAbility::set_requirements(const TypedArray<Dictionary> &p_requirements) {
	requirements = p_requirements;
}

TypedArray<Dictionary> ASAbility::get_requirements() const {
	return requirements;
}

void ASAbility::add_requirement(const StringName &p_attribute, float p_amount) {
	Dictionary req;
	req["attribute"] = p_attribute;
	req["amount"] = p_amount;
	requirements.append(req);
}

bool ASAbility::remove_requirement(const StringName &p_attribute) {
	for (int i = 0; i < requirements.size(); i++) {
		Dictionary req = requirements[i];
		StringName attr = req["attribute"];
		if (req.has("attribute") && attr == p_attribute) {
			requirements.remove_at(i);
			return true;
		}
	}
	return false;
}

float ASAbility::get_requirement_amount(const StringName &p_attribute) const {
	for (int i = 0; i < requirements.size(); i++) {
		Dictionary req = requirements[i];
		StringName attr = req["attribute"];
		if (req.has("attribute") && attr == p_attribute) {
			return req["amount"];
		}
	}
	return 0.0f;
}

bool ASAbility::can_satisfy_requirements(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec) const {
	for (int i = 0; i < requirements.size(); i++) {
		Dictionary req = requirements[i];
		if (req.has("attribute") && req.has("amount")) {
			StringName attribute = req["attribute"];
			float amount = req["amount"];

			float current_value = p_owner->get_attribute_value_by_tag(attribute);
			if (current_value < amount) {
				return false;
			}
		}
	}
	return true;
}

void ASAbility::set_activation_owned_tags(const TypedArray<StringName> &p_tags) {
	activation_owned_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASAbility::set_activation_required_all_tags(const TypedArray<StringName> &p_tags) {
	activation_required_all_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASAbility::set_activation_required_any_tags(const TypedArray<StringName> &p_tags) {
	activation_required_any_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASAbility::set_activation_blocked_any_tags(const TypedArray<StringName> &p_tags) {
	activation_blocked_any_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASAbility::set_activation_blocked_all_tags(const TypedArray<StringName> &p_tags) {
	activation_blocked_all_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASAbility::set_cues(const TypedArray<ASCue> &p_cues) {
	cues = p_cues;
	if (ability_tag != StringName()) {
		for (int i = 0; i < cues.size(); i++) {
			Ref<ASCue> cue = cues[i];
			if (cue.is_valid()) {
				cue->set_cue_tag(ability_tag);
			}
		}
	}
}

void ASAbility::set_ability_name(const String &p_name) {
	if (ability_name == p_name) {
		return;
	}

	AbilitySystem *as = AbilitySystem::get_singleton();
	if (as) {
		if (!ability_name.is_empty()) {
			as->unregister_resource_name(ability_name);
		}
		as->register_resource_name(p_name, get_instance_id());
	}
	ability_name = p_name;
}

void ASAbility::set_ability_tag(const StringName &p_tag) {
	ability_tag = p_tag;
	if (AbilitySystem::get_singleton()) {
		AbilitySystem::get_singleton()->register_tag(p_tag, AbilitySystem::TAG_TYPE_NAME, get_instance_id());
	}

	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid()) {
			cue->set_cue_tag(p_tag);
		}
	}
}

void ASAbility::add_cost(const StringName &p_attribute, float p_amount) {
	Dictionary cost;
	cost["attribute"] = p_attribute;
	cost["amount"] = p_amount;
	costs.append(cost);
}

bool ASAbility::remove_cost(const StringName &p_attribute) {
	for (int i = 0; i < costs.size(); i++) {
		Dictionary cost = costs[i];
		StringName attr = cost["attribute"];
		if (cost.has("attribute") && attr == p_attribute) {
			costs.remove_at(i);
			return true;
		}
	}
	return false;
}

float ASAbility::get_cost_amount(const StringName &p_attribute) const {
	for (int i = 0; i < costs.size(); i++) {
		Dictionary cost = costs[i];
		StringName attr = cost["attribute"];
		if (cost.has("attribute") && attr == p_attribute) {
			return cost["amount"];
		}
	}
	return 0.0f;
}

bool ASAbility::can_afford_costs(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec) const {
	for (int i = 0; i < costs.size(); i++) {
		Dictionary cost = costs[i];
		if (cost.has("attribute") && cost.has("amount")) {
			StringName attribute = cost["attribute"];
			float amount = cost["amount"];

			if (use_custom_costs) {
				p_owner->GDVIRTUAL_CALL(_on_calculate_custom_magnitude, Variant(), -3 - i, amount);
			}

			float current_value = p_owner->get_attribute_value_by_tag(attribute);
			// Cost is treated as a positive value to subtract
			if (current_value < amount) {
				return false;
			}
		}
	}
	return true;
}

void ASAbility::apply_costs(ASComponent *p_owner, Ref<ASAbilitySpec> p_spec) const {
	for (int i = 0; i < costs.size(); i++) {
		Dictionary cost = costs[i];
		if (cost.has("attribute") && cost.has("amount")) {
			StringName attribute = cost["attribute"];
			float amount = cost["amount"];

			if (use_custom_costs) {
				p_owner->GDVIRTUAL_CALL(_on_calculate_custom_magnitude, Variant(), -3 - i, amount);
			}

			float base_value = p_owner->get_attribute_base_value_by_tag(attribute);
			// Subtract the cost magnitude from the base value
			p_owner->set_attribute_base_value_by_tag(attribute, base_value - amount);
		}
	}
}

void ASAbility::set_triggers(const TypedArray<Dictionary> &p_triggers) {
	triggers = p_triggers;
}

TypedArray<Dictionary> ASAbility::get_triggers() const {
	return triggers;
}

void ASAbility::add_trigger(const StringName &p_tag, TriggerType p_type) {
	Dictionary d;
	d["tag"] = p_tag;
	d["type"] = (int)p_type;
	triggers.append(d);
}

ASAbility::ASAbility() {
}

ASAbility::~ASAbility() {
}
