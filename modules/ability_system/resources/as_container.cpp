/**************************************************************************/
/*  as_container.cpp                                                      */
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
#include "src/resources/as_container.h"
#include "src/core/ability_system.h"
#include "src/resources/as_ability.h"
#include "src/resources/as_attribute_set.h"
#include "src/resources/as_cue.h"
#include "src/resources/as_effect.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/resources/as_container.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASContainer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_abilities", "abilities"), &ASContainer::set_abilities);
	ClassDB::bind_method(D_METHOD("get_abilities"), &ASContainer::get_abilities);

	ClassDB::bind_method(D_METHOD("set_attribute_set", "attribute_set"), &ASContainer::set_attribute_set);
	ClassDB::bind_method(D_METHOD("get_attribute_set"), &ASContainer::get_attribute_set);

	ClassDB::bind_method(D_METHOD("set_effects", "effects"), &ASContainer::set_effects);
	ClassDB::bind_method(D_METHOD("get_effects"), &ASContainer::get_effects);

	ClassDB::bind_method(D_METHOD("set_cues", "cues"), &ASContainer::set_cues);
	ClassDB::bind_method(D_METHOD("get_cues"), &ASContainer::get_cues);

	ClassDB::bind_method(D_METHOD("set_events", "events"), &ASContainer::set_events);
	ClassDB::bind_method(D_METHOD("get_events"), &ASContainer::get_events);

	ClassDB::bind_method(D_METHOD("has_ability", "ability"), &ASContainer::has_ability);
	ClassDB::bind_method(D_METHOD("has_effect", "effect"), &ASContainer::has_effect);
	ClassDB::bind_method(D_METHOD("has_cue", "tag"), &ASContainer::has_cue);
	ClassDB::bind_method(D_METHOD("has_cue_resource", "cue"), &ASContainer::has_cue_resource);

	ClassDB::bind_method(D_METHOD("add_ability", "ability"), &ASContainer::add_ability);
	ClassDB::bind_method(D_METHOD("add_effect", "effect"), &ASContainer::add_effect);
	ClassDB::bind_method(D_METHOD("add_cue", "cue"), &ASContainer::add_cue);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "abilities", PROPERTY_HINT_ARRAY_TYPE, "ASAbility"), "set_abilities", "get_abilities");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "attribute_set", PROPERTY_HINT_RESOURCE_TYPE, "ASAttributeSet"), "set_attribute_set", "get_attribute_set");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "effects", PROPERTY_HINT_ARRAY_TYPE, "ASEffect"), "set_effects", "get_effects");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues", PROPERTY_HINT_ARRAY_TYPE, "ASCue"), "set_cues", "get_cues");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_events", "get_events");
}

void ASContainer::set_abilities(const TypedArray<ASAbility> &p_abilities) {
	abilities = p_abilities;
}

TypedArray<ASAbility> ASContainer::get_abilities() const {
	return abilities;
}

void ASContainer::set_effects(const TypedArray<ASEffect> &p_effects) {
	effects = p_effects;
}

TypedArray<ASEffect> ASContainer::get_effects() const {
	return effects;
}

void ASContainer::set_attribute_set(const Ref<ASAttributeSet> &p_set) {
	attribute_set = p_set;
}

Ref<ASAttributeSet> ASContainer::get_attribute_set() const {
	return attribute_set;
}

void ASContainer::set_cues(const TypedArray<ASCue> &p_cues) {
	cues = p_cues;
}

TypedArray<ASCue> ASContainer::get_cues() const {
	return cues;
}

void ASContainer::set_events(const TypedArray<StringName> &p_events) {
	events = p_events;
	if (AbilitySystem::get_singleton()) {
		for (int i = 0; i < p_events.size(); i++) {
			AbilitySystem::get_singleton()->register_tag(p_events[i], (ASTagType)AbilitySystem::TAG_TYPE_EVENT);
		}
	}
}

TypedArray<StringName> ASContainer::get_events() const {
	return events;
}

bool ASContainer::has_ability(const Ref<ASAbility> &p_ability) const {
	for (int i = 0; i < abilities.size(); i++) {
		if (abilities[i] == p_ability) {
			return true;
		}
	}
	// Check AttributeSet unlocked abilities as well
	if (attribute_set.is_valid()) {
		TypedArray<ASAbility> unlocked = attribute_set->get_unlocked_abilities();
		for (int i = 0; i < unlocked.size(); i++) {
			if (unlocked[i] == p_ability) {
				return true;
			}
		}
	}
	return false;
}

bool ASContainer::has_effect(const Ref<ASEffect> &p_effect) const {
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i] == p_effect) {
			return true;
		}
	}
	return false;
}

bool ASContainer::has_cue(const StringName &p_tag) const {
	for (int i = 0; i < cues.size(); i++) {
		Ref<ASCue> cue = cues[i];
		if (cue.is_valid() && cue->get_cue_tag() == p_tag) {
			return true;
		}
	}
	return false;
}

bool ASContainer::has_cue_resource(const Ref<ASCue> &p_cue) const {
	for (int i = 0; i < cues.size(); i++) {
		if (cues[i] == p_cue) {
			return true;
		}
	}
	return false;
}

void ASContainer::add_ability(const Ref<ASAbility> &p_ability) {
	if (p_ability.is_valid()) {
		abilities.push_back(p_ability);
	}
}

void ASContainer::add_effect(const Ref<ASEffect> &p_effect) {
	if (p_effect.is_valid()) {
		effects.push_back(p_effect);
	}
}

void ASContainer::add_cue(const Ref<ASCue> &p_cue) {
	if (p_cue.is_valid()) {
		cues.push_back(p_cue);
	}
}

ASContainer::ASContainer() {
}

ASContainer::~ASContainer() {
}
