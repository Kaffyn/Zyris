/**************************************************************************/
/*  as_effect.cpp                                                         */
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
#include "src/resources/as_effect.h"
#include "src/core/ability_system.h"
#include "src/core/as_tag_types.h"
#include "src/core/as_utils.h"
#include "src/resources/as_ability.h"
#include "src/resources/as_cue.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_tag_types.h"
#include "modules/ability_system/core/as_utils.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASEffect::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_effect_name", "name"), &ASEffect::set_effect_name);
	ClassDB::bind_method(D_METHOD("get_effect_name"), &ASEffect::get_effect_name);
	ClassDB::bind_method(D_METHOD("set_effect_tag", "tag"), &ASEffect::set_effect_tag);
	ClassDB::bind_method(D_METHOD("get_effect_tag"), &ASEffect::get_effect_tag);
	ClassDB::bind_method(D_METHOD("set_duration_policy", "policy"), &ASEffect::set_duration_policy);
	ClassDB::bind_method(D_METHOD("get_duration_policy"), &ASEffect::get_duration_policy);
	ClassDB::bind_method(D_METHOD("set_duration_magnitude", "magnitude"), &ASEffect::set_duration_magnitude);
	ClassDB::bind_method(D_METHOD("get_duration_magnitude"), &ASEffect::get_duration_magnitude);
	ClassDB::bind_method(D_METHOD("set_use_custom_duration", "use"), &ASEffect::set_use_custom_duration);
	ClassDB::bind_method(D_METHOD("get_use_custom_duration"), &ASEffect::get_use_custom_duration);
	ClassDB::bind_method(D_METHOD("set_stacking_policy", "policy"), &ASEffect::set_stacking_policy);
	ClassDB::bind_method(D_METHOD("get_stacking_policy"), &ASEffect::get_stacking_policy);
	ClassDB::bind_method(D_METHOD("set_target_type", "type"), &ASEffect::set_target_type);
	ClassDB::bind_method(D_METHOD("get_target_type"), &ASEffect::get_target_type);
	ClassDB::bind_method(D_METHOD("set_period", "period"), &ASEffect::set_period);
	ClassDB::bind_method(D_METHOD("get_period"), &ASEffect::get_period);
	ClassDB::bind_method(D_METHOD("set_execute_periodic_tick_on_application", "exec"), &ASEffect::set_execute_periodic_tick_on_application);
	ClassDB::bind_method(D_METHOD("get_execute_periodic_tick_on_application"), &ASEffect::get_execute_periodic_tick_on_application);

	ClassDB::bind_method(D_METHOD("add_modifier", "attribute", "operation", "magnitude", "use_custom_magnitude"), &ASEffect::add_modifier, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_modifier_count"), &ASEffect::get_modifier_count);
	ClassDB::bind_method(D_METHOD("get_modifier_attribute", "index"), &ASEffect::get_modifier_attribute);
	ClassDB::bind_method(D_METHOD("get_modifier_operation", "index"), &ASEffect::get_modifier_operation);
	ClassDB::bind_method(D_METHOD("get_modifier_magnitude", "index"), &ASEffect::get_modifier_magnitude);
	ClassDB::bind_method(D_METHOD("is_modifier_custom", "index"), &ASEffect::is_modifier_custom);

	ClassDB::bind_method(D_METHOD("set_modifiers_count", "count"), &ASEffect::set_modifiers_count);
	ClassDB::bind_method(D_METHOD("get_modifiers_count"), &ASEffect::get_modifiers_count);

	ClassDB::bind_method(D_METHOD("set_requirements_count", "count"), &ASEffect::set_requirements_count);
	ClassDB::bind_method(D_METHOD("get_requirements_count"), &ASEffect::get_requirements_count);
	ClassDB::bind_method(D_METHOD("add_requirement", "attribute", "amount"), &ASEffect::add_requirement);
	ClassDB::bind_method(D_METHOD("get_requirement_attribute", "index"), &ASEffect::get_requirement_attribute);
	ClassDB::bind_method(D_METHOD("get_requirement_amount", "index"), &ASEffect::get_requirement_amount);

	ClassDB::bind_method(D_METHOD("set_activation_required_all_tags", "tags"), &ASEffect::set_activation_required_all_tags);
	ClassDB::bind_method(D_METHOD("get_activation_required_all_tags"), &ASEffect::get_activation_required_all_tags);
	ClassDB::bind_method(D_METHOD("set_activation_required_any_tags", "tags"), &ASEffect::set_activation_required_any_tags);
	ClassDB::bind_method(D_METHOD("get_activation_required_any_tags"), &ASEffect::get_activation_required_any_tags);
	ClassDB::bind_method(D_METHOD("set_activation_blocked_any_tags", "tags"), &ASEffect::set_activation_blocked_any_tags);
	ClassDB::bind_method(D_METHOD("get_activation_blocked_any_tags"), &ASEffect::get_activation_blocked_any_tags);
	ClassDB::bind_method(D_METHOD("set_activation_blocked_all_tags", "tags"), &ASEffect::set_activation_blocked_all_tags);
	ClassDB::bind_method(D_METHOD("get_activation_blocked_all_tags"), &ASEffect::get_activation_blocked_all_tags);

	ClassDB::bind_method(D_METHOD("set_granted_tags", "tags"), &ASEffect::set_granted_tags);
	ClassDB::bind_method(D_METHOD("get_granted_tags"), &ASEffect::get_granted_tags);
	ClassDB::bind_method(D_METHOD("set_blocked_tags", "tags"), &ASEffect::set_blocked_tags);
	ClassDB::bind_method(D_METHOD("get_blocked_tags"), &ASEffect::get_blocked_tags);
	ClassDB::bind_method(D_METHOD("set_removed_tags", "tags"), &ASEffect::set_removed_tags);
	ClassDB::bind_method(D_METHOD("get_removed_tags"), &ASEffect::get_removed_tags);
	ClassDB::bind_method(D_METHOD("set_events_on_apply", "events"), &ASEffect::set_events_on_apply);
	ClassDB::bind_method(D_METHOD("get_events_on_apply"), &ASEffect::get_events_on_apply);
	ClassDB::bind_method(D_METHOD("set_events_on_remove", "events"), &ASEffect::set_events_on_remove);
	ClassDB::bind_method(D_METHOD("get_events_on_remove"), &ASEffect::get_events_on_remove);
	ClassDB::bind_method(D_METHOD("set_cues", "cues"), &ASEffect::set_cues);
	ClassDB::bind_method(D_METHOD("get_cues"), &ASEffect::get_cues);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "effect_name"), "set_effect_name", "get_effect_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "effect_tag"), "set_effect_tag", "get_effect_tag");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "duration_policy", PROPERTY_HINT_ENUM, "Instant,Duration,Infinite"), "set_duration_policy", "get_duration_policy");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration_magnitude"), "set_duration_magnitude", "get_duration_magnitude");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_custom_duration"), "set_use_custom_duration", "get_use_custom_duration");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "stacking_policy", PROPERTY_HINT_ENUM, "New Instance,Override,Stack Intensity,Stack Duration"), "set_stacking_policy", "get_stacking_policy");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "target_type", PROPERTY_HINT_ENUM, "Self,Others"), "set_target_type", "get_target_type");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "period"), "set_period", "get_period");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "execute_periodic_tick_on_application"), "set_execute_periodic_tick_on_application", "get_execute_periodic_tick_on_application");

	ADD_GROUP("Activation", "activation_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_required_all_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_required_all_tags", "get_activation_required_all_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_required_any_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_required_any_tags", "get_activation_required_any_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_blocked_any_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_blocked_any_tags", "get_activation_blocked_any_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_blocked_all_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_blocked_all_tags", "get_activation_blocked_all_tags");

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "granted_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_granted_tags", "get_granted_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "blocked_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_blocked_tags", "get_blocked_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "removed_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_removed_tags", "get_removed_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events_on_apply", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_events_on_apply", "get_events_on_apply");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events_on_remove", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_events_on_remove", "get_events_on_remove");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues", PROPERTY_HINT_RESOURCE_TYPE, "ASCue"), "set_cues", "get_cues");

	ADD_GROUP("Requirements", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "modifiers", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_ARRAY, "Modifiers,modifiers/"), "set_modifiers_count", "get_modifiers_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "requirements", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_ARRAY, "Requirements,requirements/"), "set_requirements_count", "get_requirements_count");

	BIND_ENUM_CONSTANT(POLICY_INSTANT);
	BIND_ENUM_CONSTANT(POLICY_DURATION);
	BIND_ENUM_CONSTANT(POLICY_INFINITE);

	BIND_ENUM_CONSTANT(STACK_NEW_INSTANCE);
	BIND_ENUM_CONSTANT(STACK_OVERRIDE);
	BIND_ENUM_CONSTANT(STACK_INTENSITY);
	BIND_ENUM_CONSTANT(STACK_DURATION);

	BIND_ENUM_CONSTANT(TARGET_SELF);
	BIND_ENUM_CONSTANT(TARGET_OTHERS);

	BIND_ENUM_CONSTANT(OP_ADD);
	BIND_ENUM_CONSTANT(OP_MULTIPLY);
	BIND_ENUM_CONSTANT(OP_DIVIDE);
	BIND_ENUM_CONSTANT(OP_OVERRIDE);
}

void ASEffect::add_modifier(const StringName &p_attr, ModifierOp p_op, float p_mag, bool p_custom) {
	ASEffectModifierData md;
	md.attribute = p_attr;
	md.operation = (::ModifierOp)p_op;
	md.magnitude = p_mag;
	md.use_custom_magnitude = p_custom;
	modifiers.push_back(md);
}

bool ASEffect::is_modifier_custom(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, modifiers.size(), false);
	return modifiers[p_idx].use_custom_magnitude;
}

StringName ASEffect::get_modifier_attribute(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, modifiers.size(), StringName());
	return modifiers[p_idx].attribute;
}

ASEffect::ModifierOp ASEffect::get_modifier_operation(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, modifiers.size(), OP_ADD);
	return (ModifierOp)modifiers[p_idx].operation;
}

float ASEffect::get_modifier_magnitude(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, modifiers.size(), 0.0f);
	return modifiers[p_idx].magnitude;
}

void ASEffect::set_modifiers_count(int p_count) {
	modifiers.resize(p_count);
}
int ASEffect::get_modifiers_count() const {
	return modifiers.size();
}

void ASEffect::set_requirements_count(int p_count) {
	requirements.resize(p_count);
}
int ASEffect::get_requirements_count() const {
	return requirements.size();
}

void ASEffect::add_requirement(const StringName &p_attr, float p_amount) {
	ASEffectRequirement rd;
	rd.attribute = p_attr;
	rd.amount = p_amount;
	requirements.push_back(rd);
}

StringName ASEffect::get_requirement_attribute(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, requirements.size(), StringName());
	return requirements[p_idx].attribute;
}

float ASEffect::get_requirement_amount(int p_idx) const {
	ERR_FAIL_INDEX_V(p_idx, requirements.size(), 0.0f);
	return requirements[p_idx].amount;
}

bool ASEffect::_set(const StringName &p_name, const Variant &p_value) {
	String prop_name = p_name;
	if (prop_name.begins_with("modifiers/")) {
		int index = prop_name.get_slicec('/', 1).to_int();
		String what = prop_name.get_slicec('/', 2);

		if (modifiers.size() <= index) {
			modifiers.resize(index + 1);
		}

		if (what == "attribute") {
			modifiers.write[index].attribute = p_value;
			return true;
		} else if (what == "operation") {
			modifiers.write[index].operation = (::ModifierOp)(int)p_value;
			return true;
		} else if (what == "magnitude") {
			modifiers.write[index].magnitude = p_value;
			return true;
		} else if (what == "use_custom_magnitude") {
			modifiers.write[index].use_custom_magnitude = p_value;
			return true;
		}
	}

	if (prop_name.begins_with("requirements/")) {
		int index = prop_name.get_slicec('/', 1).to_int();
		String what = prop_name.get_slicec('/', 2);

		if (requirements.size() <= index) {
			requirements.resize(index + 1);
		}

		if (what == "attribute") {
			requirements.write[index].attribute = p_value;
			return true;
		} else if (what == "amount") {
			requirements.write[index].amount = p_value;
			return true;
		}
	}
	return false;
}

bool ASEffect::_get(const StringName &p_name, Variant &r_ret) const {
	String prop_name = p_name;
	if (prop_name.begins_with("modifiers/")) {
		int index = prop_name.get_slicec('/', 1).to_int();
		String what = prop_name.get_slicec('/', 2);

		if (index < 0 || index >= modifiers.size()) {
			return false;
		}

		if (what == "attribute") {
			r_ret = modifiers[index].attribute;
			return true;
		} else if (what == "operation") {
			r_ret = modifiers[index].operation;
			return true;
		} else if (what == "magnitude") {
			r_ret = modifiers[index].magnitude;
			return true;
		} else if (what == "use_custom_magnitude") {
			r_ret = modifiers[index].use_custom_magnitude;
			return true;
		}
	}

	if (prop_name.begins_with("requirements/")) {
		int index = prop_name.get_slicec('/', 1).to_int();
		String what = prop_name.get_slicec('/', 2);

		if (index < 0 || index >= requirements.size()) {
			return false;
		}

		if (what == "attribute") {
			r_ret = requirements[index].attribute;
			return true;
		} else if (what == "amount") {
			r_ret = requirements[index].amount;
			return true;
		}
	}
	return false;
}

void ASEffect::_get_property_list(List<PropertyInfo> *p_list) const {
	for (int i = 0; i < modifiers.size(); i++) {
		p_list->push_back(PropertyInfo(Variant::STRING_NAME, vformat("modifiers/%d/attribute", i)));
		p_list->push_back(PropertyInfo(Variant::INT, vformat("modifiers/%d/operation", i), PROPERTY_HINT_ENUM, "Add,Multiply,Divide,Override"));
		p_list->push_back(PropertyInfo(Variant::FLOAT, vformat("modifiers/%d/magnitude", i)));
		p_list->push_back(PropertyInfo(Variant::BOOL, vformat("modifiers/%d/use_custom_magnitude", i)));
	}

	for (int i = 0; i < requirements.size(); i++) {
		p_list->push_back(PropertyInfo(Variant::STRING_NAME, vformat("requirements/%d/attribute", i)));
		p_list->push_back(PropertyInfo(Variant::FLOAT, vformat("requirements/%d/amount", i)));
	}
}

void ASEffect::set_effect_name(const String &p_name) {
	if (effect_name == p_name) {
		return;
	}

	AbilitySystem *as = AbilitySystem::get_singleton();
	if (as) {
		if (!effect_name.is_empty()) {
			as->unregister_resource_name(effect_name);
		}
		as->register_resource_name(p_name, get_instance_id());
	}
	effect_name = p_name;
}

void ASEffect::set_activation_required_all_tags(const TypedArray<StringName> &p_tags) {
	activation_required_all_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

TypedArray<StringName> ASEffect::get_activation_required_all_tags() const {
	return activation_required_all_tags;
}

void ASEffect::set_activation_required_any_tags(const TypedArray<StringName> &p_tags) {
	activation_required_any_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASEffect::set_activation_blocked_any_tags(const TypedArray<StringName> &p_tags) {
	activation_blocked_any_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASEffect::set_activation_blocked_all_tags(const TypedArray<StringName> &p_tags) {
	activation_blocked_all_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

void ASEffect::set_effect_tag(const StringName &p_tag) {
	effect_tag = p_tag;
	if (AbilitySystem::get_singleton()) {
		AbilitySystem::get_singleton()->register_tag(p_tag, (ASTagType)AbilitySystem::TAG_TYPE_NAME, get_instance_id());
	}

	// Propagate to cues
	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid()) {
			cue->set_cue_tag(p_tag);
		}
	}
}

void ASEffect::set_granted_tags(const TypedArray<StringName> &p_tags) {
	granted_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

TypedArray<StringName> ASEffect::get_granted_tags() const {
	return granted_tags;
}

void ASEffect::set_blocked_tags(const TypedArray<StringName> &p_tags) {
	blocked_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

TypedArray<StringName> ASEffect::get_blocked_tags() const {
	return blocked_tags;
}

void ASEffect::set_removed_tags(const TypedArray<StringName> &p_tags) {
	removed_tags = p_tags;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_tags.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_tags[i], (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
		}
	}
}

TypedArray<StringName> ASEffect::get_removed_tags() const {
	return removed_tags;
}

void ASEffect::set_events_on_apply(const TypedArray<StringName> &p_events) {
	events_on_apply = p_events;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_events.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_events[i], (ASTagType)AbilitySystem::TAG_TYPE_EVENT);
		}
	}
}

TypedArray<StringName> ASEffect::get_events_on_apply() const {
	return events_on_apply;
}

void ASEffect::set_events_on_remove(const TypedArray<StringName> &p_events) {
	events_on_remove = p_events;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_events.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_events[i], (ASTagType)AbilitySystem::TAG_TYPE_EVENT);
		}
	}
}

TypedArray<StringName> ASEffect::get_events_on_remove() const {
	return events_on_remove;
}

void ASEffect::set_cues(const TypedArray<ASCue> &p_cues) {
	cues = p_cues;
	if (effect_tag != StringName()) {
		for (int i = 0; i < cues.size(); i++) {
			Ref<ASCue> cue = cues[i];
			if (cue.is_valid()) {
				cue->set_cue_tag(effect_tag);
			}
		}
	}
}

TypedArray<ASCue> ASEffect::get_cues() const {
	return cues;
}

ASEffect::ASEffect() {
}

ASEffect::~ASEffect() {
}
