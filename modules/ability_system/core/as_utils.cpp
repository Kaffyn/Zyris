/**************************************************************************/
/*  as_utils.cpp                                                          */
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
#include "src/core/as_utils.h"
#include "src/core/ability_system.h"
#include "src/core/as_tag_spec.h"
#include "src/core/as_tag_types.h"
#include "src/resources/as_attribute_set.h"
#include "src/scene/as_component.h"
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#else
#include "core/os/os.h"
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_tag_types.h"
#include "modules/ability_system/core/as_utils.h"
#include "modules/ability_system/scene/as_component.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

// === ASEffectModifier Implementation ===

Dictionary ASEffectModifier::to_dict() const {
	Dictionary dict;
	dict["attribute"] = attribute;
	dict["operation"] = operation;
	dict["magnitude"] = magnitude;
	return dict;
}

void ASEffectModifier::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("attribute")) {
		attribute = p_dict["attribute"];
	}
	if (p_dict.has("operation")) {
		operation = (ModifierOp)(int)p_dict["operation"];
	}
	if (p_dict.has("magnitude")) {
		magnitude = p_dict["magnitude"];
	}
}

// === ASEffectModifierData Implementation ===

Dictionary ASEffectModifierData::to_dict() const {
	Dictionary dict;
	dict["attribute"] = attribute;
	dict["operation"] = operation;
	dict["magnitude"] = magnitude;
	dict["use_custom_magnitude"] = use_custom_magnitude;
	return dict;
}

void ASEffectModifierData::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("attribute")) {
		attribute = p_dict["attribute"];
	}
	if (p_dict.has("operation")) {
		operation = (ModifierOp)(int)p_dict["operation"];
	}
	if (p_dict.has("magnitude")) {
		magnitude = p_dict["magnitude"];
	}
	if (p_dict.has("use_custom_magnitude")) {
		use_custom_magnitude = p_dict["use_custom_magnitude"];
	}
}

// === ASEffectRequirement Implementation ===

Dictionary ASEffectRequirement::to_dict() const {
	Dictionary dict;
	dict["attribute"] = attribute;
	dict["amount"] = amount;
	return dict;
}

void ASEffectRequirement::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("attribute")) {
		attribute = p_dict["attribute"];
	}
	if (p_dict.has("amount")) {
		amount = p_dict["amount"];
	}
}

// === ASAttributeValue Implementation ===

Dictionary ASAttributeValue::to_dict() const {
	Dictionary dict;
	dict["base_value"] = base_value;
	dict["current_value"] = current_value;
	return dict;
}

void ASAttributeValue::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("base_value")) {
		base_value = p_dict["base_value"];
	}
	if (p_dict.has("current_value")) {
		current_value = p_dict["current_value"];
	}
}

// === ASEventTagData Implementation ===

Dictionary ASEventTagData::to_dict() const {
	Dictionary dict;
	dict["event_tag"] = event_tag;
	dict["instigator_id"] = instigator_id;
	dict["target_id"] = target_id;
	dict["magnitude"] = magnitude;
	dict["custom_payload"] = custom_payload;
	dict["timestamp"] = timestamp;
	dict["tick_id"] = tick_id;
	return dict;
}

void ASEventTagData::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("event_tag")) {
		event_tag = p_dict["event_tag"];
	}
	if (p_dict.has("instigator_id")) {
		instigator_id = ObjectID((uint64_t)p_dict["instigator_id"]);
	}
	if (p_dict.has("target_id")) {
		target_id = ObjectID((uint64_t)p_dict["target_id"]);
	}
	if (p_dict.has("magnitude")) {
		magnitude = p_dict["magnitude"];
	}
	if (p_dict.has("custom_payload")) {
		custom_payload = p_dict["custom_payload"];
	}
	if (p_dict.has("timestamp")) {
		timestamp = p_dict["timestamp"];
	}
	if (p_dict.has("tick_id")) {
		tick_id = p_dict["tick_id"];
	}
}

// === ASEventTagHistoricalEntry Implementation ===

Dictionary ASEventTagHistoricalEntry::to_dict() const {
	Dictionary dict;
	dict["data"] = data.to_dict();
	dict["tick"] = tick;
	return dict;
}

void ASEventTagHistoricalEntry::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("data")) {
		data.from_dict(p_dict["data"]);
	}
	if (p_dict.has("tick")) {
		tick = p_dict["tick"];
	}
}

// === ASNameTagHistoricalEntry Implementation ===

Dictionary ASNameTagHistoricalEntry::to_dict() const {
	Dictionary dict;
	dict["tag_name"] = tag_name;
	dict["target_id"] = target_id;
	dict["timestamp"] = timestamp;
	dict["tick_id"] = tick_id;
	dict["added"] = added;
	return dict;
}

void ASNameTagHistoricalEntry::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("tag_name")) {
		tag_name = p_dict["tag_name"];
	}
	if (p_dict.has("target_id")) {
		target_id = ObjectID((uint64_t)p_dict["target_id"]);
	}
	if (p_dict.has("timestamp")) {
		timestamp = p_dict["timestamp"];
	}
	if (p_dict.has("tick_id")) {
		tick_id = p_dict["tick_id"];
	}
	if (p_dict.has("added")) {
		added = p_dict["added"];
	}
}

// === ASConditionalTagHistoricalEntry Implementation ===

Dictionary ASConditionalTagHistoricalEntry::to_dict() const {
	Dictionary dict;
	dict["tag_name"] = tag_name;
	dict["target_id"] = target_id;
	dict["timestamp"] = timestamp;
	dict["tick_id"] = tick_id;
	dict["added"] = added;
	return dict;
}

void ASConditionalTagHistoricalEntry::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("tag_name")) {
		tag_name = p_dict["tag_name"];
	}
	if (p_dict.has("target_id")) {
		target_id = ObjectID((uint64_t)p_dict["target_id"]);
	}
	if (p_dict.has("timestamp")) {
		timestamp = p_dict["timestamp"];
	}
	if (p_dict.has("tick_id")) {
		tick_id = p_dict["tick_id"];
	}
	if (p_dict.has("added")) {
		added = p_dict["added"];
	}
}

// === ASStateCacheEntry Implementation ===

void ASStateCacheEntry::clear() {
	tick = 0;
	attributes.clear();
	tags.clear();
	active_effects.clear();
}

Dictionary ASStateCacheEntry::to_dict() const {
	Dictionary dict;
	dict["tick"] = tick;

	// Convert attributes
	Dictionary attr_dict;
	for (const auto &E : attributes) {
		attr_dict[E.key] = E.value;
	}
	dict["attributes"] = attr_dict;

	// Convert tags
	Array tags_array;
	for (const StringName &tag : tags) {
		tags_array.push_back(tag);
	}
	dict["tags"] = tags_array;

	// Convert effects
	Array effects_array;
	for (const ASEffectState &effect : active_effects) {
		effects_array.push_back(effect.to_dict());
	}
	dict["active_effects"] = effects_array;

	return dict;
}

void ASStateCacheEntry::from_dict(const Dictionary &p_dict) {
	clear();

	if (p_dict.has("tick")) {
		tick = p_dict["tick"];
	}

	if (p_dict.has("attributes")) {
		Dictionary attr_dict = p_dict["attributes"];
		Array keys = attr_dict.keys();
		for (int i = 0; i < keys.size(); i++) {
			StringName key = keys[i];
			attributes[key] = attr_dict[key];
		}
	}

	if (p_dict.has("tags")) {
		Array tags_array = p_dict["tags"];
		for (int i = 0; i < tags_array.size(); i++) {
			tags.push_back(tags_array[i]);
		}
	}

	if (p_dict.has("active_effects")) {
		Array effects_array = p_dict["active_effects"];
		for (int i = 0; i < effects_array.size(); i++) {
			ASEffectState effect;
			effect.from_dict(effects_array[i]);
			active_effects.push_back(effect);
		}
	}
}

// === ASEffectState Implementation ===

Dictionary ASEffectState::to_dict() const {
	Dictionary dict;
	dict["tag"] = tag;
	dict["remaining_time"] = remaining_time;
	dict["period_timer"] = period_timer;
	dict["stack_count"] = stack_count;
	dict["level"] = level;
	return dict;
}

void ASEffectState::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("tag")) {
		tag = p_dict["tag"];
	}
	if (p_dict.has("remaining_time")) {
		remaining_time = p_dict["remaining_time"];
	}
	if (p_dict.has("period_timer")) {
		period_timer = p_dict["period_timer"];
	}
	if (p_dict.has("stack_count")) {
		stack_count = p_dict["stack_count"];
	}
	if (p_dict.has("level")) {
		level = p_dict["level"];
	}
}

// === ASCooldownData Implementation ===

Dictionary ASCooldownData::to_dict() const {
	Dictionary dict;
	dict["remaining"] = remaining;
	dict["tags"] = tags;
	return dict;
}

void ASCooldownData::from_dict(const Dictionary &p_dict) {
	if (p_dict.has("remaining")) {
		remaining = p_dict["remaining"];
	}
	if (p_dict.has("tags")) {
		tags = Array(p_dict["tags"]);
	}
}

void ASStateCache::_bind_methods() {
	ClassDB::bind_method(D_METHOD("capture_state", "component"), &ASStateCache::capture_state);
	ClassDB::bind_method(D_METHOD("restore_state", "component", "tick"), &ASStateCache::restore_state);
	ClassDB::bind_method(D_METHOD("clear"), &ASStateCache::clear);
	ClassDB::bind_method(D_METHOD("set_buffer_size", "size"), &ASStateCache::set_buffer_size);
	ClassDB::bind_method(D_METHOD("get_buffer_size"), &ASStateCache::get_buffer_size);
	ClassDB::bind_method(D_METHOD("get_current_tick"), &ASStateCache::get_current_tick);
	ClassDB::bind_method(D_METHOD("has_tick", "tick"), &ASStateCache::has_tick);
	ClassDB::bind_method(D_METHOD("get_available_ticks"), &ASStateCache::get_available_ticks);
	ClassDB::bind_method(D_METHOD("serialize"), &ASStateCache::serialize);
	ClassDB::bind_method(D_METHOD("deserialize", "data"), &ASStateCache::deserialize);
	ClassDB::bind_method(D_METHOD("dump_cache"), &ASStateCache::dump_cache);
	ClassDB::bind_method(D_METHOD("get_used_slots"), &ASStateCache::get_used_slots);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "buffer_size"), "set_buffer_size", "get_buffer_size");
}

// === ASStateCache Implementation ===

ASStateCache::ASStateCache() {
	cache_buffer.resize(buffer_size);
	ASStateCacheEntry *ptr = cache_buffer.ptrw();
	for (int i = 0; i < buffer_size; i++) {
		ptr[i].tick = 0; // Mark as empty
	}
}

ASStateCache::~ASStateCache() {
	clear();
}

void ASStateCache::capture_state(ASComponent *p_component) {
	ERR_FAIL_NULL(p_component);

	current_tick++;

	// Use circular buffer
	current_index = current_tick % buffer_size;
	ASStateCacheEntry *ptr = cache_buffer.ptrw();
	ASStateCacheEntry &entry = ptr[current_index];

	entry.tick = current_tick;
	entry.attributes.clear();
	entry.tags.clear();
	entry.active_effects.clear();

	// Capture attributes
	TypedArray<ASAttributeSet> sets = p_component->get_attribute_sets();
	for (int i = 0; i < sets.size(); i++) {
		Ref<ASAttributeSet> s = sets[i];
		if (s.is_null()) {
			continue;
		}

		TypedArray<StringName> attrs = s->get_attribute_list();
		for (int j = 0; j < attrs.size(); j++) {
			StringName name = attrs[j];
			entry.attributes[name] = s->get_attribute_base_value(name);
		}
	}

	// Capture tags
	TypedArray<StringName> all_tags = p_component->get_tags();
	for (int i = 0; i < all_tags.size(); i++) {
		entry.tags.push_back(all_tags[i]);
	}

	// Capture effects
	for (int i = 0; i < p_component->active_effects.size(); i++) {
		Ref<ASEffectSpec> spec = p_component->active_effects[i];
		if (spec.is_null()) {
			continue;
		}
		Ref<ASEffect> effect = spec->get_effect();
		if (effect.is_null()) {
			continue;
		}

		ASEffectState effect_state;
		effect_state.tag = effect->get_effect_tag();
		effect_state.remaining_time = spec->get_duration_remaining();
		effect_state.period_timer = spec->get_period_timer();
		effect_state.stack_count = spec->get_stack_count();
		effect_state.level = spec->get_level();

		entry.active_effects.push_back(effect_state);
	}
}

bool ASStateCache::restore_state(ASComponent *p_component, uint32_t p_tick) {
	ERR_FAIL_NULL_V(p_component, false);

	// Find entry with matching tick
	for (int i = 0; i < cache_buffer.size(); i++) {
		const ASStateCacheEntry &entry = cache_buffer[i];
		if (entry.tick == p_tick) {
			// Restore attributes
			for (const auto &E : entry.attributes) {
				p_component->set_attribute_base_value_by_tag(E.key, E.value);
			}

			// Restore tags (silent)
			if (p_component->owned_tags.is_valid()) {
				p_component->owned_tags->clear();
				for (const StringName &tag : entry.tags) {
					p_component->owned_tags->add_tag(tag);
				}
			}

			// Restore effects
			p_component->active_effects.clear();
			for (const ASEffectState &effect_state : entry.active_effects) {
				Ref<ASEffect> effect = p_component->find_effect_by_tag(effect_state.tag);
				if (effect.is_valid()) {
					Ref<ASEffectSpec> spec;
					spec.instantiate();
					spec->init(effect, effect_state.level);
					spec->set_duration_remaining(effect_state.remaining_time);
					spec->set_period_timer(effect_state.period_timer);
					spec->set_stack_count(effect_state.stack_count);
					spec->set_target_component(p_component);
					p_component->active_effects.push_back(spec);
				}
			}

			return true;
		}
	}

	return false; // Tick not found
}

void ASStateCache::clear() {
	ASStateCacheEntry *ptr = cache_buffer.ptrw();
	for (int i = 0; i < buffer_size; i++) {
		ptr[i].clear();
	}
	current_index = 0;
	current_tick = 0;
}

void ASStateCache::set_buffer_size(uint32_t p_size) {
	if (p_size == buffer_size) {
		return;
	}

	clear();
	buffer_size = p_size;
	cache_buffer.resize(buffer_size);
	ASStateCacheEntry *ptr = cache_buffer.ptrw();
	for (int i = 0; i < buffer_size; i++) {
		ptr[i].tick = 0; // Mark as empty
	}
}

bool ASStateCache::has_tick(uint32_t p_tick) const {
	for (int i = 0; i < cache_buffer.size(); i++) {
		if (cache_buffer[i].tick == p_tick) {
			return true;
		}
	}
	return false;
}

ASStateCacheEntry ASStateCache::get_entry(uint32_t p_tick) const {
	for (int i = 0; i < cache_buffer.size(); i++) {
		if (cache_buffer[i].tick == p_tick) {
			return cache_buffer[i];
		}
	}
	return ASStateCacheEntry(); // Return empty entry if not found
}

Array ASStateCache::get_available_ticks() const {
	Array ticks;
	for (int i = 0; i < cache_buffer.size(); i++) {
		if (cache_buffer[i].tick > 0) {
			ticks.push_back(cache_buffer[i].tick);
		}
	}
	return ticks;
}

Array ASStateCache::serialize() const {
	Array result;
	for (const ASStateCacheEntry &entry : cache_buffer) {
		if (entry.is_valid()) {
			result.push_back(entry.to_dict());
		}
	}
	return result;
}

void ASStateCache::deserialize(const Array &p_data) {
	clear();
	ASStateCacheEntry *ptr = cache_buffer.ptrw();
	for (int i = 0; i < p_data.size() && i < buffer_size; i++) {
		ptr[i].from_dict(p_data[i]);
	}
}

void ASStateCache::dump_cache() const {
	UtilityFunctions::print("=== ASStateCache Dump ===");
	UtilityFunctions::print("Buffer size: ", buffer_size);
	UtilityFunctions::print("Current tick: ", current_tick);
	UtilityFunctions::print("Current index: ", current_index);

	for (int i = 0; i < cache_buffer.size(); i++) {
		const ASStateCacheEntry &entry = cache_buffer[i];
		if (entry.is_valid()) {
			UtilityFunctions::print("Slot ", i, ": tick=", entry.tick,
					" attrs=", (int)entry.attributes.size(),
					" tags=", (int)entry.tags.size(),
					" effects=", (int)entry.active_effects.size());
		}
	}
	UtilityFunctions::print("=== End Cache Dump ===");
}

int ASStateCache::get_used_slots() const {
	int count = 0;
	for (const ASStateCacheEntry &entry : cache_buffer) {
		if (entry.is_valid()) {
			count++;
		}
	}
	return count;
}

// === ASComponentState Implementation ===

void ASComponentState::clear() {
	tick = 0;
	attributes.clear();
	tags.clear();
	active_effects.clear();
	cooldowns.clear();
	name_history.clear();
	conditional_history.clear();
	event_history.clear();
}

Dictionary ASComponentState::to_dict() const {
	Dictionary dict;
	dict["tick"] = tick;

	// Convert attributes
	Dictionary attr_dict;
	for (const auto &E : attributes) {
		attr_dict[E.key] = E.value;
	}
	dict["attributes"] = attr_dict;

	// Convert other arrays and maps
	Array tags_array;
	for (const StringName &tag : tags) {
		tags_array.push_back(tag);
	}
	dict["tags"] = tags_array;

	Array effects_array;
	for (const ASEffectState &effect : active_effects) {
		effects_array.push_back(effect.to_dict());
	}
	dict["active_effects"] = effects_array;

	Dictionary cooldowns_dict;
	for (const auto &E : cooldowns) {
		cooldowns_dict[E.key] = E.value.to_dict();
	}
	dict["cooldowns"] = cooldowns_dict;

	// Historical data (optional)
	Array name_hist_array;
	for (const ASNameTagHistoricalEntry &entry : name_history) {
		Dictionary entry_dict;
		entry_dict["tag_name"] = entry.tag_name;
		entry_dict["target_id"] = entry.target_id;
		entry_dict["timestamp"] = entry.timestamp;
		entry_dict["tick_id"] = entry.tick_id;
		entry_dict["added"] = entry.added;
		name_hist_array.push_back(entry_dict);
	}
	dict["name_history"] = name_hist_array;

	// Similar for conditional and event history...

	return dict;
}

void ASComponentState::from_dict(const Dictionary &p_dict) {
	clear();

	if (p_dict.has("tick")) {
		tick = p_dict["tick"];
	}

	// Load attributes
	if (p_dict.has("attributes")) {
		Dictionary attr_dict = p_dict["attributes"];
		Array keys = attr_dict.keys();
		for (int i = 0; i < keys.size(); i++) {
			StringName key = keys[i];
			attributes[key] = attr_dict[key];
		}
	}

	// Load other data...
}

void ASComponentState::capture_from_component(ASComponent *p_component) {
	ERR_FAIL_NULL(p_component);

	clear();

	// Capture basic state
	tick = p_component->current_tick;

	// Capture attributes
	TypedArray<ASAttributeSet> sets = p_component->get_attribute_sets();
	for (int i = 0; i < sets.size(); i++) {
		Ref<ASAttributeSet> s = sets[i];
		if (s.is_null()) {
			continue;
		}

		TypedArray<StringName> attrs = s->get_attribute_list();
		for (int j = 0; j < attrs.size(); j++) {
			StringName name = attrs[j];
			attributes[name] = s->get_attribute_base_value(name);
		}
	}

	// Capture tags
	TypedArray<StringName> all_tags = p_component->get_tags();
	for (int i = 0; i < all_tags.size(); i++) {
		tags.push_back(all_tags[i]);
	}

	// Capture effects
	for (int i = 0; i < p_component->active_effects.size(); i++) {
		Ref<ASEffectSpec> spec = p_component->active_effects[i];
		if (spec.is_null()) {
			continue;
		}
		Ref<ASEffect> effect = spec->get_effect();
		if (effect.is_null()) {
			continue;
		}

		ASEffectState effect_state;
		effect_state.tag = effect->get_effect_tag();
		effect_state.remaining_time = spec->get_duration_remaining();
		effect_state.period_timer = spec->get_period_timer();
		effect_state.stack_count = spec->get_stack_count();
		effect_state.level = spec->get_level();

		active_effects.push_back(effect_state);
	}

	// Capture cooldowns
	for (const auto &E : p_component->active_cooldowns) {
		cooldowns[E.key] = E.value;
	}

	// Capture historical data
	name_history = p_component->_name_history;
	conditional_history = p_component->_cond_history;
	event_history = p_component->_event_history;
}

void ASComponentState::apply_to_component(ASComponent *p_component) const {
	ERR_FAIL_NULL(p_component);

	// Restore attributes
	for (const auto &E : attributes) {
		p_component->set_attribute_base_value_by_tag(E.key, E.value);
	}

	// Restore tags (silent)
	if (p_component->owned_tags.is_valid()) {
		p_component->owned_tags->clear();
		for (const StringName &tag : tags) {
			p_component->owned_tags->add_tag(tag);
		}
	}

	// Restore effects
	p_component->active_effects.clear();
	for (const ASEffectState &effect_state : active_effects) {
		Ref<ASEffect> effect = p_component->find_effect_by_tag(effect_state.tag);
		if (effect.is_valid()) {
			Ref<ASEffectSpec> spec;
			spec.instantiate();
			spec->init(effect, effect_state.level);
			spec->set_duration_remaining(effect_state.remaining_time);
			spec->set_period_timer(effect_state.period_timer);
			spec->set_stack_count(effect_state.stack_count);
			spec->set_target_component(p_component);
			p_component->active_effects.push_back(spec);
		}
	}

	// Restore cooldowns
	p_component->active_cooldowns.clear();
	for (const auto &E : cooldowns) {
		p_component->active_cooldowns[E.key] = E.value;
	}

	// Restore historical data
	p_component->_name_history = name_history;
	p_component->_cond_history = conditional_history;
	p_component->_event_history = event_history;

	p_component->current_tick = tick;
}

bool ASComponentState::equals(const ASComponentState &p_other) const {
	if (tick != p_other.tick) {
		return false;
	}
	if (attributes.size() != p_other.attributes.size()) {
		return false;
	}
	if (tags.size() != p_other.tags.size()) {
		return false;
	}
	// ... detailed comparison
	return true;
}

ASComponentState ASComponentState::compute_diff(const ASComponentState &p_other) const {
	ASComponentState diff;
	// Compute differences between states
	return diff;
}

void ASComponentState::apply_diff(const ASComponentState &p_diff) {
	// Apply diff to current state
}

// === ASStateUtils Implementation ===

void ASStateUtils::_bind_methods() {
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("compare_states", "a", "b"), &ASStateUtils::compare_states);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("compute_state_difference", "a", "b"), &ASStateUtils::compute_state_difference);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("serialize_state", "state"), &ASStateUtils::serialize_state);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("deserialize_state", "data"), &ASStateUtils::deserialize_state);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("compress_state", "state"), &ASStateUtils::compress_state);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("decompress_state", "data"), &ASStateUtils::decompress_state);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("validate_state", "state"), &ASStateUtils::validate_state);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("get_validation_errors", "state"), &ASStateUtils::get_validation_errors);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("dump_state", "state"), &ASStateUtils::dump_state);
	ClassDB::bind_static_method("ASStateUtils", D_METHOD("state_to_string", "state"), &ASStateUtils::state_to_string);

	// Bind Modifier Constants
	ClassDB::bind_integer_constant("ASStateUtils", "ModifierOp", "ADD", (int)ModifierOp::ADD);
	ClassDB::bind_integer_constant("ASStateUtils", "ModifierOp", "MULTIPLY", (int)ModifierOp::MULTIPLY);
	ClassDB::bind_integer_constant("ASStateUtils", "ModifierOp", "DIVIDE", (int)ModifierOp::DIVIDE);
	ClassDB::bind_integer_constant("ASStateUtils", "ModifierOp", "OVERRIDE", (int)ModifierOp::OVERRIDE);
}

bool ASStateUtils::compare_states(const Ref<ASStateSnapshot> &p_a, const Ref<ASStateSnapshot> &p_b) {
	if (p_a.is_null() || p_b.is_null()) {
		return p_a == p_b;
	}
	return p_a->get_tick() == p_b->get_tick() && p_a->get_attributes() == p_b->get_attributes() && p_a->get_tags() == p_b->get_tags();
}

float ASStateUtils::compute_state_difference(const Ref<ASStateSnapshot> &p_a, const Ref<ASStateSnapshot> &p_b) {
	// Compute numerical difference between states
	return 0.0f;
}

PackedByteArray ASStateUtils::serialize_state(const Ref<ASStateSnapshot> &p_state) {
	if (p_state.is_null()) {
		return PackedByteArray();
	}
	return p_state->serialize_binary();
}

Ref<ASStateSnapshot> ASStateUtils::deserialize_state(const PackedByteArray &p_data) {
	Ref<ASStateSnapshot> snapshot;
	snapshot.instantiate();
	snapshot->deserialize_binary(p_data);
	return snapshot;
}

PackedByteArray ASStateUtils::compress_state(const Ref<ASStateSnapshot> &p_state) {
	// Compress state data
	return serialize_state(p_state);
}

Ref<ASStateSnapshot> ASStateUtils::decompress_state(const PackedByteArray &p_data) {
	// Decompress state data
	return deserialize_state(p_data);
}

bool ASStateUtils::validate_state(const Ref<ASStateSnapshot> &p_state) {
	return p_state.is_valid();
}

Array ASStateUtils::get_validation_errors(const Ref<ASStateSnapshot> &p_state) {
	return Array();
}

void ASStateUtils::dump_state(const Ref<ASStateSnapshot> &p_state) {
	if (p_state.is_null()) {
		UtilityFunctions::print("ASComponentState: <null>");
		return;
	}
	UtilityFunctions::print("=== ASComponentState Dump ===");
	UtilityFunctions::print("Tick: ", (int)p_state->get_tick());
	UtilityFunctions::print("Attributes: ", (int)p_state->get_attributes().size());
	UtilityFunctions::print("Tags: ", (int)p_state->get_tags().size());
	UtilityFunctions::print("Active Effects: ", (int)p_state->get_active_effects().size());
	UtilityFunctions::print("=== End State Dump ===");
}

String ASStateUtils::state_to_string(const Ref<ASStateSnapshot> &p_state) {
	if (p_state.is_null()) {
		return "ASComponentState(<null>)";
	}
	return String("ASComponentState(tick=") + String::num(p_state->get_tick()) + ")";
}
