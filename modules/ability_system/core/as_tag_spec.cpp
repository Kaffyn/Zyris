/**************************************************************************/
/*  as_tag_spec.cpp                                                       */
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
#include "src/core/as_tag_spec.h"
#include "src/core/ability_system.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_tag_spec.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASTagSpec::_bind_methods() {
	ClassDB::bind_method(D_METHOD("has_tag", "tag", "exact"), &ASTagSpec::has_tag, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("has_any_tags", "tags", "exact"), &ASTagSpec::has_any_tags, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("has_all_tags", "tags", "exact"), &ASTagSpec::has_all_tags, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("add_tag", "tag"), &ASTagSpec::add_tag);
	ClassDB::bind_method(D_METHOD("remove_tag", "tag"), &ASTagSpec::remove_tag);
	ClassDB::bind_method(D_METHOD("clear"), &ASTagSpec::clear);
	ClassDB::bind_method(D_METHOD("get_all_tags"), &ASTagSpec::get_all_tags);
}

bool ASTagSpec::has_tag(const StringName &p_tag, bool p_exact) const {
	if (p_exact) {
		return tags.has(p_tag);
	}

	for (const KeyValue<StringName, int> &E : tags) {
		if (AbilitySystem::tag_matches(E.key, p_tag, false)) {
			return true;
		}
	}
	return false;
}

bool ASTagSpec::has_any_tags(const TypedArray<StringName> &p_tags, bool p_exact) const {
	for (int i = 0; i < p_tags.size(); i++) {
		if (has_tag(p_tags[i], p_exact)) {
			return true;
		}
	}
	return false;
}

bool ASTagSpec::has_all_tags(const TypedArray<StringName> &p_tags, bool p_exact) const {
	for (int i = 0; i < p_tags.size(); i++) {
		if (!has_tag(p_tags[i], p_exact)) {
			return false;
		}
	}
	return true;
}

bool ASTagSpec::add_tag(const StringName &p_tag) {
	if (p_tag == StringName()) {
		return false;
	}

	// Register with global system if available
	if (AbilitySystem::get_singleton()) {
		AbilitySystem::get_singleton()->register_tag(p_tag, (ASTagType)AbilitySystem::TAG_TYPE_CONDITIONAL);
	}

	if (tags.has(p_tag)) {
		tags[p_tag]++;
		return false;
	} else {
		tags[p_tag] = 1;
		return true;
	}
}

bool ASTagSpec::remove_tag(const StringName &p_tag) {
	if (tags.has(p_tag)) {
		tags[p_tag]--;
		if (tags[p_tag] <= 0) {
			tags.erase(p_tag);
			return true;
		}
	}
	return false;
}

bool ASTagSpec::remove_tag_fully(const StringName &p_tag) {
	if (tags.has(p_tag)) {
		tags.erase(p_tag);
		return true;
	}
	return false;
}

void ASTagSpec::clear() {
	tags.clear();
}

TypedArray<StringName> ASTagSpec::get_all_tags() const {
	TypedArray<StringName> res;
	for (const KeyValue<StringName, int> &E : tags) {
		res.push_back(E.key);
	}
	return res;
}

ASTagSpec::ASTagSpec() {
}

ASTagSpec::~ASTagSpec() {
}
