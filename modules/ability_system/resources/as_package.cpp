/**************************************************************************/
/*  as_package.cpp                                                        */
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
#include "src/resources/as_package.h"
#include "src/core/ability_system.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/resources/as_package.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASPackage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_effects", "effects"), &ASPackage::set_effects);
	ClassDB::bind_method(D_METHOD("get_effects"), &ASPackage::get_effects);
	ClassDB::bind_method(D_METHOD("add_effect", "effect"), &ASPackage::add_effect);
	ClassDB::bind_method(D_METHOD("remove_effect", "effect"), &ASPackage::remove_effect);
	ClassDB::bind_method(D_METHOD("clear_effects"), &ASPackage::clear_effects);

	ClassDB::bind_method(D_METHOD("set_effect_tags", "tags"), &ASPackage::set_effect_tags);
	ClassDB::bind_method(D_METHOD("get_effect_tags"), &ASPackage::get_effect_tags);
	ClassDB::bind_method(D_METHOD("add_effect_tag", "tag"), &ASPackage::add_effect_tag);
	ClassDB::bind_method(D_METHOD("remove_effect_tag", "tag"), &ASPackage::remove_effect_tag);

	ClassDB::bind_method(D_METHOD("set_cues", "cues"), &ASPackage::set_cues);
	ClassDB::bind_method(D_METHOD("get_cues"), &ASPackage::get_cues);
	ClassDB::bind_method(D_METHOD("add_cue", "cue"), &ASPackage::add_cue);
	ClassDB::bind_method(D_METHOD("remove_cue", "cue"), &ASPackage::remove_cue);
	ClassDB::bind_method(D_METHOD("clear_cues"), &ASPackage::clear_cues);

	ClassDB::bind_method(D_METHOD("set_cue_tags", "tags"), &ASPackage::set_cue_tags);
	ClassDB::bind_method(D_METHOD("get_cue_tags"), &ASPackage::get_cue_tags);
	ClassDB::bind_method(D_METHOD("add_cue_tag", "tag"), &ASPackage::add_cue_tag);
	ClassDB::bind_method(D_METHOD("remove_cue_tag", "tag"), &ASPackage::remove_cue_tag);

	ClassDB::bind_method(D_METHOD("set_events_on_deliver", "events"), &ASPackage::set_events_on_deliver);
	ClassDB::bind_method(D_METHOD("get_events_on_deliver"), &ASPackage::get_events_on_deliver);

	ClassDB::bind_method(D_METHOD("set_package_tag", "tag"), &ASPackage::set_package_tag);
	ClassDB::bind_method(D_METHOD("get_package_tag"), &ASPackage::get_package_tag);

	ADD_GROUP("Identity", "package_");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "package_tag"), "set_package_tag", "get_package_tag");

	ADD_GROUP("Effects", "effects_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "effects_resources", PROPERTY_HINT_ARRAY_TYPE, "ASEffect"), "set_effects", "get_effects");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "effects_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_effect_tags", "get_effect_tags");

	ADD_GROUP("Cues", "cues_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues_resources", PROPERTY_HINT_ARRAY_TYPE, "ASCue"), "set_cues", "get_cues");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_cue_tags", "get_cue_tags");

	ADD_GROUP("Events", "events_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events_on_deliver", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_events_on_deliver", "get_events_on_deliver");
}

void ASPackage::set_effects(const TypedArray<ASEffect> &p_effects) {
	effects = p_effects;
}

TypedArray<ASEffect> ASPackage::get_effects() const {
	return effects;
}

void ASPackage::add_effect(const Ref<ASEffect> &p_effect) {
	effects.append(p_effect);
}

void ASPackage::remove_effect(const Ref<ASEffect> &p_effect) {
	effects.erase(p_effect);
}

void ASPackage::clear_effects() {
	effects.clear();
}

void ASPackage::set_effect_tags(const TypedArray<StringName> &p_tags) {
	effect_tags = p_tags;
}

TypedArray<StringName> ASPackage::get_effect_tags() const {
	return effect_tags;
}

void ASPackage::add_effect_tag(const StringName &p_tag) {
	effect_tags.append(p_tag);
}

void ASPackage::remove_effect_tag(const StringName &p_tag) {
	effect_tags.erase(p_tag);
}

void ASPackage::set_cues(const TypedArray<ASCue> &p_cues) {
	cues = p_cues;
}

TypedArray<ASCue> ASPackage::get_cues() const {
	return cues;
}

void ASPackage::add_cue(const Ref<ASCue> &p_cue) {
	cues.append(p_cue);
}

void ASPackage::remove_cue(const Ref<ASCue> &p_cue) {
	cues.erase(p_cue);
}

void ASPackage::clear_cues() {
	cues.clear();
}

void ASPackage::set_cue_tags(const TypedArray<StringName> &p_tags) {
	cue_tags = p_tags;
}

TypedArray<StringName> ASPackage::get_cue_tags() const {
	return cue_tags;
}

void ASPackage::add_cue_tag(const StringName &p_tag) {
	cue_tags.append(p_tag);
}

void ASPackage::remove_cue_tag(const StringName &p_tag) {
	cue_tags.erase(p_tag);
}

void ASPackage::set_events_on_deliver(const TypedArray<StringName> &p_events) {
	events_on_deliver = p_events;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_events.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_events[i], (ASTagType)AbilitySystem::TAG_TYPE_EVENT);
		}
	}
}

TypedArray<StringName> ASPackage::get_events_on_deliver() const {
	return events_on_deliver;
}

void ASPackage::set_package_tag(const StringName &p_tag) {
	package_tag = p_tag;
}

StringName ASPackage::get_package_tag() const {
	return package_tag;
}

ASPackage::ASPackage() {
}

ASPackage::~ASPackage() {
}
