/**************************************************************************/
/*  as_tag_types.cpp                                                      */
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
#include "src/core/as_tag_types.h"
#include "src/core/ability_system.h"
#include "src/core/as_utils.h"
#include "src/scene/as_component.h"
#include <godot_cpp/classes/time.hpp>
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

bool ASTagBase::is_valid() const {
	AbilitySystem *as = AbilitySystem::get_singleton();
	return !tag_name.is_empty() && as && as->is_tag_registered(tag_name);
}

void ASEventTag::dispatch(Node *p_instigator, float p_magnitude, const Dictionary &p_payload) const {
	if (ASComponent *asc = ASComponent::get_from_node(p_instigator)) {
		asc->dispatch_event(tag_name, p_instigator, p_magnitude, p_payload);
	}
}

bool ASEventTag::occurred_recently(Node *p_target, float p_lookback_sec) const {
	if (ASComponent *asc = ASComponent::get_from_node(p_target)) {
		return asc->has_event_occurred(tag_name, p_lookback_sec);
	}
	return false;
}

void ASTagUtils::_bind_methods() {
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("validate_tag_type", "tag", "expected_type"), &ASTagUtils::validate_tag_type);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("detect_tag_type", "tag"), &ASTagUtils::detect_tag_type);
	// Note: create_tag returns ASTagBase (struct), so it cannot be easily bound to ClassDB without wrapping.
	// We skip binding create_tag for now as it's mostly internal.

	ClassDB::bind_static_method("ASTagUtils", D_METHOD("is_state_tag", "tag"), &ASTagUtils::is_state_tag);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("is_class_tag", "tag"), &ASTagUtils::is_class_tag);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("is_team_tag", "tag"), &ASTagUtils::is_team_tag);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("is_event_tag", "tag"), &ASTagUtils::is_event_tag);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("is_immune_tag", "tag"), &ASTagUtils::is_immune_tag);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("is_can_tag", "tag"), &ASTagUtils::is_can_tag);

	// Name History
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_was_tag_added", "tag", "target", "lookback_sec"), &ASTagUtils::name_was_tag_added, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_was_tag_removed", "tag", "target", "lookback_sec"), &ASTagUtils::name_was_tag_removed, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_had_tag", "tag", "target", "lookback_sec"), &ASTagUtils::name_had_tag, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_get_recent_additions", "target", "lookback_sec"), &ASTagUtils::name_get_recent_additions, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_get_recent_removals", "target", "lookback_sec"), &ASTagUtils::name_get_recent_removals, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_get_recent_changes", "target", "lookback_sec"), &ASTagUtils::name_get_recent_changes, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_count_additions", "tag", "target", "lookback_sec"), &ASTagUtils::name_count_additions, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("name_count_removals", "tag", "target", "lookback_sec"), &ASTagUtils::name_count_removals, DEFVAL(1.0f));

	// Conditional History
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_was_tag_added", "tag", "target", "lookback_sec"), &ASTagUtils::cond_was_tag_added, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_was_tag_removed", "tag", "target", "lookback_sec"), &ASTagUtils::cond_was_tag_removed, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_had_tag", "tag", "target", "lookback_sec"), &ASTagUtils::cond_had_tag, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_get_recent_additions", "target", "lookback_sec"), &ASTagUtils::cond_get_recent_additions, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_get_recent_removals", "target", "lookback_sec"), &ASTagUtils::cond_get_recent_removals, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_get_recent_changes", "target", "lookback_sec"), &ASTagUtils::cond_get_recent_changes, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_count_additions", "tag", "target", "lookback_sec"), &ASTagUtils::cond_count_additions, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("cond_count_removals", "tag", "target", "lookback_sec"), &ASTagUtils::cond_count_removals, DEFVAL(1.0f));

	// Event History
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("event_did_occur", "tag", "target", "lookback_sec"), &ASTagUtils::event_did_occur, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("event_get_recent_events", "tag", "target", "lookback_sec"), &ASTagUtils::event_get_recent_events, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("event_get_all_recent_events", "target", "lookback_sec"), &ASTagUtils::event_get_all_recent_events, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("event_count_occurrences", "tag", "target", "lookback_sec"), &ASTagUtils::event_count_occurrences, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("event_get_last_data", "tag", "target"), &ASTagUtils::event_get_last_data);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("event_get_last_magnitude", "tag", "target"), &ASTagUtils::event_get_last_magnitude);
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("event_get_last_instigator", "tag", "target"), &ASTagUtils::event_get_last_instigator);

	// Unified History
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("history_was_tag_present", "tag", "target", "lookback_sec"), &ASTagUtils::history_was_tag_present, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("history_get_tag_history", "tag", "target", "lookback_sec"), &ASTagUtils::history_get_tag_history, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("history_get_all_changes", "target", "lookback_sec"), &ASTagUtils::history_get_all_changes, DEFVAL(1.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("history_dump", "target", "lookback_sec"), &ASTagUtils::history_dump, DEFVAL(5.0f));
	ClassDB::bind_static_method("ASTagUtils", D_METHOD("history_get_total_size", "target"), &ASTagUtils::history_get_total_size);

	// Bind Tag Type Constants
	ClassDB::bind_integer_constant("ASTagUtils", "ASTagType", "NAME", (int)ASTagType::NAME);
	ClassDB::bind_integer_constant("ASTagUtils", "ASTagType", "CONDITIONAL", (int)ASTagType::CONDITIONAL);
	ClassDB::bind_integer_constant("ASTagUtils", "ASTagType", "EVENT", (int)ASTagType::EVENT);
	ClassDB::bind_integer_constant("ASTagUtils", "ASTagType", "UNKNOWN", (int)ASTagType::UNKNOWN);
}

bool ASTagUtils::validate_tag_type(const StringName &p_tag, ASTagType p_expected_type) {
	AbilitySystem *as = AbilitySystem::get_singleton();
	if (!as) {
		return false;
	}

	// Check If Tag Exists
	if (!as->is_tag_registered(p_tag)) {
		return false;
	}

	ASTagType detected_type = detect_tag_type(p_tag);
	return detected_type == p_expected_type;
}

ASTagType ASTagUtils::detect_tag_type(const StringName &p_tag) {
	String tag_str = p_tag;

	// Event Tags Always Start With "Event."
	if (tag_str.begins_with("Event.")) {
		return ASTagType::EVENT;
	}

	// Conditional Tags Patterns
	if (tag_str.begins_with("Can.") ||
			tag_str.begins_with("Immune.") ||
			tag_str.begins_with("State.Grounded") ||
			tag_str.begins_with("State.Flying") ||
			tag_str.begins_with("State.Stealthed")) {
		return CONDITIONAL;
	}

	// Default to NAME for All Other Tags (State.*, Class.*, Team.*, etc.)
	return NAME;
}

ASTagBase ASTagUtils::create_tag(const StringName &p_tag) {
	ASTagType type = detect_tag_type(p_tag);

	switch (type) {
		case NAME:
			return ASNameTag::create(p_tag);
		case CONDITIONAL:
			return ASConditionalTag::create(p_tag);
		case EVENT:
			return ASEventTag::create(p_tag);
		default:
			return ASNameTag::create(p_tag); // Fallback
	}
}

// Common Tag Validation Patterns
bool ASTagUtils::is_state_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("State.");
}

bool ASTagUtils::is_class_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Class.");
}

bool ASTagUtils::is_team_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Team.");
}

bool ASTagUtils::is_event_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Event.");
}

bool ASTagUtils::is_immune_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Immune.");
}

bool ASTagUtils::is_can_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Can.");
}

// --- Historical API Implementations ---

// Helper to Get Current Timestamp
static double _get_current_time() {
#ifdef ABILITY_SYSTEM_GDEXTENSION
	return (double)Time::get_singleton()->get_ticks_msec() / 1000.0;
#else
	return (double)OS::get_singleton()->get_ticks_msec() / 1000.0;
#endif
}

bool ASTagUtils::name_was_tag_added(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return false;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return false;
	}

	double current_time = _get_current_time();
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && entry.added) {
			return true;
		}
	}
	return false;
}

bool ASTagUtils::name_was_tag_removed(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return false;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return false;
	}

	double current_time = _get_current_time();
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && !entry.added) {
			return true;
		}
	}
	return false;
}

bool ASTagUtils::name_had_tag(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	return name_was_tag_added(p_tag, p_target, p_lookback_sec);
}

Array ASTagUtils::name_get_recent_additions(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.added) {
			Dictionary entry_dict;
			entry_dict["tag"] = entry.tag_name;
			entry_dict["timestamp"] = entry.timestamp;
			entry_dict["tick_id"] = entry.tick_id;
			result.push_back(entry_dict);
		}
	}
	return result;
}

Array ASTagUtils::name_get_recent_removals(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (!entry.added) {
			Dictionary entry_dict;
			entry_dict["tag"] = entry.tag_name;
			entry_dict["timestamp"] = entry.timestamp;
			entry_dict["tick_id"] = entry.tick_id;
			result.push_back(entry_dict);
		}
	}
	return result;
}

Array ASTagUtils::name_get_recent_changes(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		Dictionary entry_dict;
		entry_dict["tag"] = entry.tag_name;
		entry_dict["added"] = entry.added;
		entry_dict["timestamp"] = entry.timestamp;
		entry_dict["tick_id"] = entry.tick_id;
		result.push_back(entry_dict);
	}
	return result;
}

int ASTagUtils::name_count_additions(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return 0;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return 0;
	}

	int count = 0;
	double current_time = _get_current_time();
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && entry.added) {
			count++;
		}
	}
	return count;
}

int ASTagUtils::name_count_removals(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return 0;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return 0;
	}

	int count = 0;
	double current_time = _get_current_time();
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && !entry.added) {
			count++;
		}
	}
	return count;
}

bool ASTagUtils::cond_was_tag_added(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return false;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return false;
	}

	double current_time = _get_current_time();
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && entry.added) {
			return true;
		}
	}
	return false;
}

bool ASTagUtils::cond_was_tag_removed(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return false;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return false;
	}

	double current_time = _get_current_time();
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && !entry.added) {
			return true;
		}
	}
	return false;
}

bool ASTagUtils::cond_had_tag(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	return cond_was_tag_added(p_tag, p_target, p_lookback_sec);
}

Array ASTagUtils::cond_get_recent_additions(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.added) {
			Dictionary entry_dict;
			entry_dict["tag"] = entry.tag_name;
			entry_dict["timestamp"] = entry.timestamp;
			entry_dict["tick_id"] = entry.tick_id;
			result.push_back(entry_dict);
		}
	}
	return result;
}

Array ASTagUtils::cond_get_recent_removals(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (!entry.added) {
			Dictionary entry_dict;
			entry_dict["tag"] = entry.tag_name;
			entry_dict["timestamp"] = entry.timestamp;
			entry_dict["tick_id"] = entry.tick_id;
			result.push_back(entry_dict);
		}
	}
	return result;
}

Array ASTagUtils::cond_get_recent_changes(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		Dictionary entry_dict;
		entry_dict["tag"] = entry.tag_name;
		entry_dict["added"] = entry.added;
		entry_dict["timestamp"] = entry.timestamp;
		entry_dict["tick_id"] = entry.tick_id;
		result.push_back(entry_dict);
	}
	return result;
}

int ASTagUtils::cond_count_additions(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return 0;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return 0;
	}

	int count = 0;
	double current_time = _get_current_time();
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && entry.added) {
			count++;
		}
	}
	return count;
}

int ASTagUtils::cond_count_removals(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return 0;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return 0;
	}

	int count = 0;
	double current_time = _get_current_time();
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.tag_name == p_tag && !entry.added) {
			count++;
		}
	}
	return count;
}

bool ASTagUtils::event_did_occur(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return false;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return false;
	}

	return asc->has_event_occurred(p_tag, p_lookback_sec);
}

Array ASTagUtils::event_get_recent_events(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (current_time - entry.data.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.data.event_tag == p_tag) {
			Dictionary event_dict;
			event_dict["event_tag"] = entry.data.event_tag;
			event_dict["instigator_id"] = entry.data.instigator_id;
			event_dict["target_id"] = entry.data.target_id;
			event_dict["magnitude"] = entry.data.magnitude;
			event_dict["custom_payload"] = entry.data.custom_payload;
			event_dict["timestamp"] = entry.data.timestamp;
			event_dict["tick_id"] = entry.data.tick_id;
			result.push_back(event_dict);
		}
	}
	return result;
}

Array ASTagUtils::event_get_all_recent_events(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();
	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (current_time - entry.data.timestamp > (double)p_lookback_sec) {
			break;
		}

		Dictionary event_dict;
		event_dict["event_tag"] = entry.data.event_tag;
		event_dict["instigator_id"] = entry.data.instigator_id;
		event_dict["target_id"] = entry.data.target_id;
		event_dict["magnitude"] = entry.data.magnitude;
		event_dict["custom_payload"] = entry.data.custom_payload;
		event_dict["timestamp"] = entry.data.timestamp;
		event_dict["tick_id"] = entry.data.tick_id;
		result.push_back(event_dict);
	}
	return result;
}

int ASTagUtils::event_count_occurrences(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return 0;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return 0;
	}

	int count = 0;
	double current_time = _get_current_time();
	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (current_time - entry.data.timestamp > (double)p_lookback_sec) {
			break;
		}

		if (entry.data.event_tag == p_tag) {
			count++;
		}
	}
	return count;
}

Dictionary ASTagUtils::event_get_last_data(const StringName &p_tag, Node *p_target) {
	Dictionary result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (entry.data.event_tag == p_tag) {
			result = entry.data.custom_payload;
			break;
		}
	}
	return result;
}

float ASTagUtils::event_get_last_magnitude(const StringName &p_tag, Node *p_target) {
	if (!p_target) {
		return 0.0f;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return 0.0f;
	}

	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (entry.data.event_tag == p_tag) {
			return entry.data.magnitude;
		}
	}
	return 0.0f;
}

Node *ASTagUtils::event_get_last_instigator(const StringName &p_tag, Node *p_target) {
	if (!p_target) {
		return nullptr;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return nullptr;
	}

	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (entry.data.event_tag == p_tag) {
			return Object::cast_to<Node>(ObjectDB::get_instance(entry.data.instigator_id));
		}
	}
	return nullptr;
}

bool ASTagUtils::history_was_tag_present(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	ASTagType type = ASTagUtils::detect_tag_type(p_tag);

	switch (type) {
		case NAME:
			return name_had_tag(p_tag, p_target, p_lookback_sec);
		case CONDITIONAL:
			return cond_had_tag(p_tag, p_target, p_lookback_sec);
		case EVENT:
			return event_did_occur(p_tag, p_target, p_lookback_sec);
		default:
			return false;
	}
}

Array ASTagUtils::history_get_tag_history(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	ASTagType type = ASTagUtils::detect_tag_type(p_tag);

	switch (type) {
		case NAME:
			return name_get_recent_changes(p_target, p_lookback_sec);
		case CONDITIONAL:
			return cond_get_recent_changes(p_target, p_lookback_sec);
		case EVENT:
			return event_get_recent_events(p_tag, p_target, p_lookback_sec);
		default:
			return Array();
	}
}

Array ASTagUtils::history_get_all_changes(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target) {
		return result;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return result;
	}

	double current_time = _get_current_time();

	// Add ASNameTag Changes
	for (int i = asc->_name_history.size() - 1; i >= 0; i--) {
		const ASNameTagHistoricalEntry &entry = asc->_name_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		Dictionary change_dict;
		change_dict["type"] = "NAME";
		change_dict["tag"] = entry.tag_name;
		change_dict["added"] = entry.added;
		change_dict["timestamp"] = entry.timestamp;
		change_dict["tick_id"] = entry.tick_id;
		result.push_back(change_dict);
	}

	// Add ASConditionalTag Changes
	for (int i = asc->_cond_history.size() - 1; i >= 0; i--) {
		const ASConditionalTagHistoricalEntry &entry = asc->_cond_history[i];

		if (current_time - entry.timestamp > (double)p_lookback_sec) {
			break;
		}

		Dictionary change_dict;
		change_dict["type"] = "CONDITIONAL";
		change_dict["tag"] = entry.tag_name;
		change_dict["added"] = entry.added;
		change_dict["timestamp"] = entry.timestamp;
		change_dict["tick_id"] = entry.tick_id;
		result.push_back(change_dict);
	}

	// Add ASEventTag Changes
	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (current_time - entry.data.timestamp > (double)p_lookback_sec) {
			break;
		}

		Dictionary change_dict;
		change_dict["type"] = "EVENT";
		change_dict["tag"] = entry.data.event_tag;
		change_dict["custom_payload"] = entry.data.custom_payload;
		change_dict["timestamp"] = entry.data.timestamp;
		change_dict["tick_id"] = entry.data.tick_id;
		change_dict["magnitude"] = entry.data.magnitude;
		result.push_back(change_dict);
	}

	return result;
}

void ASTagUtils::history_dump(Node *p_target, float p_lookback_sec) {
	if (!p_target) {
		return;
	}

	Array changes = history_get_all_changes(p_target, p_lookback_sec);

	UtilityFunctions::print("=== AS Tag History Dump for ", p_target->get_name(), " (last ", String::num(p_lookback_sec), "s) ===");

	for (int i = 0; i < changes.size(); i++) {
		Dictionary change = changes[i];
		String type = change["type"];
		String tag = change["tag"];
		double timestamp = change["timestamp"];

		if (type == "EVENT") {
			float magnitude = change["magnitude"];
			UtilityFunctions::print("[", type, "] ", tag, " (mag:", String::num(magnitude), ") @ ", String::num(timestamp, 3));
		} else {
			bool added = change["added"];
			UtilityFunctions::print("[", type, "] ", tag, " ", added ? "ADDED" : "REMOVED", " @ ", String::num(timestamp, 3));
		}
	}

	UtilityFunctions::print("=== End History Dump ===");
}

int ASTagUtils::history_get_total_size(Node *p_target) {
	if (!p_target) {
		return 0;
	}

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc) {
		return 0;
	}

	return asc->_name_history.size() + asc->_cond_history.size() + asc->_event_history.size();
}
