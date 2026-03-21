/**************************************************************************/
/*  as_component.h                                                        */
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
#include "src/core/as_effect_spec.h"
#include "src/resources/as_ability.h"
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/core/type_info.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#else
#include "core/object/gdvirtual.gen.inc"
#include "core/object/object.h"
#include "core/templates/hash_map.h"
#include "core/templates/vector.h"
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/resources/as_ability.h"
#include "scene/main/node.h"
#include "servers/audio/audio_stream.h"
#endif

// In Native mode, engine classes live in the global namespace.
#ifndef ABILITY_SYSTEM_GDEXTENSION

class CharacterBody2D;
class CharacterBody3D;
class AnimationPlayer;
class AnimatedSprite2D;
class AnimatedSprite3D;
class AudioStreamPlayer;
class AudioStreamPlayer2D;
class AudioStreamPlayer3D;
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class ASAttributeSet;
class ASTagSpec;
class ASAttribute;

class ASAbility;
class ASAbilitySpec;
class ASEffect;
class ASCue;
class ASCueSpec;
class ASContainer;
class ASPackage;
class ASStateSnapshot;

// In GDExtension mode, all Godot classes live inside namespace godot.
#ifdef ABILITY_SYSTEM_GDEXTENSION
namespace godot {
class CharacterBody2D;
class CharacterBody3D;
class AnimationPlayer;
class AnimatedSprite2D;
class AnimatedSprite3D;
class AudioStreamPlayer;
class AudioStreamPlayer2D;
class AudioStreamPlayer3D;
} // namespace godot
#endif

/**
 * ASComponent (ASC)
 * The central hub for an actor's logic. Manages abilities, attributes, and effects.
 */
class ASComponent : public Node {
	GDCLASS(ASComponent, Node);

public:
	CharacterBody2D *character_body_2d = nullptr;
	CharacterBody3D *character_body_3d = nullptr;
	HashMap<StringName, Node *> registered_nodes;
	Node *animation_player_node = nullptr;
	Node *audio_player_node = nullptr;

	// Core Ability System Data
	Vector<Ref<ASAttributeSet>> attribute_sets;
	Vector<Ref<ASAbilitySpec>> unlocked_abilities;
	Vector<Ref<ASAbilitySpec>> active_abilities;
	Vector<Ref<ASEffectSpec>> active_effects;
	Ref<ASTagSpec> owned_tags;
	Vector<Ref<ASCue>> registered_cues;
	Ref<ASContainer> container;
	Ref<ASStateSnapshot> snapshot_state;

	// Cooldown management
	struct CooldownData {
		float remaining = 0.0f;
		TypedArray<StringName> tags;
	};
	HashMap<StringName, CooldownData> active_cooldowns;

	// Guard against re-entrant trigger processing (e.g. effect grants tag -> trigger -> activate -> effect -> tag...)
	bool _handling_triggers = false;
	bool _updating_attributes = false;
	bool _is_rolling_back = false;
	bool _is_predicting = false;

protected:
	static void _bind_methods();
	void _notification(int p_what);

	void _on_attribute_set_attribute_changed(const StringName &p_name, float p_old_val, float p_new_val);
	void _update_attribute_current_values();

	void _process_effects(float p_delta);
	void _process_abilities(float p_delta);
	void _process_cooldowns(float p_delta);
	void _remove_effect_at_index(int p_idx);

public:
	// --- Cooldown API ---
	void start_cooldown(const StringName &p_ability_tag, float p_duration, const TypedArray<StringName> &p_cooldown_tags);
	bool is_on_cooldown(const StringName &p_ability_tag) const;
	float get_cooldown_remaining(const StringName &p_ability_tag) const;
	void tick(float p_delta);
	void _handle_ability_triggers(const StringName &p_tag, ASAbility::TriggerType p_type);

	// --- General Initialization & Management ---

	// --- Multiplayer & Prediction ---
	struct ASEffectState {
		StringName tag;
		float remaining_time = 0.0f;
		float period_timer = 0.0f;
		int stack_count = 1;
		float level = 1.0f;
	};

	struct ASStateCache {
		uint32_t tick = 0;
		HashMap<StringName, float> attributes;
		Vector<StringName> tags;
		Vector<ASEffectState> active_effects;
	};
	Vector<ASStateCache> cache_buffer;
	uint32_t current_tick = 0;

	bool _is_server() const;
	bool _is_local_authority() const;

	void capture_snapshot();
	void apply_snapshot(uint32_t p_tick);

	void request_activate_ability(const StringName &p_tag);
	void confirm_ability_activation(const StringName &p_tag);

	void apply_container(Ref<ASContainer> p_container, int p_level = 1);
	void add_attribute_set(Ref<ASAttributeSet> p_set);
	TypedArray<ASAttributeSet> get_attribute_sets() const;

	// --- Ability Unlocking API (Permanent for the Actor instance) ---
	void unlock_ability_by_tag(const StringName &p_tag);
	void unlock_ability_by_resource(const Ref<ASAbility> &p_ability);
	void lock_ability_by_tag(const StringName &p_tag);
	void lock_ability_by_resource(const Ref<ASAbility> &p_ability);
	bool is_ability_unlocked(const StringName &p_tag) const;

	// --- Ability Activation API (Transient execution) ---
	bool can_activate_ability_by_tag(const StringName &p_tag);
	bool try_activate_ability_by_tag(const StringName &p_tag);
	void cancel_ability_by_tag(const StringName &p_tag);
	bool is_ability_active(const StringName &p_tag) const;

	bool can_activate_ability_by_resource(const Ref<ASAbility> &p_ability);
	bool try_activate_ability_by_resource(const Ref<ASAbility> &p_ability);
	void cancel_ability_by_resource(const Ref<ASAbility> &p_ability);

	void cancel_all_abilities();

	// --- Effect Activation API ---
	bool can_activate_effect_by_tag(const StringName &p_tag);
	bool try_activate_effect_by_tag(const StringName &p_tag, float p_level = 1.0f, Object *p_target_node = nullptr);
	void cancel_effect_by_tag(const StringName &p_tag);

	bool can_activate_effect_by_resource(const Ref<ASEffect> &p_effect);
	bool try_activate_effect_by_resource(const Ref<ASEffect> &p_effect, float p_level = 1.0f, Object *p_target_node = nullptr);
	void cancel_effect_by_resource(const Ref<ASEffect> &p_effect);

	void remove_effect_by_tag(const StringName &p_tag);
	void remove_effect_by_resource(const Ref<ASEffect> &p_effect);
	void clear_effects();

	// --- Effect Execution API (Low level) ---
	void apply_effect_by_tag(const StringName &p_tag, float p_level = 1.0f, Object *p_target_node = nullptr);
	void apply_effect_by_resource(const Ref<ASEffect> &p_effect, float p_level = 1.0f, Object *p_target_node = nullptr);
	void apply_package(const Ref<ASPackage> &p_package, float p_level = 1.0f, ASComponent *p_source_component = nullptr);

	Ref<ASEffectSpec> make_outgoing_spec(Ref<ASEffect> p_effect, float p_level = 1.0f, Object *p_target_node = nullptr);
	void apply_effect_spec_to_self(Ref<ASEffectSpec> p_spec);
	void apply_effect_spec_to_target(Ref<ASEffectSpec> p_spec, ASComponent *p_target);
	void remove_active_effect(Ref<ASEffectSpec> p_spec);
	bool has_active_effect_by_tag(const StringName &p_tag) const;
	bool has_active_effect_by_resource(const Ref<ASEffect> &p_effect) const;
	Ref<ASEffect> find_effect_by_tag(const StringName &p_tag) const;

	// --- Attribute API ---
	float get_attribute_value_by_tag(const StringName &p_tag) const;
	float get_attribute_base_value_by_tag(const StringName &p_tag) const;
	void set_attribute_base_value_by_tag(const StringName &p_tag, float p_value);
	bool has_attribute_by_tag(const StringName &p_tag) const;

	float get_attribute_value_by_resource(const Ref<ASAttribute> &p_attribute) const;
	float get_attribute_base_value_by_resource(const Ref<ASAttribute> &p_attribute) const;
	void set_attribute_base_value_by_resource(const Ref<ASAttribute> &p_attribute, float p_value);
	bool has_attribute_by_resource(const Ref<ASAttribute> &p_attribute) const;

	// --- Tag Management ---
	void add_tag(const StringName &p_tag);
	void remove_tag(const StringName &p_tag);
	void remove_all_tags();
	bool has_tag(const StringName &p_tag) const;
	TypedArray<StringName> get_tags() const;
	Ref<ASTagSpec> get_owned_tags() const;

	// --- Cue Activation API ---
	bool can_activate_cue_by_tag(const StringName &p_tag);
	bool try_activate_cue_by_tag(const StringName &p_tag, const Dictionary &p_data = Dictionary(), Object *p_target_node = nullptr);
	void cancel_cue_by_tag(const StringName &p_tag);

	bool can_activate_cue_by_resource(const Ref<ASCue> &p_cue);
	bool try_activate_cue_by_resource(const Ref<ASCue> &p_cue, const Dictionary &p_data = Dictionary(), Object *p_target_node = nullptr);
	void cancel_cue_by_resource(const Ref<ASCue> &p_cue);

	void _execute_cue_with_spec(const StringName &p_tag, Ref<ASCueSpec> p_spec);

	void register_cue_resource(Ref<ASCue> p_cue);
	void unregister_cue_resource(const StringName &p_tag);
	Ref<ASCue> get_cue_resource(const StringName &p_tag) const;

	// --- Montage API ---
	void play_montage(const StringName &p_name, Node *p_target = nullptr);
	bool is_montage_playing(const StringName &p_name, Node *p_target = nullptr) const;
	void play_sound(Ref<AudioStream> p_sound, Node *p_target = nullptr);

	// --- Node Registration ---
	void register_node(const StringName &p_name, Node *p_node);
	void unregister_node(const StringName &p_name);
	Node *get_node_ptr(const StringName &p_name) const;

	void set_animation_player(Node *p_node) { animation_player_node = p_node; }
	Node *get_animation_player() const { return animation_player_node; }

	void set_audio_player(Node *p_node) { audio_player_node = p_node; }
	Node *get_audio_player() const { return audio_player_node; }

	// --- Logic & Calculations ---
	float calculate_modifier_magnitude(const Ref<ASEffectSpec> &p_spec, int p_modifier_idx) const;
	float calculate_effect_duration(const Ref<ASEffectSpec> &p_spec) const;

	void set_container(Ref<ASContainer> p_container);
	Ref<ASContainer> get_container() const;

	void set_snapshot_state(Ref<ASStateSnapshot> p_snapshot);
	Ref<ASStateSnapshot> get_snapshot_state() const;

	// --- Metadata Preview (Pre-activation queries) ---
	float get_ability_duration_preview(const StringName &p_tag) const;
	float get_ability_cost_preview(const StringName &p_tag, const StringName &p_attr) const;
	float get_ability_cooldown_preview(const StringName &p_tag) const;

	// Virtual callback for custom magnitudes from scripts
	GDVIRTUAL2RC(float, _on_calculate_custom_magnitude, Ref<ASEffectSpec>, int);

	ASComponent();
	~ASComponent();
};
