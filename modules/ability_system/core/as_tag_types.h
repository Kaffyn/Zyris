/**************************************************************************/
/*  as_tag_types.h                                                        */
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
#include "src/core/as_utils.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/variant.hpp>
#else
#include "core/object/ref_counted.h"
#include "core/string/string_name.h"
#include "core/variant/dictionary.h"
#include "core/variant/variant.h"
#include "modules/ability_system/core/as_utils.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class AbilitySystem;
class ASComponent;

/**
 * Base tag structure with common functionality
 * Inspired by LimboAI's bb_param pattern for type safety.
 */
struct ASTagBase {
	StringName tag_name;
	ASTagType tag_type;

	// Constructor
	ASTagBase(const StringName &p_name, ASTagType p_type) : tag_name(p_name), tag_type(p_type) {}

	// Validation
	bool is_valid() const;

	// Type checking
	bool is_name_tag() const { return tag_type == NAME; }
	bool is_conditional_tag() const { return tag_type == CONDITIONAL; }
	bool is_event_tag() const { return tag_type == EVENT; }
};

/**
 * ASNameTag - Persistent Identity Tags
 */
struct ASNameTag : public ASTagBase {
	// Constructor
	ASNameTag(const StringName &p_name) : ASTagBase(p_name, NAME) {}

	// Convenience factory methods
	static ASNameTag create(const StringName &p_name) { return ASNameTag(p_name); }

	// Common state tags
	static ASNameTag STUNNED() { return ASNameTag("State.Stunned"); }
	static ASNameTag DEAD() { return ASNameTag("State.Dead"); }
	static ASNameTag INVISIBLE() { return ASNameTag("State.Invisible"); }

	// Common class tags
	static ASNameTag WARRIOR() { return ASNameTag("Class.Warrior"); }
	static ASNameTag MAGE() { return ASNameTag("Class.Mage"); }
	static ASNameTag ARCHER() { return ASNameTag("Class.Archer"); }

	// Common team tags
	static ASNameTag TEAM_BLUE() { return ASNameTag("Team.Blue"); }
	static ASNameTag TEAM_RED() { return ASNameTag("Team.Red"); }
};

/**
 * ASConditionalTag - Requirement/Block Tags
 */
struct ASConditionalTag : public ASTagBase {
	// Constructor
	ASConditionalTag(const StringName &p_name) : ASTagBase(p_name, CONDITIONAL) {}

	// Convenience factory methods
	static ASConditionalTag create(const StringName &p_name) { return ASConditionalTag(p_name); }

	// Common permission tags
	static ASConditionalTag CAN_PARRY() { return ASConditionalTag("Can.Parried"); }
	static ASConditionalTag CAN_DODGE() { return ASConditionalTag("Can.Dodged"); }
	static ASConditionalTag CAN_INTERRUPT() { return ASConditionalTag("Can.Interrupted"); }

	// Common immunity tags
	static ASConditionalTag IMMUNE_FIRE() { return ASConditionalTag("Immune.Fire"); }
	static ASConditionalTag IMMUNE_POISON() { return ASConditionalTag("Immune.Poison"); }
	static ASConditionalTag IMMUNE_PHYSICAL() { return ASConditionalTag("Immune.Physical"); }

	// Common state condition tags
	static ASConditionalTag GROUNDED() { return ASConditionalTag("State.Grounded"); }
	static ASConditionalTag FLYING() { return ASConditionalTag("State.Flying"); }
	static ASConditionalTag STEALTHED() { return ASConditionalTag("State.Stealthed"); }
};

/**
 * ASEventTag - Event Dispatcher Tags
 */
struct ASEventTag : public ASTagBase {
	// Constructor
	ASEventTag(const StringName &p_name) : ASTagBase(p_name, EVENT) {}

	// Convenience factory methods
	static ASEventTag create(const StringName &p_name) { return ASEventTag(p_name); }

	// Combat event tags
	static ASEventTag DAMAGE_DEALT() { return ASEventTag("Event.Damage.Dealt"); }
	static ASEventTag DAMAGE_TAKEN() { return ASEventTag("Event.Damage.Taken"); }
	static ASEventTag DAMAGE_BLOCKED() { return ASEventTag("Event.Damage.Blocked"); }
	static ASEventTag HEAL_RECEIVED() { return ASEventTag("Event.Heal.Received"); }

	// Ability event tags
	static ASEventTag ABILITY_ACTIVATED() { return ASEventTag("Event.Ability.Activated"); }
	static ASEventTag ABILITY_FAILED() { return ASEventTag("Event.Ability.Failed"); }
	static ASEventTag ABILITY_COOLDOWN_END() { return ASEventTag("Event.Ability.CooldownEnd"); }
	static ASEventTag ABILITY_INTERRUPTED() { return ASEventTag("Event.Ability.Interrupted"); }

	// State event tags
	static ASEventTag STUN_BEGIN() { return ASEventTag("Event.Stun.Begin"); }
	static ASEventTag STUN_END() { return ASEventTag("Event.Stun.End"); }
	static ASEventTag DEATH() { return ASEventTag("Event.Death"); }
	static ASEventTag RESPAWN() { return ASEventTag("Event.Respawn"); }

	// Weapon event tags
	static ASEventTag WEAPON_HIT() { return ASEventTag("Event.Weapon.Hit"); }
	static ASEventTag WEAPON_MISS() { return ASEventTag("Event.Weapon.Miss"); }
	static ASEventTag WEAPON_CRITICAL() { return ASEventTag("Event.Weapon.Critical"); }

	// Dispatch helper - creates event data and dispatches
	void dispatch(Node *p_instigator, float p_magnitude = 0.0f, const Dictionary &p_payload = Dictionary()) const;

	// Query helper - checks if event occurred recently
	bool occurred_recently(Node *p_target, float p_lookback_sec = 1.0f) const;
};

/**
 * ASTagUtils - Tag type utilities and validation
 */
class ASTagUtils : public RefCounted {
	GDCLASS(ASTagUtils, RefCounted);

protected:
	static void _bind_methods();

public:
	// Type validation
	static bool validate_tag_type(const StringName &p_tag, ASTagType p_expected_type);

	// Type detection from tag name convention
	static ASTagType detect_tag_type(const StringName &p_tag);

	// Factory that creates appropriate tag type based on naming convention
	// Note: Returns a temporary object, not a Ref
	static ASTagBase create_tag(const StringName &p_tag);

	// Common tag validation patterns
	static bool is_state_tag(const StringName &p_tag);
	static bool is_class_tag(const StringName &p_tag);
	static bool is_team_tag(const StringName &p_tag);
	static bool is_event_tag(const StringName &p_tag);
	static bool is_immune_tag(const StringName &p_tag);
	static bool is_can_tag(const StringName &p_tag);

	/**
	 * ASNameTag Historical Query API
	 */
	static bool name_was_tag_added(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static bool name_was_tag_removed(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static bool name_had_tag(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static Array name_get_recent_additions(Node *p_target, float p_lookback_sec = 1.0f);
	static Array name_get_recent_removals(Node *p_target, float p_lookback_sec = 1.0f);
	static Array name_get_recent_changes(Node *p_target, float p_lookback_sec = 1.0f);
	static int name_count_additions(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static int name_count_removals(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);

	/**
	 * ASConditionalTag Historical Query API
	 */
	static bool cond_was_tag_added(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static bool cond_was_tag_removed(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static bool cond_had_tag(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static Array cond_get_recent_additions(Node *p_target, float p_lookback_sec = 1.0f);
	static Array cond_get_recent_removals(Node *p_target, float p_lookback_sec = 1.0f);
	static Array cond_get_recent_changes(Node *p_target, float p_lookback_sec = 1.0f);
	static int cond_count_additions(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static int cond_count_removals(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);

	/**
	 * ASEventTag Historical Query API
	 */
	static bool event_did_occur(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static Array event_get_recent_events(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static Array event_get_all_recent_events(Node *p_target, float p_lookback_sec = 1.0f);
	static int event_count_occurrences(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static Dictionary event_get_last_data(const StringName &p_tag, Node *p_target);
	static float event_get_last_magnitude(const StringName &p_tag, Node *p_target);
	static Node *event_get_last_instigator(const StringName &p_tag, Node *p_target);

	/**
	 * Unified Historical Query API
	 */
	static bool history_was_tag_present(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static Array history_get_tag_history(const StringName &p_tag, Node *p_target, float p_lookback_sec = 1.0f);
	static Array history_get_all_changes(Node *p_target, float p_lookback_sec = 1.0f);
	static void history_dump(Node *p_target, float p_lookback_sec = 5.0f);
	static int history_get_total_size(Node *p_target);
};
