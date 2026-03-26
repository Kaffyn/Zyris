/**************************************************************************/
/*  ability_system.cpp                                                    */
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
#include "src/core/ability_system.h"
#include "src/core/as_tag_types.h"
#include "src/core/as_utils.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_tag_types.h"
#include "modules/ability_system/core/as_utils.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#else
#include "core/config/engine.h"
#include "core/config/project_settings.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

AbilitySystem *AbilitySystem::singleton = nullptr;

void AbilitySystem::_bind_methods() {
	ClassDB::bind_method(D_METHOD("run_tests"), &AbilitySystem::run_tests);
	ClassDB::bind_method(D_METHOD("register_tag", "tag", "type", "owner_id"), &AbilitySystem::register_tag, DEFVAL(ASTagType::NAME), DEFVAL(0));
	ClassDB::bind_method(D_METHOD("rename_tag", "old_tag", "new_tag"), &AbilitySystem::rename_tag);
	ClassDB::bind_method(D_METHOD("is_tag_registered", "tag"), &AbilitySystem::is_tag_registered);
	ClassDB::bind_method(D_METHOD("unregister_tag", "tag"), &AbilitySystem::unregister_tag);
	ClassDB::bind_method(D_METHOD("remove_tag_branch", "tag"), &AbilitySystem::remove_tag_branch);
	ClassDB::bind_method(D_METHOD("get_registered_tags"), &AbilitySystem::get_registered_tags);
	ClassDB::bind_method(D_METHOD("get_registered_tags_of_type", "type"), &AbilitySystem::get_registered_tags_of_type);
	ClassDB::bind_method(D_METHOD("get_tag_type", "tag"), &AbilitySystem::get_tag_type);

	ADD_SIGNAL(MethodInfo("tags_changed"));

	ClassDB::bind_method(D_METHOD("register_resource_name", "name", "owner_id"), &AbilitySystem::register_resource_name);
	ClassDB::bind_method(D_METHOD("unregister_resource_name", "name"), &AbilitySystem::unregister_resource_name);
	ClassDB::bind_method(D_METHOD("get_resource_name_owner", "name"), &AbilitySystem::get_resource_name_owner);

	ClassDB::bind_static_method(get_class_static(), D_METHOD("tag_matches", "tag", "match_against", "exact"), &AbilitySystem::tag_matches, DEFVAL(false));

	BIND_ENUM_CONSTANT(ASTagType::NAME);
	BIND_ENUM_CONSTANT(ASTagType::CONDITIONAL);
	BIND_ENUM_CONSTANT(ASTagType::EVENT);
}

void AbilitySystem::_load_settings() {
	ProjectSettings *ps = ProjectSettings::get_singleton();

	// Load Name Tags
	if (ps->has_setting("ability_system/common/name_tags")) {
		PackedStringArray tags = ps->get_setting("ability_system/common/name_tags");
		for (int i = 0; i < tags.size(); i++) {
			registered_tags[tags[i]] = (ASTagType)ASTagType::NAME;
		}
	} else {
		ps->set_setting("ability_system/common/name_tags", PackedStringArray());
		ps->set_initial_value("ability_system/common/name_tags", PackedStringArray());
	}

	// Load Conditional Tags
	if (ps->has_setting("ability_system/common/conditional_tags")) {
		PackedStringArray tags = ps->get_setting("ability_system/common/conditional_tags");
		for (int i = 0; i < tags.size(); i++) {
			registered_tags[tags[i]] = (ASTagType)ASTagType::CONDITIONAL;
		}
	} else {
		ps->set_setting("ability_system/common/conditional_tags", PackedStringArray());
		ps->set_initial_value("ability_system/common/conditional_tags", PackedStringArray());
	}

	// Load Event Tags
	if (ps->has_setting("ability_system/common/event_tags")) {
		PackedStringArray tags = ps->get_setting("ability_system/common/event_tags");
		for (int i = 0; i < tags.size(); i++) {
			registered_tags[tags[i]] = (ASTagType)ASTagType::EVENT;
		}
	} else {
		ps->set_setting("ability_system/common/event_tags", PackedStringArray());
		ps->set_initial_value("ability_system/common/event_tags", PackedStringArray());
	}
}

void AbilitySystem::_update_settings() {
	PackedStringArray name_tags;
	PackedStringArray cond_tags;
	PackedStringArray event_tags;

	for (const KeyValue<StringName, ASTagType> &E : registered_tags) {
		if (E.value == (ASTagType)ASTagType::NAME) {
			name_tags.push_back(E.key);
		} else if (E.value == (ASTagType)ASTagType::CONDITIONAL) {
			cond_tags.push_back(E.key);
		} else {
			event_tags.push_back(E.key);
		}
	}

	name_tags.sort();
	cond_tags.sort();
	event_tags.sort();

	ProjectSettings::get_singleton()->set_setting("ability_system/common/name_tags", name_tags);
	ProjectSettings::get_singleton()->set_setting("ability_system/common/conditional_tags", cond_tags);
	ProjectSettings::get_singleton()->set_setting("ability_system/common/event_tags", event_tags);

	if (ProjectSettings::get_singleton()->has_method("save") || Engine::get_singleton()->is_editor_hint()) {
		ProjectSettings::get_singleton()->save();
	}
	emit_signal("tags_changed");
}

void AbilitySystem::register_tag(const StringName &p_tag, ASTagType p_type, uint64_t p_owner_id) {
	if (p_tag == StringName()) {
		return;
	}

	bool changed = false;
	if (!registered_tags.has(p_tag)) {
		registered_tags[p_tag] = p_type;
		changed = true;
	} else if (p_owner_id == 0 && registered_tags[p_tag] != p_type) {
		// Re-registration from UI (no owner specified) usually means changing type
		registered_tags[p_tag] = p_type;
		changed = true;
	}

	if (p_owner_id != 0 && (!tag_owners.has(p_tag) || tag_owners[p_tag] != p_owner_id)) {
		tag_owners[p_tag] = p_owner_id;
		// Owner changes don't necessarily require settings update, but they change runtime state
	}

	if (changed) {
		_update_settings();
	}
}

void AbilitySystem::rename_tag(const StringName &p_old_tag, const StringName &p_new_tag) {
	if (p_old_tag == p_new_tag || p_new_tag == StringName()) {
		return;
	}

	bool changed = false;

	// Rename the exact tag if it exists
	if (registered_tags.has(p_old_tag)) {
		ASTagType t = registered_tags[p_old_tag];
		uint64_t owner = tag_owners.has(p_old_tag) ? tag_owners[p_old_tag] : 0;

		registered_tags.erase(p_old_tag);
		tag_owners.erase(p_old_tag);

		registered_tags[p_new_tag] = t;
		if (owner != 0) {
			tag_owners[p_new_tag] = owner;
		}
		changed = true;
	}

	// Rename any hierarchical sub-tags
	String prefix = String(p_old_tag) + ".";
	TypedArray<StringName> to_rename;
	for (const KeyValue<StringName, ASTagType> &E : registered_tags) {
		if (String(E.key).begins_with(prefix)) {
			to_rename.push_back(E.key);
		}
	}

	for (int i = 0; i < to_rename.size(); i++) {
		StringName old_child = to_rename[i];
		String old_child_str = old_child;
		String new_child_str = String(p_new_tag) + old_child_str.substr(String(p_old_tag).length());
		StringName new_child = new_child_str;

		ASTagType t = registered_tags[old_child];
		uint64_t owner = tag_owners.has(old_child) ? tag_owners[old_child] : 0;

		registered_tags.erase(old_child);
		tag_owners.erase(old_child);

		registered_tags[new_child] = t;
		if (owner != 0) {
			tag_owners[new_child] = owner;
		}
		changed = true;
	}

	if (changed) {
		_update_settings();
	}
}

bool AbilitySystem::is_tag_registered(const StringName &p_tag) const {
	return registered_tags.has(p_tag);
}

void AbilitySystem::unregister_tag(const StringName &p_tag) {
	if (registered_tags.has(p_tag)) {
		registered_tags.erase(p_tag);
		tag_owners.erase(p_tag);
		_update_settings();
	}
}

void AbilitySystem::remove_tag_branch(const StringName &p_tag) {
	bool changed = false;

	if (registered_tags.has(p_tag)) {
		registered_tags.erase(p_tag);
		tag_owners.erase(p_tag);
		changed = true;
	}

	String prefix = String(p_tag) + ".";
	TypedArray<StringName> to_remove;
	for (const KeyValue<StringName, ASTagType> &E : registered_tags) {
		if (String(E.key).begins_with(prefix)) {
			to_remove.push_back(E.key);
		}
	}

	for (int i = 0; i < to_remove.size(); ++i) {
		registered_tags.erase(to_remove[i]);
		tag_owners.erase(to_remove[i]);
		changed = true;
	}

	if (changed) {
		_update_settings();
	}
}

uint64_t AbilitySystem::get_tag_owner(const StringName &p_tag) const {
	if (tag_owners.has(p_tag)) {
		return tag_owners[p_tag];
	}
	return 0;
}

ASTagType AbilitySystem::get_tag_type(const StringName &p_tag) const {
	if (registered_tags.has(p_tag)) {
		return registered_tags[p_tag];
	}
	return (ASTagType)ASTagType::NAME;
}

TypedArray<StringName> AbilitySystem::get_registered_tags() const {
	TypedArray<StringName> res;
	for (const KeyValue<StringName, ASTagType> &E : registered_tags) {
		res.push_back(E.key);
	}
	return res;
}

TypedArray<StringName> AbilitySystem::get_registered_tags_of_type(ASTagType p_type) const {
	TypedArray<StringName> res;
	for (const KeyValue<StringName, ASTagType> &E : registered_tags) {
		if (E.value == p_type) {
			res.push_back(E.key);
		}
	}
	return res;
}

bool AbilitySystem::register_resource_name(const String &p_name, uint64_t p_owner_id) {
	if (p_name.is_empty()) {
		return true;
	}

	if (resource_names.has(p_name)) {
		if (resource_names[p_name] == p_owner_id) {
			return true;
		}
		return false;
	}

	resource_names[p_name] = p_owner_id;
	return true;
}

void AbilitySystem::unregister_resource_name(const String &p_name) {
	resource_names.erase(p_name);
}

uint64_t AbilitySystem::get_resource_name_owner(const String &p_name) const {
	if (resource_names.has(p_name)) {
		return resource_names[p_name];
	}
	return 0;
}

bool AbilitySystem::tag_matches(const StringName &p_tag, const StringName &p_match_against, bool p_exact) {
	if (p_tag == p_match_against) {
		return true;
	}

	if (p_exact) {
		return false;
	}

	String s_tag = p_tag;
	String s_match = p_match_against;

	if (s_tag.begins_with(s_match + ".")) {
		return true;
	}

	return false;
}

#ifdef ABILITY_SYSTEM_GDEXTENSION
extern int run_gdextension_tests();
#endif

int AbilitySystem::run_tests() {
#ifdef ABILITY_SYSTEM_GDEXTENSION
	return run_gdextension_tests();
#else
	// In Module mode, tests are executed natively by Godot Engine via --test flag.
	return 0;
#endif
}

AbilitySystem::AbilitySystem() {
	singleton = this;
	_load_settings();
}

AbilitySystem::~AbilitySystem() {
	if (singleton == this) {
		singleton = nullptr;
	}
}
