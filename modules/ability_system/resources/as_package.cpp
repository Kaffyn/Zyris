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
#else
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

	ADD_GROUP("Effects", "effects_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "effects_resources", PROPERTY_HINT_ARRAY_TYPE, "ASEffect"), "set_effects", "get_effects");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "effects_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_effect_tags", "get_effect_tags");

	ADD_GROUP("Cues", "cues_");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues_resources", PROPERTY_HINT_ARRAY_TYPE, "ASCue"), "set_cues", "get_cues");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_cue_tags", "get_cue_tags");
}

void ASPackage::add_effect(const Ref<ASEffect> &p_effect) {
	effects.append(p_effect);
}

void ASPackage::remove_effect(const Ref<ASEffect> &p_effect) {
	int idx = effects.find(p_effect);
	if (idx != -1) {
		effects.remove_at(idx);
	}
}

void ASPackage::clear_effects() {
	effects.clear();
}

void ASPackage::add_effect_tag(const StringName &p_tag) {
	effect_tags.append(p_tag);
}

void ASPackage::remove_effect_tag(const StringName &p_tag) {
	int idx = effect_tags.find(p_tag);
	if (idx != -1) {
		effect_tags.remove_at(idx);
	}
}

void ASPackage::add_cue(const Ref<ASCue> &p_cue) {
	cues.append(p_cue);
}

void ASPackage::remove_cue(const Ref<ASCue> &p_cue) {
	int idx = cues.find(p_cue);
	if (idx != -1) {
		cues.remove_at(idx);
	}
}

void ASPackage::clear_cues() {
	cues.clear();
}

void ASPackage::add_cue_tag(const StringName &p_tag) {
	cue_tags.append(p_tag);
}

void ASPackage::remove_cue_tag(const StringName &p_tag) {
	int idx = cue_tags.find(p_tag);
	if (idx != -1) {
		cue_tags.remove_at(idx);
	}
}

ASPackage::ASPackage() {
}

ASPackage::~ASPackage() {
}
