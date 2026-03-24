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
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_tag_types.h"
#include "modules/ability_system/core/as_utils.h"
#include "modules/ability_system/scene/as_component.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include "src/scene/as_component.h"
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;
#endif

namespace godot {

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

namespace ASTagUtils {

bool validate_tag_type(const StringName &p_tag, ASTagType p_expected_type) {
	AbilitySystem *as = AbilitySystem::get_singleton();
	if (!as) {
		return false;
	}

	// Check If Tag Exists
	if (!as->is_tag_registered(p_tag)) {
		return false;
	}

	// Check If Tag Type Matches Expected Type
	// Note: This Depends on AbilitySystem API Having Tag Type Information
	// For Now, We'll Use Naming Convention Validation

	ASTagType detected_type = detect_tag_type(p_tag);
	return detected_type == p_expected_type;
}

ASTagType detect_tag_type(const StringName &p_tag) {
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
		return ASTagType::CONDITIONAL;
	}

	// Default to NAME for All Other Tags (State.*, Class.*, Team.*, etc.)
	// This Matches BUSINESS_RULES.md Where Most Tags Are Persistent Identity
	return ASTagType::NAME;
}

ASTagBase create_tag(const StringName &p_tag) {
	ASTagType type = detect_tag_type(p_tag);

	switch (type) {
		case ASTagType::NAME:
			return ASNameTag::create(p_tag);
		case ASTagType::CONDITIONAL:
			return ASConditionalTag::create(p_tag);
		case ASTagType::EVENT:
			return ASEventTag::create(p_tag);
		default:
			return ASNameTag::create(p_tag); // Fallback
	}
}

// Common Tag Validation Patterns
bool is_state_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("State.");
}

bool is_class_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Class.");
}

bool is_team_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Team.");
}

bool is_event_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Event.");
}

bool is_immune_tag(const StringName &p_tag) {
	return String(p_tag).begins_with("Immune.");
}

bool is_can_tag(const StringName &p_tag) {
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

namespace NameHistory {

bool was_tag_added(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return false;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return false;

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

bool was_tag_removed(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return false;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return false;

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

bool had_tag(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	return was_tag_added(p_tag, p_target, p_lookback_sec);
}

Array get_recent_additions(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

Array get_recent_removals(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

Array get_recent_changes(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

int count_additions(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return 0;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return 0;

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

int count_removals(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return 0;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return 0;

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

} // namespace NameHistory

namespace ConditionalHistory {

bool was_tag_added(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return false;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return false;

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

bool was_tag_removed(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return false;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return false;

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

bool had_tag(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	return was_tag_added(p_tag, p_target, p_lookback_sec);
}

Array get_recent_additions(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

Array get_recent_removals(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

Array get_recent_changes(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

int count_additions(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return 0;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return 0;

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

int count_removals(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return 0;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return 0;

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

} // namespace ConditionalHistory

namespace EventHistory {

bool did_occur(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return false;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return false;

	return asc->has_event_occurred(p_tag, p_lookback_sec);
}

Array get_recent_events(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

Array get_all_recent_events(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

int count_occurrences(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return 0;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return 0;

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

Dictionary get_last_event_data(const StringName &p_tag, Node *p_target) {
	Dictionary result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (entry.data.event_tag == p_tag) {
			result = entry.data.custom_payload;
			break;
		}
	}
	return result;
}

float get_last_magnitude(const StringName &p_tag, Node *p_target) {
	if (!p_target)
		return 0.0f;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return 0.0f;

	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (entry.data.event_tag == p_tag) {
			return entry.data.magnitude;
		}
	}
	return 0.0f;
}

Node *get_last_instigator(const StringName &p_tag, Node *p_target) {
	if (!p_target)
		return nullptr;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return nullptr;

	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (entry.data.event_tag == p_tag) {
			return Object::cast_to<Node>(ObjectDB::get_instance(entry.data.instigator_id));
		}
	}
	return nullptr;
}

} // namespace EventHistory

namespace UnifiedHistory {

bool was_tag_present(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	ASTagType type = ASTagUtils::detect_tag_type(p_tag);

	switch (type) {
		case ASTagType::NAME:
			return NameHistory::had_tag(p_tag, p_target, p_lookback_sec);
		case ASTagType::CONDITIONAL:
			return ConditionalHistory::had_tag(p_tag, p_target, p_lookback_sec);
		case ASTagType::EVENT:
			return EventHistory::did_occur(p_tag, p_target, p_lookback_sec);
		default:
			return false;
	}
}

Array get_tag_history(const StringName &p_tag, Node *p_target, float p_lookback_sec) {
	ASTagType type = ASTagUtils::detect_tag_type(p_tag);

	switch (type) {
		case ASTagType::NAME:
			return NameHistory::get_recent_changes(p_target, p_lookback_sec);
		case ASTagType::CONDITIONAL:
			return ConditionalHistory::get_recent_changes(p_target, p_lookback_sec);
		case ASTagType::EVENT:
			return EventHistory::get_recent_events(p_tag, p_target, p_lookback_sec);
		default:
			return Array();
	}
}

Array get_all_changes(Node *p_target, float p_lookback_sec) {
	Array result;
	if (!p_target)
		return result;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return result;

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

	// Add ASEventTag Occurrences
	for (int i = asc->_event_history.size() - 1; i >= 0; i--) {
		const ASEventTagHistoricalEntry &entry = asc->_event_history[i];

		if (current_time - entry.data.timestamp > (double)p_lookback_sec) {
			break;
		}

		Dictionary change_dict;
		change_dict["type"] = "EVENT";
		change_dict["tag"] = entry.data.event_tag;
		change_dict["magnitude"] = entry.data.magnitude;
		change_dict["custom_payload"] = entry.data.custom_payload;
		change_dict["timestamp"] = entry.data.timestamp;
		change_dict["tick_id"] = entry.data.tick_id;
		result.push_back(change_dict);
	}

	return result;
}

void dump_history(Node *p_target, float p_lookback_sec) {
	if (!p_target)
		return;

	Array changes = get_all_changes(p_target, p_lookback_sec);

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

int get_total_history_size(Node *p_target) {
	if (!p_target)
		return 0;

	ASComponent *asc = ASComponent::get_from_node(p_target);
	if (!asc)
		return 0;

	return asc->_name_history.size() + asc->_cond_history.size() + asc->_event_history.size();
}

} // namespace UnifiedHistory

} // namespace ASTagUtils

} // namespace godot
