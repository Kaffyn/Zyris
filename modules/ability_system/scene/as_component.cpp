/**************************************************************************/
/*  as_component.cpp                                                      */
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
#include "src/scene/as_component.h"
#include "src/core/ability_system.h"
#include "src/core/as_ability_spec.h"
#include "src/core/as_cue_spec.h"
#include "src/core/as_effect_spec.h"
#include "src/core/as_tag_spec.h"
#include "src/resources/as_ability.h"
#include "src/resources/as_attribute.h"
#include "src/resources/as_attribute_set.h"
#include "src/resources/as_container.h"
#include "src/resources/as_cue.h"
#include "src/resources/as_effect.h"
#include "src/resources/as_package.h"
#include "src/resources/as_state_snapshot.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_ability_spec.h"
#include "modules/ability_system/core/as_cue_spec.h"
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/core/as_tag_spec.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/resources/as_container.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/resources/as_package.h"
#include "modules/ability_system/resources/as_state_snapshot.h"
#include "modules/ability_system/scene/as_component.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/animated_sprite3d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_player2d.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/multiplayer_api.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/sprite3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#else
#include "core/config/project_settings.h"
#include "scene/2d/animated_sprite_2d.h"
#include "scene/2d/audio_stream_player_2d.h"
#include "scene/2d/physics/character_body_2d.h"
#include "scene/3d/audio_stream_player_3d.h"
#include "scene/3d/physics/character_body_3d.h"
#include "scene/3d/sprite_3d.h"
#include "scene/animation/animation_player.h"
#include "scene/audio/audio_stream_player.h"
#include "scene/main/multiplayer_api.h"
#include "servers/audio/audio_stream.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

// Methods implementation

void ASComponent::_bind_methods() {
	// --- Ability Container ---
	ClassDB::bind_method(D_METHOD("set_container", "container"), &ASComponent::set_container);
	ClassDB::bind_method(D_METHOD("get_container"), &ASComponent::get_container);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "container", PROPERTY_HINT_RESOURCE_TYPE, "ASContainer"), "set_container", "get_container");

	ClassDB::bind_method(D_METHOD("set_snapshot_state", "snapshot"), &ASComponent::set_snapshot_state);
	ClassDB::bind_method(D_METHOD("get_snapshot_state"), &ASComponent::get_snapshot_state);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "snapshot_state", PROPERTY_HINT_RESOURCE_TYPE, "ASStateSnapshot"), "set_snapshot_state", "get_snapshot_state");

	// --- Initialization API ---
	ClassDB::bind_method(D_METHOD("apply_container", "container", "level"), &ASComponent::apply_container, DEFVAL(1));
	ClassDB::bind_method(D_METHOD("add_attribute_set", "set"), &ASComponent::add_attribute_set);

	// --- Ability Management API ---
	ClassDB::bind_method(D_METHOD("unlock_ability_by_tag", "tag"), &ASComponent::unlock_ability_by_tag);
	ClassDB::bind_method(D_METHOD("unlock_ability_by_resource", "ability"), &ASComponent::unlock_ability_by_resource);
	ClassDB::bind_method(D_METHOD("lock_ability_by_tag", "tag"), &ASComponent::lock_ability_by_tag);
	ClassDB::bind_method(D_METHOD("lock_ability_by_resource", "ability"), &ASComponent::lock_ability_by_resource);
	ClassDB::bind_method(D_METHOD("is_ability_unlocked", "tag"), &ASComponent::is_ability_unlocked);

	// --- Ability Activation API (By Tag) ---
	ClassDB::bind_method(D_METHOD("can_activate_ability_by_tag", "tag"), &ASComponent::can_activate_ability_by_tag);
	ClassDB::bind_method(D_METHOD("try_activate_ability_by_tag", "tag"), &ASComponent::try_activate_ability_by_tag);
	ClassDB::bind_method(D_METHOD("cancel_ability_by_tag", "tag"), &ASComponent::cancel_ability_by_tag);
	ClassDB::bind_method(D_METHOD("is_ability_active", "tag"), &ASComponent::is_ability_active);

	// --- Ability Activation API (By Resource) ---
	ClassDB::bind_method(D_METHOD("can_activate_ability_by_resource", "ability"), &ASComponent::can_activate_ability_by_resource);
	ClassDB::bind_method(D_METHOD("try_activate_ability_by_resource", "ability"), &ASComponent::try_activate_ability_by_resource);
	ClassDB::bind_method(D_METHOD("cancel_ability_by_resource", "ability"), &ASComponent::cancel_ability_by_resource);

	ClassDB::bind_method(D_METHOD("cancel_all_abilities"), &ASComponent::cancel_all_abilities);

	// --- Effect Activation API (By Tag) ---
	ClassDB::bind_method(D_METHOD("can_activate_effect_by_tag", "tag"), &ASComponent::can_activate_effect_by_tag);
	ClassDB::bind_method(D_METHOD("try_activate_effect_by_tag", "tag", "level", "target_node"), &ASComponent::try_activate_effect_by_tag, DEFVAL(1.0f), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("cancel_effect_by_tag", "tag"), &ASComponent::cancel_effect_by_tag);

	// --- Effect Activation API (By Resource) ---
	ClassDB::bind_method(D_METHOD("can_activate_effect_by_resource", "effect"), &ASComponent::can_activate_effect_by_resource);
	ClassDB::bind_method(D_METHOD("try_activate_effect_by_resource", "effect", "level", "target_node"), &ASComponent::try_activate_effect_by_resource, DEFVAL(1.0f), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("cancel_effect_by_resource", "effect"), &ASComponent::cancel_effect_by_resource);

	// --- Effect Specification API (Low level) ---
	ClassDB::bind_method(D_METHOD("make_outgoing_spec", "effect", "level", "target_node"), &ASComponent::make_outgoing_spec, DEFVAL(1.0f), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("apply_effect_spec_to_self", "spec"), &ASComponent::apply_effect_spec_to_self);
	ClassDB::bind_method(D_METHOD("apply_effect_spec_to_target", "spec", "target"), &ASComponent::apply_effect_spec_to_target);
	ClassDB::bind_method(D_METHOD("remove_active_effect", "spec"), &ASComponent::remove_active_effect);
	ClassDB::bind_method(D_METHOD("remove_effect_by_tag", "tag"), &ASComponent::remove_effect_by_tag);
	ClassDB::bind_method(D_METHOD("remove_effect_by_resource", "effect"), &ASComponent::remove_effect_by_resource);
	ClassDB::bind_method(D_METHOD("clear_effects"), &ASComponent::clear_effects);
	ClassDB::bind_method(D_METHOD("apply_effect_by_tag", "tag", "level", "target_node"), &ASComponent::apply_effect_by_tag, DEFVAL(1.0f), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("apply_effect_by_resource", "effect", "level", "target_node"), &ASComponent::apply_effect_by_resource, DEFVAL(1.0f), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("apply_package", "package", "level", "source_component"), &ASComponent::apply_package, DEFVAL(1.0f), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("has_active_effect_by_tag", "tag"), &ASComponent::has_active_effect_by_tag);
	ClassDB::bind_method(D_METHOD("has_active_effect_by_resource", "effect"), &ASComponent::has_active_effect_by_resource);

	// --- Attribute API (By Tag) ---
	ClassDB::bind_method(D_METHOD("get_attribute_value_by_tag", "tag"), &ASComponent::get_attribute_value_by_tag);
	ClassDB::bind_method(D_METHOD("get_attribute_base_value_by_tag", "tag"), &ASComponent::get_attribute_base_value_by_tag);
	ClassDB::bind_method(D_METHOD("set_attribute_base_value_by_tag", "tag", "value"), &ASComponent::set_attribute_base_value_by_tag);
	ClassDB::bind_method(D_METHOD("has_attribute_by_tag", "tag"), &ASComponent::has_attribute_by_tag);

	// --- Attribute API (By Resource) ---
	ClassDB::bind_method(D_METHOD("get_attribute_value_by_resource", "attribute"), &ASComponent::get_attribute_value_by_resource);
	ClassDB::bind_method(D_METHOD("get_attribute_base_value_by_resource", "attribute"), &ASComponent::get_attribute_base_value_by_resource);
	ClassDB::bind_method(D_METHOD("set_attribute_base_value_by_resource", "attribute", "value"), &ASComponent::set_attribute_base_value_by_resource);
	ClassDB::bind_method(D_METHOD("has_attribute_by_resource", "attribute"), &ASComponent::has_attribute_by_resource);

	// --- Cooldown API ---
	ClassDB::bind_method(D_METHOD("start_cooldown", "ability_tag", "duration", "tags"), &ASComponent::start_cooldown);
	ClassDB::bind_method(D_METHOD("is_on_cooldown", "ability_tag"), &ASComponent::is_on_cooldown);
	ClassDB::bind_method(D_METHOD("get_cooldown_remaining", "ability_tag"), &ASComponent::get_cooldown_remaining);
	ClassDB::bind_method(D_METHOD("tick", "delta"), &ASComponent::tick);

	// --- Tag Management ---
	ClassDB::bind_method(D_METHOD("add_tag", "tag"), &ASComponent::add_tag);
	ClassDB::bind_method(D_METHOD("remove_tag", "tag"), &ASComponent::remove_tag);
	ClassDB::bind_method(D_METHOD("remove_all_tags"), &ASComponent::remove_all_tags);
	ClassDB::bind_method(D_METHOD("has_tag", "tag"), &ASComponent::has_tag);
	ClassDB::bind_method(D_METHOD("get_tags"), &ASComponent::get_tags);
	ClassDB::bind_method(D_METHOD("get_owned_tags"), &ASComponent::get_owned_tags);
	ClassDB::bind_method(D_METHOD("get_attribute_sets"), &ASComponent::get_attribute_sets);

	// --- Multiplayer & Prediction ---
	ClassDB::bind_method(D_METHOD("capture_snapshot"), &ASComponent::capture_snapshot);
	ClassDB::bind_method(D_METHOD("apply_snapshot", "tick"), &ASComponent::apply_snapshot);
	ClassDB::bind_method(D_METHOD("_is_server"), &ASComponent::_is_server);
	ClassDB::bind_method(D_METHOD("_is_local_authority"), &ASComponent::_is_local_authority);
	ClassDB::bind_method(D_METHOD("request_activate_ability", "tag"), &ASComponent::request_activate_ability);
	ClassDB::bind_method(D_METHOD("confirm_ability_activation", "tag"), &ASComponent::confirm_ability_activation);

	// --- Cue Activation API (By Tag) ---
	ClassDB::bind_method(D_METHOD("can_activate_cue_by_tag", "tag"), &ASComponent::can_activate_cue_by_tag);
	ClassDB::bind_method(D_METHOD("try_activate_cue_by_tag", "tag", "data", "target_node"), &ASComponent::try_activate_cue_by_tag, DEFVAL(Dictionary()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("cancel_cue_by_tag", "tag"), &ASComponent::cancel_cue_by_tag);

	// --- Cue Activation API (By Resource) ---
	ClassDB::bind_method(D_METHOD("can_activate_cue_by_resource", "cue"), &ASComponent::can_activate_cue_by_resource);
	ClassDB::bind_method(D_METHOD("try_activate_cue_by_resource", "cue", "data", "target_node"), &ASComponent::try_activate_cue_by_resource, DEFVAL(Dictionary()), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("cancel_cue_by_resource", "cue"), &ASComponent::cancel_cue_by_resource);

	ClassDB::bind_method(D_METHOD("register_cue_resource", "cue"), &ASComponent::register_cue_resource);
	ClassDB::bind_method(D_METHOD("unregister_cue_resource", "tag"), &ASComponent::unregister_cue_resource);
	ClassDB::bind_method(D_METHOD("get_cue_resource", "tag"), &ASComponent::get_cue_resource);

	// --- Montage API ---
	ClassDB::bind_method(D_METHOD("play_montage", "name", "target"), &ASComponent::play_montage, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("is_montage_playing", "name", "target"), &ASComponent::is_montage_playing, DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("play_sound", "sound", "target"), &ASComponent::play_sound, DEFVAL(Variant()));

	// --- Node Registration ---
	ClassDB::bind_method(D_METHOD("register_node", "name", "node"), &ASComponent::register_node);
	ClassDB::bind_method(D_METHOD("unregister_node", "name"), &ASComponent::unregister_node);
	ClassDB::bind_method(D_METHOD("get_node_ptr", "name"), &ASComponent::get_node_ptr);

	ClassDB::bind_method(D_METHOD("set_animation_player", "node"), &ASComponent::set_animation_player);
	ClassDB::bind_method(D_METHOD("get_animation_player"), &ASComponent::get_animation_player);
	ClassDB::bind_method(D_METHOD("set_audio_player", "node"), &ASComponent::set_audio_player);
	ClassDB::bind_method(D_METHOD("get_audio_player"), &ASComponent::get_audio_player);

	// --- Metadata Preview ---
	ClassDB::bind_method(D_METHOD("get_ability_duration_preview", "tag"), &ASComponent::get_ability_duration_preview);
	ClassDB::bind_method(D_METHOD("get_ability_cost_preview", "tag", "attribute"), &ASComponent::get_ability_cost_preview);
	ClassDB::bind_method(D_METHOD("get_ability_cooldown_preview", "tag"), &ASComponent::get_ability_cooldown_preview);

	// --- Calculation Engine ---
	ClassDB::bind_method(D_METHOD("calculate_modifier_magnitude", "spec", "modifier_index"), &ASComponent::calculate_modifier_magnitude);
	ClassDB::bind_method(D_METHOD("calculate_effect_duration", "spec"), &ASComponent::calculate_effect_duration);

	GDVIRTUAL_BIND(_on_calculate_custom_magnitude, "effect_spec", "modifier_index");

	// --- Signals ---
	ADD_SIGNAL(MethodInfo("attribute_changed", PropertyInfo(Variant::STRING_NAME, "attribute_name"), PropertyInfo(Variant::FLOAT, "old_value"), PropertyInfo(Variant::FLOAT, "new_value")));
	ADD_SIGNAL(MethodInfo("tag_changed", PropertyInfo(Variant::STRING_NAME, "tag_name"), PropertyInfo(Variant::BOOL, "is_present")));
	ADD_SIGNAL(MethodInfo("ability_activated", PropertyInfo(Variant::OBJECT, "ability_spec", PROPERTY_HINT_NONE, "ASAbilitySpec")));
	ADD_SIGNAL(MethodInfo("ability_ended", PropertyInfo(Variant::OBJECT, "ability_spec", PROPERTY_HINT_NONE, "ASAbilitySpec"), PropertyInfo(Variant::BOOL, "was_cancelled")));
	ADD_SIGNAL(MethodInfo("ability_failed", PropertyInfo(Variant::STRING_NAME, "ability_name"), PropertyInfo(Variant::STRING, "reason")));

	ADD_SIGNAL(MethodInfo("effect_failed", PropertyInfo(Variant::STRING_NAME, "effect_name"), PropertyInfo(Variant::STRING, "reason")));
	ADD_SIGNAL(MethodInfo("cue_failed", PropertyInfo(Variant::STRING_NAME, "cue_name"), PropertyInfo(Variant::STRING, "reason")));

	ADD_SIGNAL(MethodInfo("effect_applied", PropertyInfo(Variant::OBJECT, "effect_spec", PROPERTY_HINT_NONE, "ASEffectSpec")));
	ADD_SIGNAL(MethodInfo("effect_removed", PropertyInfo(Variant::OBJECT, "effect_spec", PROPERTY_HINT_NONE, "ASEffectSpec")));
	ADD_SIGNAL(MethodInfo("tag_event_received", PropertyInfo(Variant::STRING_NAME, "event_tag"), PropertyInfo(Variant::DICTIONARY, "data")));
	ADD_SIGNAL(MethodInfo("cooldown_started", PropertyInfo(Variant::STRING_NAME, "ability_tag"), PropertyInfo(Variant::FLOAT, "duration")));
	ADD_SIGNAL(MethodInfo("cooldown_ended", PropertyInfo(Variant::STRING_NAME, "ability_tag")));
}

void ASComponent::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			// Validate parent is CharacterBody2D or CharacterBody3D
			Node *parent = get_parent();
			if (!parent || (!Object::cast_to<CharacterBody2D>(parent) && !Object::cast_to<CharacterBody3D>(parent))) {
				ERR_PRINT("ASComponent FATAL: Can only be child of CharacterBody2D or CharacterBody3D. Disabling component.");
				set_physics_process(false);
				return;
			}

			// Cache the validated CharacterBody parent
			character_body_2d = Object::cast_to<CharacterBody2D>(parent);
			character_body_3d = Object::cast_to<CharacterBody3D>(parent);

			if (container.is_valid()) {
				apply_container(container);
			}

			// Explicitly disable idle process to ensure 100% physics-based operation
			set_process(false);
			set_physics_process(true);
		} break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			if (_is_local_authority()) {
				tick(get_physics_process_delta_time());
			} else {
				// Clients still need to advance their local tick clock
				current_tick++;
				// If we have active abilities (predicted), we MUST tick to process their logic
				if (!active_abilities.is_empty()) {
					tick(get_physics_process_delta_time());
				}
			}
		} break;
	}
}

void ASComponent::_process_cooldowns(float p_delta) {
	if (active_cooldowns.is_empty()) {
		return;
	}

	// Safe iteration: copy keys first to avoid iterator invalidation during re-entrant calls
	Vector<StringName> keys;
	for (const KeyValue<StringName, CooldownData> &E : active_cooldowns) {
		keys.push_back(E.key);
	}

	Vector<StringName> to_remove;
	for (int i = 0; i < keys.size(); i++) {
		StringName key = keys[i];
		if (!active_cooldowns.has(key)) {
			continue;
		}

		CooldownData &cd = active_cooldowns[key];
		cd.remaining -= p_delta;

		if (cd.remaining <= 0) {
			// Cleanup tags - this might trigger re-entrant calls!
			for (int j = 0; j < cd.tags.size(); j++) {
				remove_tag(cd.tags[j]);
			}
			to_remove.push_back(key);
		}
	}

	for (int i = 0; i < to_remove.size(); i++) {
		active_cooldowns.erase(to_remove[i]);
		emit_signal("cooldown_ended", to_remove[i]);
	}
}

void ASComponent::start_cooldown(const StringName &p_ability_tag, float p_duration, const TypedArray<StringName> &p_cooldown_tags) {
	if (p_duration <= 0) {
		return;
	}

	CooldownData cd;
	cd.remaining = p_duration;
	cd.tags = p_cooldown_tags;
	active_cooldowns[p_ability_tag] = cd;

	for (int i = 0; i < p_cooldown_tags.size(); i++) {
		add_tag(p_cooldown_tags[i]);
	}

	emit_signal("cooldown_started", p_ability_tag, p_duration);
}

bool ASComponent::is_on_cooldown(const StringName &p_ability_tag) const {
	return active_cooldowns.has(p_ability_tag);
}

float ASComponent::get_cooldown_remaining(const StringName &p_ability_tag) const {
	if (active_cooldowns.has(p_ability_tag)) {
		return active_cooldowns[p_ability_tag].remaining;
	}
	return 0.0f;
}

void ASComponent::set_attribute_base_value_by_tag(const StringName &p_tag, float p_value) {
	if (is_inside_tree() && !is_multiplayer_authority() && !_is_predicting) {
		return;
	}
	for (int i = 0; i < attribute_sets.size(); i++) {
		if (attribute_sets[i].is_valid() && attribute_sets[i]->has_attribute(p_tag)) {
			attribute_sets[i]->set_attribute_base_value(p_tag, p_value);
			_update_attribute_current_values();
			return;
		}
	}
}

void ASComponent::add_tag(const StringName &p_tag) {
	if (is_inside_tree() && !is_multiplayer_authority() && !_is_predicting) {
		return;
	}
	if (owned_tags.is_valid()) {
		if (owned_tags->add_tag(p_tag)) {
			_handle_ability_triggers(p_tag, ASAbility::TRIGGER_ON_TAG_ADDED);
			emit_signal("tag_changed", p_tag, true);
		}
	}
}

void ASComponent::remove_tag(const StringName &p_tag) {
	if (is_inside_tree() && !is_multiplayer_authority() && !_is_predicting) {
		return;
	}
	if (owned_tags.is_valid()) {
		if (owned_tags->remove_tag(p_tag)) {
			_handle_ability_triggers(p_tag, ASAbility::TRIGGER_ON_TAG_REMOVED);
			emit_signal("tag_changed", p_tag, false);
		}
	}
}

void ASComponent::remove_all_tags() {
	if (is_inside_tree() && !is_multiplayer_authority() && !_is_predicting) {
		return;
	}
	if (owned_tags.is_null()) {
		return;
	}
	TypedArray<StringName> tags = get_tags();
	owned_tags->clear();
	for (int i = 0; i < tags.size(); i++) {
		emit_signal("tag_changed", (StringName)tags[i], false);
		_handle_ability_triggers((StringName)tags[i], ASAbility::TRIGGER_ON_TAG_REMOVED);
	}
}

bool ASComponent::has_tag(const StringName &p_tag) const {
	if (owned_tags.is_null()) {
		return false;
	}
	return owned_tags->has_tag(p_tag, false);
}

TypedArray<StringName> ASComponent::get_tags() const {
	if (owned_tags.is_null()) {
		return TypedArray<StringName>();
	}
	return owned_tags->get_all_tags();
}

Ref<ASTagSpec> ASComponent::get_owned_tags() const {
	return owned_tags;
}

void ASComponent::tick(float p_delta) {
	current_tick++;
	_process_effects(p_delta);
	_process_abilities(p_delta);
	_process_cooldowns(p_delta);

	capture_snapshot(); // Cache every tick for accurate prediction/rollback
}

void ASComponent::_process_effects(float p_delta) {
	if (active_effects.is_empty()) {
		return;
	}

	bool removed_any = false;
	bool recalculate_needed = false;

	// Use a copy for safe iteration because _remove_effect_at_index triggers tags/signals
	// which can modify the active_effects vector re-entrantly.
	Vector<Ref<ASEffectSpec>> effects_to_process = active_effects;

	for (int i = effects_to_process.size() - 1; i >= 0; i--) {
		Ref<ASEffectSpec> spec = effects_to_process[i];
		if (spec.is_null()) {
			continue;
		}

		// Verify it's still in the main list
		int current_idx = active_effects.find(spec);
		if (current_idx == -1) {
			continue;
		}

		Ref<ASEffect> effect = spec->get_effect();

		// --- Duration countdown ---
		if (effect->get_duration_policy() == ASEffect::POLICY_DURATION) {
			float remaining = spec->get_duration_remaining() - p_delta;
			spec->set_duration_remaining(remaining);
			if (remaining <= 0) {
				_remove_effect_at_index(current_idx);
				removed_any = true;
				continue;
			}
		}

		// --- Periodic tick ---
		if (effect->get_period() > 0.0f) {
			float timer = spec->get_period_timer() - p_delta;
			if (timer <= 0.0f) {
				timer += effect->get_period();
				// Apply instant-style attribute modification for this tick
				for (int j = 0; j < effect->get_modifier_count(); j++) {
					StringName attr = effect->get_modifier_attribute(j);
					ASEffect::ModifierOp op = effect->get_modifier_operation(j);
					float mag = calculate_modifier_magnitude(spec, j);
					float current_base = get_attribute_base_value_by_tag(attr);
					float next_base = current_base;
					switch (op) {
						case ASEffect::OP_ADD:
							next_base += mag;
							break;
						case ASEffect::OP_MULTIPLY:
							next_base *= mag;
							break;
						case ASEffect::OP_DIVIDE:
							if (mag != 0) {
								next_base /= mag;
							}
							break;
						case ASEffect::OP_OVERRIDE:
							next_base = mag;
							break;
					}
					set_attribute_base_value_by_tag(attr, next_base);
				}
				recalculate_needed = true;
			}
			spec->set_period_timer(timer);
		}
	}

	if (removed_any || recalculate_needed) {
		_update_attribute_current_values();
	}
}

void ASComponent::_process_abilities(float p_delta) {
	if (active_abilities.is_empty()) {
		return;
	}

	// Safe iteration: copy to handle re-entrant activation/cancellation
	Vector<Ref<ASAbilitySpec>> to_process = active_abilities;

	for (int i = to_process.size() - 1; i >= 0; i--) {
		Ref<ASAbilitySpec> spec = to_process[i];
		if (spec.is_null()) {
			continue;
		}

		// Process only if still active and in our real list
		int current_idx = active_abilities.find(spec);
		if (current_idx == -1) {
			continue;
		}

		if (spec->tick(p_delta)) {
			Ref<ASAbility> ability = spec->get_ability();
			ability->end_ability(this, spec);
			spec->set_is_active(false);

			// Must re-find index as the list might have changed due to end_ability triggers
			current_idx = active_abilities.find(spec);
			if (current_idx != -1) {
				active_abilities.remove_at(current_idx);
			}

			emit_signal("ability_ended", spec, false);
		}
	}
}

void ASComponent::_remove_effect_at_index(int p_idx) {
	Ref<ASEffectSpec> spec = active_effects[p_idx];
	Ref<ASEffect> effect = spec->get_effect();

	// Remove tags
	TypedArray<StringName> granted = effect->get_granted_tags();
	for (int i = 0; i < granted.size(); i++) {
		remove_tag(granted[i]);
	}

	// Trigger Cues (Tags and Direct)
	{
		// 1. Cue Tags (Registry based)
		StringName effect_tag = effect->get_effect_tag();
		if (effect_tag != StringName()) {
			Ref<ASCue> cue = get_cue_resource(effect_tag);
			if (cue.is_null() && spec->get_source_component()) {
				cue = spec->get_source_component()->get_cue_resource(effect_tag);
			}

			if (cue.is_valid() && cue->get_event_type() == ASCue::ON_REMOVE) {
				Ref<ASCueSpec> cue_spec;
				cue_spec.instantiate();
				cue_spec->init_from_effect(cue, spec->get_source_component(), this, spec, 0.0f);
				_execute_cue_with_spec(effect_tag, cue_spec);
			} else if (cue.is_null()) {
				emit_signal("tag_event_received", effect_tag, Dictionary());
			}
		}

		// 2. Direct Cues (Resource based)
		TypedArray<ASCue> cues = effect->get_cues();
		for (int i = 0; i < cues.size(); i++) {
			Ref<ASCue> cue = cues[i];
			if (cue.is_valid() && cue->get_event_type() == ASCue::ON_REMOVE) {
				Ref<ASCueSpec> cue_spec;
				cue_spec.instantiate();
				cue_spec->init_from_effect(cue, spec->get_source_component(), this, spec, 0.0f);
				cue->execute(cue_spec);
			}
		}
	}

	active_effects.remove_at(p_idx);
	emit_signal("effect_removed", spec);

	// Recalculate if duration/infinite
	if (effect->get_duration_policy() != ASEffect::POLICY_INSTANT) {
		_update_attribute_current_values();
	}
}

void ASComponent::remove_active_effect(Ref<ASEffectSpec> p_spec) {
	ERR_FAIL_COND(p_spec.is_null());
	int idx = active_effects.find(p_spec);
	if (idx != -1) {
		_remove_effect_at_index(idx);
		_update_attribute_current_values();
	}
}

void ASComponent::remove_effect_by_tag(const StringName &p_tag) {
	Vector<Ref<ASEffectSpec>> to_remove;
	for (int i = 0; i < active_effects.size(); i++) {
		Ref<ASEffect> effect = active_effects[i]->get_effect();
		if (effect.is_valid() && effect->get_effect_tag() == p_tag) {
			to_remove.push_back(active_effects[i]);
		}
	}

	for (int i = 0; i < to_remove.size(); i++) {
		remove_active_effect(to_remove[i]);
	}
}

void ASComponent::remove_effect_by_resource(const Ref<ASEffect> &p_effect) {
	ERR_FAIL_COND(p_effect.is_null());
	Vector<Ref<ASEffectSpec>> to_remove;
	for (int i = 0; i < active_effects.size(); i++) {
		if (active_effects[i]->get_effect() == p_effect) {
			to_remove.push_back(active_effects[i]);
		}
	}

	for (int i = 0; i < to_remove.size(); i++) {
		remove_active_effect(to_remove[i]);
	}
}

void ASComponent::clear_effects() {
	for (int i = active_effects.size() - 1; i >= 0; i--) {
		_remove_effect_at_index(i);
	}
	_update_attribute_current_values();
}

bool ASComponent::has_active_effect_by_tag(const StringName &p_tag) const {
	for (int i = 0; i < active_effects.size(); i++) {
		Ref<ASEffect> effect = active_effects[i]->get_effect();
		if (effect.is_valid() && effect->get_effect_tag() == p_tag) {
			return true;
		}
	}
	return false;
}

bool ASComponent::has_active_effect_by_resource(const Ref<ASEffect> &p_effect) const {
	for (int i = 0; i < active_effects.size(); i++) {
		if (active_effects[i]->get_effect() == p_effect) {
			return true;
		}
	}
	return false;
}

void ASComponent::cancel_all_abilities() {
	for (int i = active_abilities.size() - 1; i >= 0; i--) {
		Ref<ASAbilitySpec> spec = active_abilities[i];
		if (spec.is_valid() && spec->get_is_active()) {
			Ref<ASAbility> ability = spec->get_ability();
			if (ability.is_valid()) {
				ability->end_ability(this, spec);
				spec->set_is_active(false);
				emit_signal("ability_ended", spec, true);
			}
		}
	}
	active_abilities.clear();
}

void ASComponent::apply_container(Ref<ASContainer> p_container, int p_level) {
	ERR_FAIL_COND(p_container.is_null());

	// 1. Duplicate container (shallow) to allow local modifications without affecting the resource file
	// but keeping the same effect/ability resource references (blueprints).
	Ref<ASContainer> local_container = p_container->duplicate(false);
	set_container(local_container);
	Ref<ASAttributeSet> attr_set = local_container->get_attribute_set();

	if (attr_set.is_valid()) {
		// Register a unique instance of the AttributeSet
		Ref<ASAttributeSet> local_set = attr_set->duplicate(true);
		local_container->set_attribute_set(local_set);
		add_attribute_set(local_set);

		// Grant abilities defined by the AttributeSet (already unlocked)
		TypedArray<ASAbility> unlocked = local_set->get_unlocked_abilities();
		for (int i = 0; i < unlocked.size(); i++) {
			Ref<ASAbility> ability = unlocked[i];
			if (ability.is_valid()) {
				unlock_ability_by_resource(ability);
			}
		}
	}

	// 2. Apply Effects
	TypedArray<ASEffect> effects = local_container->get_effects();
	for (int i = 0; i < effects.size(); i++) {
		Ref<ASEffect> effect = effects[i];
		if (effect.is_valid()) {
			Ref<ASEffectSpec> spec = make_outgoing_spec(effect, (float)p_level);
			apply_effect_spec_to_self(spec);
		}
	}

	// 3. Register Cues
	TypedArray<ASCue> cues = local_container->get_cues();
	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid()) {
			register_cue_resource(cue);
		}
	}

	// Force update of current values after setting base values
	_update_attribute_current_values();
}

// --- Ability Unlocking API (Permanent for the Actor instance) ---

void ASComponent::unlock_ability_by_resource(const Ref<ASAbility> &p_ability) {
	ERR_FAIL_COND(p_ability.is_null());
	if (attribute_sets.size() > 0) {
		Ref<ASAttributeSet> primary_set = attribute_sets[0];
		if (primary_set.is_valid()) {
			TypedArray<ASAbility> unlocked = primary_set->get_unlocked_abilities();
			if (unlocked.find(p_ability) == -1) {
				unlocked.push_back(p_ability);
				primary_set->set_unlocked_abilities(unlocked);
			}
		}
	}

	for (int i = 0; i < unlocked_abilities.size(); i++) {
		if (unlocked_abilities[i]->get_ability() == p_ability) {
			return;
		}
	}

	Ref<ASAbilitySpec> spec;
	spec.instantiate();
	spec->init(p_ability);
	spec->set_owner(this);
	unlocked_abilities.push_back(spec);
}

void ASComponent::unlock_ability_by_tag(const StringName &p_tag) {
	if (container.is_valid()) {
		TypedArray<ASAbility> catalog = container->get_abilities();
		for (int i = 0; i < catalog.size(); i++) {
			Ref<ASAbility> ability = catalog[i];
			if (ability.is_valid() && ability->get_ability_tag() == p_tag) {
				unlock_ability_by_resource(ability);
				return;
			}
		}
	}
	ERR_PRINT(vformat("ASComponent: unlock_ability_by_tag failed - '%s' not found in catalog.", p_tag));
}

void ASComponent::lock_ability_by_resource(const Ref<ASAbility> &p_ability) {
	ERR_FAIL_COND(p_ability.is_null());
	if (attribute_sets.size() > 0) {
		Ref<ASAttributeSet> primary_set = attribute_sets[0];
		if (primary_set.is_valid()) {
			TypedArray<ASAbility> unlocked = primary_set->get_unlocked_abilities();
			int idx = unlocked.find(p_ability);
			if (idx != -1) {
				unlocked.remove_at(idx);
				primary_set->set_unlocked_abilities(unlocked);
			}
		}
	}

	for (int i = 0; i < unlocked_abilities.size(); i++) {
		if (unlocked_abilities[i]->get_ability() == p_ability) {
			unlocked_abilities.remove_at(i);
			break;
		}
	}
	cancel_ability_by_resource(p_ability);
}

void ASComponent::lock_ability_by_tag(const StringName &p_tag) {
	if (attribute_sets.size() > 0) {
		Ref<ASAttributeSet> primary_set = attribute_sets[0];
		if (primary_set.is_valid()) {
			TypedArray<ASAbility> unlocked = primary_set->get_unlocked_abilities();
			for (int i = 0; i < unlocked.size(); i++) {
				Ref<ASAbility> ability = unlocked[i];
				if (ability.is_valid() && ability->get_ability_tag() == p_tag) {
					unlocked.remove_at(i);
					primary_set->set_unlocked_abilities(unlocked);
					break;
				}
			}
		}
	}

	for (int i = 0; i < unlocked_abilities.size(); i++) {
		if (unlocked_abilities[i]->get_ability()->get_ability_tag() == p_tag) {
			unlocked_abilities.remove_at(i);
			break;
		}
	}
	cancel_ability_by_tag(p_tag);
}

bool ASComponent::is_ability_unlocked(const StringName &p_tag) const {
	for (int i = 0; i < unlocked_abilities.size(); i++) {
		if (unlocked_abilities[i]->get_ability()->get_ability_tag() == p_tag) {
			return true;
		}
	}
	return false;
}

// --- Ability Activation API (By Tag) ---

bool ASComponent::is_ability_active(const StringName &p_tag) const {
	for (int i = 0; i < active_abilities.size(); i++) {
		if (active_abilities[i]->get_ability()->get_ability_tag() == p_tag) {
			return true;
		}
	}
	return false;
}

bool ASComponent::can_activate_ability_by_tag(const StringName &p_tag) {
	if (!is_ability_unlocked(p_tag)) {
		return false;
	}

	if (container.is_valid()) {
		TypedArray<ASAbility> catalog = container->get_abilities();
		for (int i = 0; i < catalog.size(); i++) {
			Ref<ASAbility> ability = catalog[i];
			if (ability.is_valid() && ability->get_ability_tag() == p_tag) {
				Ref<ASAbilitySpec> temp_spec;
				temp_spec.instantiate();
				temp_spec->init(ability);
				temp_spec->set_owner(this);
				return ability->can_activate_ability(this, temp_spec);
			}
		}
	}
	return false;
}

bool ASComponent::try_activate_ability_by_tag(const StringName &p_tag) {
	bool authority = is_multiplayer_authority();
	if (!authority) {
		_is_predicting = true;
	}

	bool success = false;
	if (container.is_valid()) {
		TypedArray<ASAbility> catalog = container->get_abilities();
		for (int i = 0; i < catalog.size(); i++) {
			Ref<ASAbility> ability = catalog[i];
			if (ability.is_valid() && ability->get_ability_tag() == p_tag) {
				if (is_ability_unlocked(p_tag)) {
					Ref<ASAbilitySpec> spec;
					spec.instantiate();
					spec->init(ability);
					spec->set_owner(this);

					if (ability->can_activate_ability(this, spec)) {
						// Cancel abilities by tag as requested by the activating ability
						TypedArray<StringName> cancel_tags = ability->get_activation_cancel_tags();
						for (int j = 0; j < cancel_tags.size(); j++) {
							cancel_ability_by_tag(cancel_tags[j]);
						}

						spec->set_is_active(true);
						ability->activate_ability(this, spec);
						if (ability->get_duration_policy() != ASAbility::POLICY_INSTANT) {
							active_abilities.push_back(spec);
						} else {
							ability->end_ability(this, spec);
							spec->set_is_active(false);
							emit_signal("ability_ended", spec, false);
						}
						emit_signal("ability_activated", spec);
						success = true;
						break;
					}
				}
			}
		}
	}

	if (!authority) {
		_is_predicting = false;
	}

	if (!success) {
		emit_signal("ability_failed", p_tag, "Ability locked or not in catalog");
	}
	return success;
}

void ASComponent::cancel_ability_by_tag(const StringName &p_tag) {
	for (int i = active_abilities.size() - 1; i >= 0; i--) {
		Ref<ASAbilitySpec> spec = active_abilities[i];
		if (spec->get_ability()->get_ability_tag() == p_tag) {
			spec->get_ability()->end_ability(this, spec);
			spec->set_is_active(false);
			active_abilities.remove_at(i);
			emit_signal("ability_ended", spec, true);
		}
	}
}

// --- Ability Activation API (By Resource) ---

bool ASComponent::can_activate_ability_by_resource(const Ref<ASAbility> &p_ability) {
	ERR_FAIL_COND_V(p_ability.is_null(), false);
	if (!is_ability_unlocked(p_ability->get_ability_tag())) {
		return false;
	}
	Ref<ASAbilitySpec> temp_spec;
	temp_spec.instantiate();
	temp_spec->init(p_ability);
	temp_spec->set_owner(this);
	return p_ability->can_activate_ability(this, temp_spec);
}

bool ASComponent::try_activate_ability_by_resource(const Ref<ASAbility> &p_ability) {
	ERR_FAIL_COND_V(p_ability.is_null(), false);

	bool authority = is_multiplayer_authority();
	if (!authority) {
		_is_predicting = true;
	}

	bool success = false;
	if (is_ability_unlocked(p_ability->get_ability_tag())) {
		Ref<ASAbilitySpec> spec;
		spec.instantiate();
		spec->init(p_ability);
		spec->set_owner(this);

		if (p_ability->can_activate_ability(this, spec)) {
			// Cancel abilities by tag as requested by the activating ability
			TypedArray<StringName> cancel_tags = p_ability->get_activation_cancel_tags();
			for (int j = 0; j < cancel_tags.size(); j++) {
				cancel_ability_by_tag(cancel_tags[j]);
			}

			spec->set_is_active(true);
			p_ability->activate_ability(this, spec);

			if (p_ability->get_duration_policy() != ASAbility::POLICY_INSTANT) {
				active_abilities.push_back(spec);
			} else {
				p_ability->end_ability(this, spec);
				spec->set_is_active(false);
				emit_signal("ability_ended", spec, false);
			}
			emit_signal("ability_activated", spec);
			success = true;
		}
	}

	if (!authority) {
		_is_predicting = false;
	}
	return success;
}

void ASComponent::cancel_ability_by_resource(const Ref<ASAbility> &p_ability) {
	ERR_FAIL_COND(p_ability.is_null());
	for (int i = active_abilities.size() - 1; i >= 0; i--) {
		Ref<ASAbilitySpec> spec = active_abilities[i];
		if (spec->get_ability() == p_ability) {
			p_ability->end_ability(this, spec);
			spec->set_is_active(false);
			active_abilities.remove_at(i);
			emit_signal("ability_ended", spec, true);
		}
	}
}

// --- Effect Activation API ---

bool ASComponent::can_activate_effect_by_resource(const Ref<ASEffect> &p_effect) {
	ERR_FAIL_COND_V(p_effect.is_null(), false);

	if (owned_tags.is_null()) {
		owned_tags.instantiate();
	}

	if (!owned_tags->has_all_tags(p_effect->get_activation_required_all_tags())) {
		emit_signal("effect_failed", p_effect->get_effect_tag(), "Required tags missing");
		return false;
	}

	if (!p_effect->get_activation_required_any_tags().is_empty() &&
			!owned_tags->has_any_tags(p_effect->get_activation_required_any_tags())) {
		emit_signal("effect_failed", p_effect->get_effect_tag(), "Required any tags missing");
		return false;
	}

	if (owned_tags->has_any_tags(p_effect->get_activation_blocked_any_tags())) {
		emit_signal("effect_failed", p_effect->get_effect_tag(), "Blocked by tags");
		return false;
	}

	if (!p_effect->get_activation_blocked_all_tags().is_empty() &&
			owned_tags->has_all_tags(p_effect->get_activation_blocked_all_tags())) {
		emit_signal("effect_failed", p_effect->get_effect_tag(), "Blocked by all tags");
		return false;
	}

	for (int i = 0; i < p_effect->get_requirement_count(); i++) {
		StringName attr = p_effect->get_requirement_attribute(i);
		float amount = p_effect->get_requirement_amount(i);
		if (get_attribute_value_by_tag(attr) < amount) {
			emit_signal("effect_failed", p_effect->get_effect_tag(), "Attribute requirement not met");
			return false;
		}
	}

	return true;
}

bool ASComponent::can_activate_effect_by_tag(const StringName &p_tag) {
	if (container.is_valid()) {
		TypedArray<ASEffect> effects = container->get_effects();
		for (int i = 0; i < effects.size(); i++) {
			Ref<ASEffect> effect = effects[i];
			if (effect.is_valid() && effect->get_effect_tag() == p_tag) {
				return can_activate_effect_by_resource(effect);
			}
		}
	}
	emit_signal("effect_failed", p_tag, "Effect tag not found in container catalog");
	return false;
}

bool ASComponent::try_activate_effect_by_resource(const Ref<ASEffect> &p_effect, float p_level, Object *p_target_node) {
	if (can_activate_effect_by_resource(p_effect)) {
		apply_effect_by_resource(p_effect, p_level, p_target_node);
		return true;
	}
	return false;
}

bool ASComponent::try_activate_effect_by_tag(const StringName &p_tag, float p_level, Object *p_target_node) {
	if (can_activate_effect_by_tag(p_tag)) {
		apply_effect_by_tag(p_tag, p_level, p_target_node);
		return true;
	}
	return false;
}

void ASComponent::cancel_effect_by_tag(const StringName &p_tag) {
	remove_effect_by_tag(p_tag);
}

void ASComponent::cancel_effect_by_resource(const Ref<ASEffect> &p_effect) {
	remove_effect_by_resource(p_effect);
}

// --- Effect Execution API (Low level) ---

void ASComponent::apply_effect_by_tag(const StringName &p_tag, float p_level, Object *p_target_node) {
	Ref<ASEffect> effect = find_effect_by_tag(p_tag);
	if (effect.is_valid()) {
		apply_effect_by_resource(effect, p_level, p_target_node);
	} else {
		ERR_PRINT(vformat("ASComponent: apply_effect_by_tag failed - Effect tag '%s' not found in container or unlocked abilities.", p_tag));
	}
}

void ASComponent::apply_effect_by_resource(const Ref<ASEffect> &p_effect, float p_level, Object *p_target_node) {
	Ref<ASEffectSpec> spec = make_outgoing_spec(p_effect, p_level, p_target_node);
	apply_effect_spec_to_self(spec);
}

void ASComponent::apply_package(const Ref<ASPackage> &p_package, float p_level, ASComponent *p_source_component) {
	ERR_FAIL_COND(p_package.is_null());

	// 1. Deliver effect resources
	TypedArray<ASEffect> effects = p_package->get_effects();
	for (int i = 0; i < effects.size(); i++) {
		Ref<ASEffect> effect = effects[i];
		if (effect.is_valid()) {
			Ref<ASEffectSpec> spec;
			if (p_source_component) {
				spec = p_source_component->make_outgoing_spec(effect, p_level);
			} else {
				spec.instantiate();
				spec->init(effect, p_level);
			}
			apply_effect_spec_to_self(spec);
		}
	}

	// 2. Deliver effect tags
	TypedArray<StringName> effect_tags = p_package->get_effect_tags();
	for (int i = 0; i < effect_tags.size(); i++) {
		StringName tag = effect_tags[i];
		if (p_source_component) {
			Ref<ASContainer> src_container = p_source_component->get_container();
			if (src_container.is_valid()) {
				TypedArray<ASEffect> source_effects = src_container->get_effects();
				for (int j = 0; j < source_effects.size(); j++) {
					Ref<ASEffect> e = source_effects[j];
					if (e.is_valid() && e->get_effect_tag() == tag) {
						Ref<ASEffectSpec> spec = p_source_component->make_outgoing_spec(e, p_level);
						apply_effect_spec_to_self(spec);
						break;
					}
				}
			}
		} else {
			apply_effect_by_tag(tag, p_level);
		}
	}

	// 3. Deliver cue resources
	TypedArray<ASCue> cues = p_package->get_cues();
	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid()) {
			Dictionary cue_data;
			cue_data["level"] = p_level;
			try_activate_cue_by_resource(cue, cue_data, nullptr);
		}
	}

	// 4. Deliver cue tags
	TypedArray<StringName> cue_tags = p_package->get_cue_tags();
	for (int i = 0; i < cue_tags.size(); i++) {
		StringName tag = cue_tags[i];
		Dictionary cue_tag_data;
		cue_tag_data["level"] = p_level;
		try_activate_cue_by_tag(tag, cue_tag_data, nullptr);
	}
}

Ref<ASEffect> ASComponent::find_effect_by_tag(const StringName &p_tag) const {
	if (container.is_valid()) {
		TypedArray<ASEffect> catalog = container->get_effects();
		for (int i = 0; i < catalog.size(); i++) {
			Ref<ASEffect> e = catalog[i];
			if (e.is_valid() && e->get_effect_tag() == p_tag) {
				return e;
			}
		}
	}

	for (int i = 0; i < unlocked_abilities.size(); i++) {
		if (unlocked_abilities[i].is_null()) {
			continue;
		}
		Ref<ASAbility> ability = unlocked_abilities[i]->get_ability();
		if (ability.is_valid()) {
			TypedArray<ASEffect> ability_effects = ability->get_effects();
			for (int k = 0; k < ability_effects.size(); k++) {
				Ref<ASEffect> e = ability_effects[k];
				if (e.is_valid() && e->get_effect_tag() == p_tag) {
					return e;
				}
			}
		}
	}
	return Ref<ASEffect>();
}

Ref<ASEffectSpec> ASComponent::make_outgoing_spec(Ref<ASEffect> p_effect, float p_level, Object *p_target_node) {
	ERR_FAIL_COND_V(p_effect.is_null(), Ref<ASEffectSpec>());

	// Validation: Outgoing effects must be part of the archetype contract.
	if (container.is_valid()) {
		if (!container->has_effect(p_effect)) {
			bool found = false;
			for (int i = 0; i < unlocked_abilities.size(); i++) {
				Ref<ASAbility> ability = unlocked_abilities[i]->get_ability();
				if (ability.is_valid()) {
					TypedArray<ASEffect> ability_effects = ability->get_effects();
					for (int k = 0; k < ability_effects.size(); k++) {
						if (ability_effects[k] == p_effect) {
							found = true;
							break;
						}
					}
					if (found)
						break;

					TypedArray<Dictionary> costs = ability->get_costs();
					for (int j = 0; j < costs.size(); j++) {
						Dictionary cost = costs[j];
						if (cost.has("effect")) {
							Ref<ASEffect> cost_effect = cost["effect"];
							if (cost_effect == p_effect) {
								found = true;
								break;
							}
						}
					}
					if (found)
						break;
				}
			}

			if (!found) {
				String path = p_effect->get_path();
				if (path.is_empty())
					path = p_effect->get_class();
				ERR_PRINT(vformat("ASComponent Error: Effect '%s' is NOT registered in the source's AbilityContainer.", path));
			}
		}
	}

	Ref<ASEffectSpec> spec;
	spec.instantiate();
	spec->init(p_effect, p_level);
	spec->set_source_component(this);
	spec->set_target_node(p_target_node);
	return spec;
}

void ASComponent::apply_effect_spec_to_target(Ref<ASEffectSpec> p_spec, ASComponent *p_target) {
	ERR_FAIL_COND(p_spec.is_null());
	ERR_FAIL_COND(p_target == nullptr);
	p_target->apply_effect_spec_to_self(p_spec);
}

void ASComponent::apply_effect_spec_to_self(Ref<ASEffectSpec> p_spec) {
	ERR_FAIL_COND(p_spec.is_null());
	Ref<ASEffect> effect = p_spec->get_effect();
	ERR_FAIL_COND(effect.is_null());

	p_spec->set_target_component(this);

	if (owned_tags.is_null()) {
		owned_tags.instantiate();
	}

	// --- Target Validation ---
	if (!owned_tags->has_all_tags(effect->get_activation_required_all_tags())) {
		emit_signal("effect_failed", effect->get_effect_tag(), "Target missing required tags");
		return;
	}

	if (!effect->get_activation_required_any_tags().is_empty() &&
			!owned_tags->has_any_tags(effect->get_activation_required_any_tags())) {
		emit_signal("effect_failed", effect->get_effect_tag(), "Target missing required any tags");
		return;
	}

	if (owned_tags->has_any_tags(effect->get_activation_blocked_any_tags())) {
		emit_signal("effect_failed", effect->get_effect_tag(), "Target has blocking tags");
		return;
	}

	if (!effect->get_activation_blocked_all_tags().is_empty() &&
			owned_tags->has_all_tags(effect->get_activation_blocked_all_tags())) {
		emit_signal("effect_failed", effect->get_effect_tag(), "Target blocked by all tags");
		return;
	}
	// -------------------------

	if (effect->get_duration_policy() == ASEffect::POLICY_DURATION) {
		float duration = calculate_effect_duration(p_spec);
		p_spec->set_duration_remaining(duration);
		p_spec->set_total_duration(duration);
	}

	if (effect->get_duration_policy() == ASEffect::POLICY_INSTANT) {
		for (int i = 0; i < effect->get_modifier_count(); i++) {
			StringName attr = effect->get_modifier_attribute(i);
			ASEffect::ModifierOp op = effect->get_modifier_operation(i);
			float mag = calculate_modifier_magnitude(p_spec, i);
			float current = get_attribute_value_by_tag(attr);
			float next = current;
			switch (op) {
				case ASEffect::OP_ADD:
					next += mag;
					break;
				case ASEffect::OP_MULTIPLY:
					next *= mag;
					break;
				case ASEffect::OP_DIVIDE:
					if (mag != 0) {
						next /= mag;
					}
					break;
				case ASEffect::OP_OVERRIDE:
					next = mag;
					break;
			}
			set_attribute_base_value_by_tag(attr, next);
		}
		_update_attribute_current_values();
	} else {
		ASEffect::StackingPolicy stacking = effect->get_stacking_policy();
		if (stacking != ASEffect::STACK_NEW_INSTANCE) {
			for (int i = 0; i < active_effects.size(); i++) {
				if (active_effects[i]->get_effect() == effect) {
					Ref<ASEffectSpec> existing = active_effects[i];
					switch (stacking) {
						case ASEffect::STACK_OVERRIDE:
							existing->set_duration_remaining(p_spec->get_total_duration());
							if (effect->get_period() > 0.0f) {
								existing->set_period_timer(effect->get_period());
							}
							break;
						case ASEffect::STACK_INTENSITY:
							existing->set_stack_count(existing->get_stack_count() + 1);
							break;
						case ASEffect::STACK_DURATION:
							existing->set_duration_remaining(existing->get_duration_remaining() + p_spec->get_total_duration());
							break;
						default:
							break;
					}
					_update_attribute_current_values();
					goto finish_cues;
				}
			}
		}

		active_effects.push_back(p_spec);
		if (effect->get_period() > 0.0f) {
			p_spec->set_period_timer(effect->get_period());

			if (effect->get_execute_periodic_tick_on_application()) {
				// Execute first tick immediately
				for (int j = 0; j < effect->get_modifier_count(); j++) {
					StringName attr = effect->get_modifier_attribute(j);
					ASEffect::ModifierOp op = effect->get_modifier_operation(j);
					float mag = calculate_modifier_magnitude(p_spec, j);
					float current_base = get_attribute_base_value_by_tag(attr);
					float next_base = current_base;
					switch (op) {
						case ASEffect::OP_ADD:
							next_base += mag;
							break;
						case ASEffect::OP_MULTIPLY:
							next_base *= mag;
							break;
						case ASEffect::OP_DIVIDE:
							if (mag != 0) {
								next_base /= mag;
							}
							break;
						case ASEffect::OP_OVERRIDE:
							next_base = mag;
							break;
					}
					// Use set_attribute_base_value_by_tag which handles _update_attribute_current_values
					set_attribute_base_value_by_tag(attr, next_base);
				}
			}
		}
		TypedArray<StringName> granted = effect->get_granted_tags();
		for (int i = 0; i < granted.size(); i++) {
			add_tag(granted[i]);
		}
		_update_attribute_current_values();
	}

finish_cues: {
	float rep_mag = 0.0f;
	for (int j = 0; j < effect->get_modifier_count(); j++) {
		rep_mag += calculate_modifier_magnitude(p_spec, j);
	}

	StringName effect_tag = effect->get_effect_tag();
	if (effect_tag != StringName()) {
		Ref<ASCue> cue = get_cue_resource(effect_tag);
		if (cue.is_null() && p_spec->get_source_component()) {
			cue = p_spec->get_source_component()->get_cue_resource(effect_tag);
		}
		if (cue.is_valid() && cue->get_event_type() != ASCue::ON_REMOVE) {
			Ref<ASCueSpec> cue_spec;
			cue_spec.instantiate();
			cue_spec->init_from_effect(cue, p_spec->get_source_component(), this, p_spec, rep_mag);
			_execute_cue_with_spec(effect_tag, cue_spec);
		} else if (cue.is_null()) {
			emit_signal("tag_event_received", effect_tag, Dictionary());
		}
	}

	TypedArray<ASCue> cues = effect->get_cues();
	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid() && cue->get_event_type() != ASCue::ON_REMOVE) {
			Ref<ASCueSpec> cue_spec;
			cue_spec.instantiate();
			cue_spec->init_from_effect(cue, p_spec->get_source_component(), this, p_spec, rep_mag);
			cue->execute(cue_spec);
		}
	}
}
	emit_signal("effect_applied", p_spec);
}

// --- Cue Activation API ---

bool ASComponent::can_activate_cue_by_resource(const Ref<ASCue> &p_cue) {
	ERR_FAIL_COND_V(p_cue.is_null(), false);
	if (!owned_tags->has_all_tags(p_cue->get_activation_required_all_tags())) {
		emit_signal("cue_failed", p_cue->get_cue_tag(), "Required tags missing");
		return false;
	}

	if (!p_cue->get_activation_required_any_tags().is_empty() &&
			!owned_tags->has_any_tags(p_cue->get_activation_required_any_tags())) {
		emit_signal("cue_failed", p_cue->get_cue_tag(), "Required any tags missing");
		return false;
	}

	if (owned_tags->has_any_tags(p_cue->get_activation_blocked_any_tags())) {
		emit_signal("cue_failed", p_cue->get_cue_tag(), "Blocked by tags");
		return false;
	}

	if (!p_cue->get_activation_blocked_all_tags().is_empty() &&
			owned_tags->has_all_tags(p_cue->get_activation_blocked_all_tags())) {
		emit_signal("cue_failed", p_cue->get_cue_tag(), "Blocked by all tags");
		return false;
	}

	return true;
}

bool ASComponent::can_activate_cue_by_tag(const StringName &p_tag) {
	Ref<ASCue> cue = get_cue_resource(p_tag);
	if (cue.is_valid()) {
		return can_activate_cue_by_resource(cue);
	}
	emit_signal("cue_failed", p_tag, "Cue tag not found in registered cues");
	return false;
}

bool ASComponent::try_activate_cue_by_resource(const Ref<ASCue> &p_cue, const Dictionary &p_data, Object *p_target_node) {
	if (can_activate_cue_by_resource(p_cue)) {
		Ref<ASCueSpec> spec;
		spec.instantiate();
		spec->init_manual(p_cue, this, p_data);
		spec->set_target_node(p_target_node);
		_execute_cue_with_spec(p_cue->get_cue_tag(), spec);
		return true;
	}
	return false;
}

bool ASComponent::try_activate_cue_by_tag(const StringName &p_tag, const Dictionary &p_data, Object *p_target_node) {
	if (can_activate_cue_by_tag(p_tag)) {
		Ref<ASCue> cue = get_cue_resource(p_tag);
		return try_activate_cue_by_resource(cue, p_data, p_target_node);
	}
	return false;
}

void ASComponent::cancel_cue_by_tag(const StringName &p_tag) {
	Ref<ASCue> cue = get_cue_resource(p_tag);
	if (cue.is_valid()) {
		cancel_cue_by_resource(cue);
	}
}

void ASComponent::cancel_cue_by_resource(const Ref<ASCue> &p_cue) {
	ERR_FAIL_COND(p_cue.is_null());
	Ref<ASCueSpec> spec;
	spec.instantiate();
	spec->init_manual(p_cue, this, Dictionary());
	// Manually trigger REMOVE event
	if (p_cue->get_event_type() == ASCue::ON_ACTIVE || p_cue->get_event_type() == ASCue::ON_REMOVE) {
		p_cue->set_event_type(ASCue::ON_REMOVE);
		p_cue->execute(spec);
	}
}

void ASComponent::_execute_cue_with_spec(const StringName &p_tag, Ref<ASCueSpec> p_spec) {
	emit_signal("tag_event_received", p_tag, p_spec->get_extra_data());
	Ref<ASCue> cue = p_spec->get_cue();
	if (cue.is_valid()) {
		cue->execute(p_spec);
	}
}

void ASComponent::register_cue_resource(Ref<ASCue> p_cue) {
	ERR_FAIL_COND(p_cue.is_null());
	StringName tag = p_cue->get_cue_tag();
	if (tag != StringName()) {
		for (int i = 0; i < registered_cues.size(); i++) {
			if (registered_cues[i]->get_cue_tag() == tag) {
				registered_cues.write[i] = p_cue;
				return;
			}
		}
		registered_cues.push_back(p_cue);
	}
}

void ASComponent::unregister_cue_resource(const StringName &p_tag) {
	for (int i = 0; i < registered_cues.size(); i++) {
		if (registered_cues[i]->get_cue_tag() == p_tag) {
			registered_cues.remove_at(i);
			return;
		}
	}
}

Ref<ASCue> ASComponent::get_cue_resource(const StringName &p_tag) const {
	for (int i = 0; i < registered_cues.size(); i++) {
		if (registered_cues[i]->get_cue_tag() == p_tag) {
			return registered_cues[i];
		}
	}
	return Ref<ASCue>();
}

// --- Attribute API ---

float ASComponent::get_attribute_value_by_tag(const StringName &p_tag) const {
	for (int i = 0; i < attribute_sets.size(); i++) {
		if (attribute_sets[i].is_valid() && attribute_sets[i]->has_attribute(p_tag)) {
			return attribute_sets[i]->get_attribute_current_value(p_tag);
		}
	}
	return 0.0f;
}

float ASComponent::get_attribute_base_value_by_tag(const StringName &p_tag) const {
	for (int i = 0; i < attribute_sets.size(); i++) {
		if (attribute_sets[i].is_valid() && attribute_sets[i]->has_attribute(p_tag)) {
			return attribute_sets[i]->get_attribute_base_value(p_tag);
		}
	}
	return 0.0f;
}

float ASComponent::get_attribute_value_by_resource(const Ref<ASAttribute> &p_attribute) const {
	ERR_FAIL_COND_V(p_attribute.is_null(), 0.0f);
	return get_attribute_value_by_tag(p_attribute->get_attribute_name());
}

float ASComponent::get_attribute_base_value_by_resource(const Ref<ASAttribute> &p_attribute) const {
	ERR_FAIL_COND_V(p_attribute.is_null(), 0.0f);
	return get_attribute_base_value_by_tag(p_attribute->get_attribute_name());
}

void ASComponent::set_attribute_base_value_by_resource(const Ref<ASAttribute> &p_attribute, float p_value) {
	ERR_FAIL_COND(p_attribute.is_null());
	set_attribute_base_value_by_tag(p_attribute->get_attribute_name(), p_value);
}

bool ASComponent::has_attribute_by_tag(const StringName &p_tag) const {
	for (int i = 0; i < attribute_sets.size(); i++) {
		if (attribute_sets[i]->has_attribute(p_tag)) {
			return true;
		}
	}
	return false;
}

bool ASComponent::has_attribute_by_resource(const Ref<ASAttribute> &p_attribute) const {
	ERR_FAIL_COND_V(p_attribute.is_null(), false);
	return has_attribute_by_tag(p_attribute->get_attribute_name());
}

void ASComponent::_handle_ability_triggers(const StringName &p_tag, ASAbility::TriggerType p_type) {
	// Re-entrancy and Rollback guard
	if (_handling_triggers || _is_rolling_back) {
		return;
	}
	_handling_triggers = true;

	for (int i = 0; i < unlocked_abilities.size(); i++) {
		if (unlocked_abilities[i].is_null()) {
			continue;
		}
		Ref<ASAbility> ability = unlocked_abilities[i]->get_ability();
		if (ability.is_null()) {
			continue;
		}

		TypedArray<Dictionary> trigger_list = ability->get_triggers();
		for (int j = 0; j < trigger_list.size(); j++) {
			Dictionary d = trigger_list[j];
			if (d.has("tag") && (StringName)d["tag"] == p_tag && d.has("type") && (int)d["type"] == (int)p_type) {
				try_activate_ability_by_resource(ability);
				break;
			}
		}
	}

	_handling_triggers = false;
}

// --- Logic & Calculations ---

float ASComponent::calculate_modifier_magnitude(const Ref<ASEffectSpec> &p_spec, int p_modifier_idx) const {
	ERR_FAIL_COND_V(p_spec.is_null(), 0.0f);
	Ref<ASEffect> effect = p_spec->get_effect();
	ERR_FAIL_COND_V(effect.is_null(), 0.0f);
	if (effect->is_modifier_custom(p_modifier_idx)) {
		float result = 0.0f;
		if (const_cast<ASComponent *>(this)->GDVIRTUAL_CALL(_on_calculate_custom_magnitude, p_spec, p_modifier_idx, result)) {
			return result;
		}
	}
	return effect->get_modifier_magnitude(p_modifier_idx) * p_spec->get_level();
}

float ASComponent::calculate_effect_duration(const Ref<ASEffectSpec> &p_spec) const {
	ERR_FAIL_COND_V(p_spec.is_null(), 0.0f);
	Ref<ASEffect> effect = p_spec->get_effect();
	ERR_FAIL_COND_V(effect.is_null(), 0.0f);
	if (effect->get_use_custom_duration()) {
		float result = 0.0f;
		if (const_cast<ASComponent *>(this)->GDVIRTUAL_CALL(_on_calculate_custom_magnitude, p_spec, -1, result)) {
			return result;
		}
	}
	return effect->get_duration_magnitude();
}

void ASComponent::add_attribute_set(Ref<ASAttributeSet> p_set) {
	ERR_FAIL_COND(p_set.is_null());
	attribute_sets.push_back(p_set);
	if (!p_set->is_connected("attribute_changed", callable_mp(this, &ASComponent::_on_attribute_set_attribute_changed))) {
		p_set->connect("attribute_changed", callable_mp(this, &ASComponent::_on_attribute_set_attribute_changed));
	}
}

TypedArray<ASAttributeSet> ASComponent::get_attribute_sets() const {
	TypedArray<ASAttributeSet> res;
	for (int i = 0; i < attribute_sets.size(); i++) {
		res.push_back(attribute_sets[i]);
	}
	return res;
}

void ASComponent::_on_attribute_set_attribute_changed(const StringName &p_name, float p_old_val, float p_new_val) {
	if (_is_rolling_back)
		return;
	emit_signal("attribute_changed", p_name, p_old_val, p_new_val);
}

void ASComponent::_update_attribute_current_values() {
	if (_updating_attributes) {
		return;
	}
	_updating_attributes = true;

	for (int k = 0; k < attribute_sets.size(); k++) {
		attribute_sets[k]->reset_current_values();
	}

	// Safe copy to prevent issues if calculate_modifier_magnitude triggers re-entrant changes
	Vector<Ref<ASEffectSpec>> effects_to_process = active_effects;

	const ASEffect::ModifierOp OP_ORDER[] = { ASEffect::OP_ADD, ASEffect::OP_MULTIPLY, ASEffect::OP_DIVIDE, ASEffect::OP_OVERRIDE };
	for (int pass = 0; pass < 4; pass++) {
		ASEffect::ModifierOp current_op = OP_ORDER[pass];
		for (int i = 0; i < effects_to_process.size(); i++) {
			Ref<ASEffectSpec> spec = effects_to_process[i];
			if (spec.is_null()) {
				continue;
			}
			Ref<ASEffect> effect = spec->get_effect();
			const float stack_multiplier = (effect->get_stacking_policy() == ASEffect::STACK_INTENSITY) ? (float)spec->get_stack_count() : 1.0f;
			for (int j = 0; j < effect->get_modifier_count(); j++) {
				if (effect->get_modifier_operation(j) != current_op || effect->get_period() > 0.0f) {
					continue;
				}
				StringName attr_name = effect->get_modifier_attribute(j);
				for (int k = 0; k < attribute_sets.size(); k++) {
					if (!attribute_sets[k]->has_attribute(attr_name)) {
						continue;
					}
					float current = attribute_sets[k]->get_attribute_current_value(attr_name);
					float mag = calculate_modifier_magnitude(spec, j) * stack_multiplier;
					float next = current;
					switch (current_op) {
						case ASEffect::OP_ADD:
							next += mag;
							break;
						case ASEffect::OP_MULTIPLY:
							next *= mag;
							break;
						case ASEffect::OP_DIVIDE:
							if (mag != 0) {
								next /= mag;
							}
							break;
						case ASEffect::OP_OVERRIDE:
							next = mag;
							break;
					}
					attribute_sets[k]->set_attribute_current_value(attr_name, next);
					break;
				}
			}
		}
	}
	_updating_attributes = false;
}

void ASComponent::play_montage(const StringName &p_name, Node *p_target) {
	if (!p_target) {
		p_target = animation_player_node;
	}
	if (!p_target) {
		return;
	}
	if (AnimationPlayer *ap = Object::cast_to<AnimationPlayer>(p_target)) {
		ap->play(p_name);
	} else if (AnimatedSprite2D *as2 = Object::cast_to<AnimatedSprite2D>(p_target)) {
		as2->play(p_name);
	} else if (AnimatedSprite3D *as3 = Object::cast_to<AnimatedSprite3D>(p_target)) {
		as3->play(p_name);
	}
}

bool ASComponent::is_montage_playing(const StringName &p_name, Node *p_target) const {
	if (!p_target) {
		p_target = animation_player_node;
	}
	if (!p_target) {
		return false;
	}
	if (AnimationPlayer *ap = Object::cast_to<AnimationPlayer>(p_target)) {
		return ap->is_playing() && ap->get_current_animation() == p_name;
	} else if (AnimatedSprite2D *as2 = Object::cast_to<AnimatedSprite2D>(p_target)) {
		return as2->is_playing() && as2->get_animation() == p_name;
	} else if (AnimatedSprite3D *as3 = Object::cast_to<AnimatedSprite3D>(p_target)) {
		return as3->is_playing() && as3->get_animation() == p_name;
	}
	return false;
}

void ASComponent::play_sound(Ref<AudioStream> p_sound, Node *p_target) {
	if (p_sound.is_null()) {
		return;
	}
	if (!p_target) {
		p_target = audio_player_node;
	}
	if (!p_target) {
		return;
	}
	if (AudioStreamPlayer *ap = Object::cast_to<AudioStreamPlayer>(p_target)) {
		ap->set_stream(p_sound);
		ap->play();
	} else if (AudioStreamPlayer2D *ap2 = Object::cast_to<AudioStreamPlayer2D>(p_target)) {
		ap2->set_stream(p_sound);
		ap2->play();
	} else if (AudioStreamPlayer3D *ap3 = Object::cast_to<AudioStreamPlayer3D>(p_target)) {
		ap3->set_stream(p_sound);
		ap3->play();
	}
}

void ASComponent::register_node(const StringName &p_name, Node *p_node) {
	registered_nodes[p_name] = p_node;
}
void ASComponent::unregister_node(const StringName &p_name) {
	registered_nodes.erase(p_name);
}
Node *ASComponent::get_node_ptr(const StringName &p_name) const {
	return registered_nodes.has(p_name) ? registered_nodes[p_name] : nullptr;
}

void ASComponent::set_container(Ref<ASContainer> p_container) {
	container = p_container;
}
Ref<ASContainer> ASComponent::get_container() const {
	return container;
}

void ASComponent::set_snapshot_state(Ref<ASStateSnapshot> p_snapshot) {
	snapshot_state = p_snapshot;
}

Ref<ASStateSnapshot> ASComponent::get_snapshot_state() const {
	return snapshot_state;
}

float ASComponent::get_ability_duration_preview(const StringName &p_tag) const {
	if (container.is_valid()) {
		TypedArray<ASAbility> catalog = container->get_abilities();
		for (int i = 0; i < catalog.size(); i++) {
			Ref<ASAbility> ability = catalog[i];
			if (ability.is_valid() && ability->get_ability_tag() == p_tag) {
				return ability->calculate_ability_duration(const_cast<ASComponent *>(this));
			}
		}
	}
	return 0.0f;
}

float ASComponent::get_ability_cost_preview(const StringName &p_tag, const StringName &p_attr) const {
	if (container.is_valid()) {
		TypedArray<ASAbility> catalog = container->get_abilities();
		for (int i = 0; i < catalog.size(); i++) {
			Ref<ASAbility> ability = catalog[i];
			if (ability.is_valid() && ability->get_ability_tag() == p_tag) {
				return ability->get_cost_amount(p_attr);
			}
		}
	}
	return 0.0f;
}

float ASComponent::get_ability_cooldown_preview(const StringName &p_tag) const {
	if (container.is_valid()) {
		TypedArray<ASAbility> catalog = container->get_abilities();
		for (int i = 0; i < catalog.size(); i++) {
			Ref<ASAbility> ability = catalog[i];
			if (ability.is_valid() && ability->get_ability_tag() == p_tag) {
				return ability->get_cooldown_duration();
			}
		}
	}
	return 0.0f;
}

bool ASComponent::_is_server() const {
#ifdef ABILITY_SYSTEM_GDEXTENSION
	Ref<MultiplayerAPI> mp = get_multiplayer();
	if (mp.is_null())
		return true; // Standalone
	return mp->is_server();
#else
	return !get_tree() || get_tree()->get_multiplayer()->is_server();
#endif
}

bool ASComponent::_is_local_authority() const {
	return is_multiplayer_authority();
}

void ASComponent::capture_snapshot() {
	// 1. Capture to Light Cache (Always)
	ASStateCache s;
	s.tick = current_tick;

	// Attributes
	for (int i = 0; i < attribute_sets.size(); i++) {
		if (attribute_sets[i].is_null())
			continue;
		TypedArray<StringName> attrs = attribute_sets[i]->get_attribute_list();
		for (int j = 0; j < attrs.size(); j++) {
			StringName name = attrs[j];
			s.attributes[name] = attribute_sets[i]->get_attribute_base_value(name);
		}
	}

	// Tags
	TypedArray<StringName> all_tags = get_tags();
	for (int i = 0; i < all_tags.size(); i++) {
		s.tags.push_back(all_tags[i]);
	}

	// Effects
	for (int i = 0; i < active_effects.size(); i++) {
		Ref<ASEffectSpec> spec = active_effects[i];
		if (spec.is_null())
			continue;
		Ref<ASEffect> effect = spec->get_effect();
		if (effect.is_null())
			continue;

		ASEffectState es;
		es.tag = effect->get_effect_tag();
		es.remaining_time = spec->get_duration_remaining();
		es.period_timer = spec->get_period_timer();
		es.stack_count = spec->get_stack_count();
		es.level = spec->get_level();
		s.active_effects.push_back(es);
	}

	cache_buffer.push_back(s);
	if (cache_buffer.size() > 128) {
		cache_buffer.remove_at(0);
	}

	// 2. Capture to Snapshot Resource (If assigned - typically for Players)
	if (snapshot_state.is_valid()) {
		snapshot_state->set_tick(current_tick);
		snapshot_state->capture_from_component(this);
	}
}

void ASComponent::apply_snapshot(uint32_t p_tick) {
	_is_rolling_back = true;
	// 1. Try Light Cache first (High performance)
	for (int i = cache_buffer.size() - 1; i >= 0; i--) {
		if (cache_buffer[i].tick == p_tick) {
			const ASStateCache &s = cache_buffer[i];

			// Restore Attributes (No-update loop)
			for (const KeyValue<StringName, float> &E : s.attributes) {
				for (int j = 0; j < attribute_sets.size(); j++) {
					if (attribute_sets[j].is_valid() && attribute_sets[j]->has_attribute(E.key)) {
						attribute_sets[j]->set_attribute_base_value(E.key, E.value);
						break;
					}
				}
			}

			// Restore Tags
			if (owned_tags.is_valid()) {
				owned_tags->clear();
				for (int j = 0; j < s.tags.size(); j++) {
					owned_tags->add_tag(s.tags[j]);
				}
			}

			// Restore Effects (Silent clear and re-instantiate)
			active_effects.clear();
			for (int j = 0; j < s.active_effects.size(); j++) {
				const ASEffectState &es = s.active_effects[j];
				Ref<ASEffect> e = find_effect_by_tag(es.tag);
				if (e.is_valid()) {
					Ref<ASEffectSpec> spec;
					spec.instantiate();
					spec->init(e, es.level);
					spec->set_duration_remaining(es.remaining_time);
					spec->set_period_timer(es.period_timer);
					spec->set_stack_count(es.stack_count);
					spec->set_target_component(this);
					active_effects.push_back(spec);
				}
			}

			current_tick = p_tick;
			_is_rolling_back = false;
			_update_attribute_current_values();
			return;
		}
	}

	// 2. Try Snapshot Resource fallback
	if (snapshot_state.is_valid() && snapshot_state->get_tick() == p_tick) {
		snapshot_state->apply_to_component(this);
		current_tick = p_tick;
		_is_rolling_back = false;
		_update_attribute_current_values();
	}
	_is_rolling_back = false;
}

void ASComponent::request_activate_ability(const StringName &p_tag) {
	if (_is_server()) {
		try_activate_ability_by_tag(p_tag);
	} else {
		// RPC call simulation
		// In GDExtension, you would use rpc("confirm_ability_activation", p_tag);
		// For now we simulate.
	}
}

void ASComponent::confirm_ability_activation(const StringName &p_tag) {
	// Reconcile prediction
}

ASComponent::ASComponent() {
	owned_tags.instantiate();
}
ASComponent::~ASComponent() {
	cancel_all_abilities();
	active_abilities.clear();
	active_effects.clear();
	unlocked_abilities.clear();
}
