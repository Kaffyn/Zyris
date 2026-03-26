/**************************************************************************/
/*  as_utils.h                                                            */
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
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/variant.hpp>
#else
#include "core/string/string_name.h"
#include "core/templates/hash_map.h"
#include "core/templates/vector.h"
#include "core/variant/dictionary.h"
#include "scene/main/node.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class ASComponent;
class ASStateSnapshot;

enum ASTagType {
	NAME = 0, // Persistent identity tags
	CONDITIONAL = 1, // Requirement/block tags
	EVENT = 2, // Event dispatcher tags
	UNKNOWN = 255
};

/**
 * ModifierOp
 * Core attribute modification operations.
 */
enum ModifierOp {
	ADD = 0,
	MULTIPLY = 1,
	DIVIDE = 2,
	OVERRIDE = 3
};

/**
 * ASEffectModifier
 * Represents a single attribute modifier for effects.
 */
struct ASEffectModifier {
	StringName attribute;
	ModifierOp operation = ADD;
	float magnitude = 0.0f;

	// Helper methods
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASEffectModifierData
 * Runtime modifier data with custom magnitude support.
 */
struct ASEffectModifierData {
	StringName attribute;
	ModifierOp operation = ADD;
	float magnitude = 0.0f;
	bool use_custom_magnitude = false;

	// Helper methods
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASEffectRequirement
 * Attribute requirement for effect activation.
 */
struct ASEffectRequirement {
	StringName attribute;
	float amount = 0.0f;

	// Helper methods
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASAttributeValue
 * Stores base and current values for attributes.
 */
struct ASAttributeValue {
	float base_value = 0.0f;
	float current_value = 0.0f;

	// Helper methods
	void set_base(float p_value) {
		base_value = p_value;
		current_value = p_value;
	}
	void set_current(float p_value) { current_value = p_value; }
	float get_difference() const { return current_value - base_value; }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASEventTagData
 * Complete data for event dispatching.
 */
struct ASEventTagData {
	StringName event_tag;
	ObjectID instigator_id;
	ObjectID target_id;
	float magnitude = 0.0f;
	Dictionary custom_payload;
	double timestamp = 0.0;
	uint64_t tick_id = 0;

	// Helper methods
	Node *get_instigator() const { return Object::cast_to<Node>(ObjectDB::get_instance(instigator_id)); }
	Node *get_target() const { return Object::cast_to<Node>(ObjectDB::get_instance(target_id)); }
	void set_instigator(Node *p_node) { instigator_id = p_node ? p_node->get_instance_id() : ObjectID(); }
	void set_target(Node *p_node) { target_id = p_node ? p_node->get_instance_id() : ObjectID(); }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASEventTagHistoricalEntry
 * Historical entry for event occurrences.
 */
struct ASEventTagHistoricalEntry {
	ASEventTagData data;
	uint64_t tick = 0;

	// Helper methods
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASNameTagHistoricalEntry
 * Historical entry for NAME tag changes.
 */
struct ASNameTagHistoricalEntry {
	StringName tag_name;
	ObjectID target_id;
	double timestamp = 0.0;
	uint64_t tick_id = 0;
	bool added = true;

	// Helper methods
	Node *get_target() const { return Object::cast_to<Node>(ObjectDB::get_instance(target_id)); }
	void set_target(Node *p_node) { target_id = p_node ? p_node->get_instance_id() : ObjectID(); }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASConditionalTagHistoricalEntry
 * Historical entry for CONDITIONAL tag changes.
 */
struct ASConditionalTagHistoricalEntry {
	StringName tag_name;
	ObjectID target_id;
	double timestamp = 0.0;
	uint64_t tick_id = 0;
	bool added = true;

	// Helper methods
	Node *get_target() const { return Object::cast_to<Node>(ObjectDB::get_instance(target_id)); }
	void set_target(Node *p_node) { target_id = p_node ? p_node->get_instance_id() : ObjectID(); }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASEffectState
 * Represents the current state of an active effect.
 * Used for serialization and state management.
 */
struct ASEffectState {
	StringName tag;
	float remaining_time = 0.0f;
	float period_timer = 0.0f;
	int stack_count = 1;
	float level = 1.0f;

	// Helper methods
	bool is_expired() const { return remaining_time <= 0.0f; }
	bool is_period_ready() const { return period_timer <= 0.0f; }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASStateCacheEntry
 * Lightweight state cache entry for high-performance rollback.
 * Stores minimal state information for a specific tick.
 */
struct ASStateCacheEntry {
	uint32_t tick = 0;
	HashMap<StringName, float> attributes;
	Vector<StringName> tags;
	Vector<ASEffectState> active_effects;

	// Helper methods
	void clear();
	bool is_valid() const { return tick > 0; }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASCooldownData
 * Stores cooldown information for abilities and tags.
 */
struct ASCooldownData {
	float remaining = 0.0f;
	TypedArray<StringName> tags;

	// Helper methods
	bool is_expired() const { return remaining <= 0.0f; }
	void update(float p_delta) { remaining = MAX(0.0f, remaining - p_delta); }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);
};

/**
 * ASStateCache
 * Circular buffer for high-performance state caching.
 * Optimized for multiplayer rollback and prediction.
 */
class ASStateCache : public RefCounted {
	GDCLASS(ASStateCache, RefCounted);

private:
	Vector<ASStateCacheEntry> cache_buffer;
	uint32_t buffer_size = 128;
	uint32_t current_index = 0;
	uint32_t current_tick = 0;

protected:
	static void _bind_methods();

public:
	ASStateCache();
	~ASStateCache();

	// Core cache operations
	void capture_state(ASComponent *p_component);
	bool restore_state(ASComponent *p_component, uint32_t p_tick);
	void clear();

	// Buffer management
	void set_buffer_size(uint32_t p_size);
	uint32_t get_buffer_size() const { return buffer_size; }
	uint32_t get_current_tick() const { return current_tick; }

	// Query operations
	bool has_tick(uint32_t p_tick) const;
	ASStateCacheEntry get_entry(uint32_t p_tick) const;
	Array get_available_ticks() const;

	// Serialization
	Array serialize() const;
	void deserialize(const Array &p_data);

	// Debug utilities
	void dump_cache() const;
	int get_used_slots() const;
};

/**
 * ASComponentState
 * Complete state representation of an ASComponent.
 * Used for save/load operations and full serialization.
 */
struct ASComponentState {
	uint32_t tick = 0;
	HashMap<StringName, float> attributes;
	Vector<StringName> tags;
	Vector<ASEffectState> active_effects;
	HashMap<StringName, ASCooldownData> cooldowns;

	// Historical data (optional for full serialization)
	Vector<ASNameTagHistoricalEntry> name_history;
	Vector<ASConditionalTagHistoricalEntry> conditional_history;
	Vector<ASEventTagHistoricalEntry> event_history;

	// Helper methods
	void clear();
	bool is_valid() const { return tick > 0; }
	Dictionary to_dict() const;
	void from_dict(const Dictionary &p_dict);

	// Component operations
	void capture_from_component(ASComponent *p_component);
	void apply_to_component(ASComponent *p_component) const;

	// Comparison utilities
	bool equals(const ASComponentState &p_other) const;
	ASComponentState compute_diff(const ASComponentState &p_other) const;
	void apply_diff(const ASComponentState &p_diff);
};

/**
 * ASStateUtils
 * Utility functions for state management and serialization.
 */
class ASStateUtils : public RefCounted {
	GDCLASS(ASStateUtils, RefCounted);

protected:
	static void _bind_methods();

public:
	// State comparison
	static bool compare_states(const Ref<ASStateSnapshot> &p_a, const Ref<ASStateSnapshot> &p_b);
	static float compute_state_difference(const Ref<ASStateSnapshot> &p_a, const Ref<ASStateSnapshot> &p_b);

	// Serialization helpers
	static PackedByteArray serialize_state(const Ref<ASStateSnapshot> &p_state);
	static Ref<ASStateSnapshot> deserialize_state(const PackedByteArray &p_data);

	// Compression utilities
	static PackedByteArray compress_state(const Ref<ASStateSnapshot> &p_state);
	static Ref<ASStateSnapshot> decompress_state(const PackedByteArray &p_data);

	// Validation
	static bool validate_state(const Ref<ASStateSnapshot> &p_state);
	static Array get_validation_errors(const Ref<ASStateSnapshot> &p_state);

	// Debug utilities
	static void dump_state(const Ref<ASStateSnapshot> &p_state);
	static String state_to_string(const Ref<ASStateSnapshot> &p_state);
};

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

VARIANT_ENUM_CAST(::ASTagType);
VARIANT_ENUM_CAST(::ModifierOp);
