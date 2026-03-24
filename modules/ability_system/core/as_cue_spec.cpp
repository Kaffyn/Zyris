/**************************************************************************/
/*  as_cue_spec.cpp                                                       */
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
#include "src/core/as_cue_spec.h"
#include "src/core/as_effect_spec.h"
#include "src/resources/as_cue.h"
#include "src/scene/as_component.h"
#else
#include "modules/ability_system/core/as_cue_spec.h"
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/scene/as_component.h"
#endif

namespace godot {

void ASCueSpec::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_cue", "cue"), &ASCueSpec::set_cue);
	ClassDB::bind_method(D_METHOD("get_cue"), &ASCueSpec::get_cue);

	ClassDB::bind_method(D_METHOD("set_effect_spec", "spec"), &ASCueSpec::set_effect_spec);
	ClassDB::bind_method(D_METHOD("get_effect_spec"), &ASCueSpec::get_effect_spec);

	ClassDB::bind_method(D_METHOD("set_magnitude", "magnitude"), &ASCueSpec::set_magnitude);
	ClassDB::bind_method(D_METHOD("get_magnitude"), &ASCueSpec::get_magnitude);

	ClassDB::bind_method(D_METHOD("set_source_asc", "asc"), &ASCueSpec::set_source_asc);
	ClassDB::bind_method(D_METHOD("get_source_asc"), &ASCueSpec::get_source_asc);
	ClassDB::bind_method(D_METHOD("set_target_asc", "asc"), &ASCueSpec::set_target_asc);
	ClassDB::bind_method(D_METHOD("get_target_asc"), &ASCueSpec::get_target_asc);

	ClassDB::bind_method(D_METHOD("set_extra_data", "data"), &ASCueSpec::set_extra_data);
	ClassDB::bind_method(D_METHOD("get_extra_data"), &ASCueSpec::get_extra_data);

	ClassDB::bind_method(D_METHOD("set_level", "level"), &ASCueSpec::set_level);
	ClassDB::bind_method(D_METHOD("get_level"), &ASCueSpec::get_level);

	ClassDB::bind_method(D_METHOD("set_target_node", "node"), &ASCueSpec::set_target_node);
	ClassDB::bind_method(D_METHOD("get_target_node"), &ASCueSpec::get_target_node);
	ClassDB::bind_method(D_METHOD("set_hit_position", "position"), &ASCueSpec::set_hit_position);
	ClassDB::bind_method(D_METHOD("get_hit_position"), &ASCueSpec::get_hit_position);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "cue", PROPERTY_HINT_RESOURCE_TYPE, "ASCue"), "set_cue", "get_cue");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "effect_spec", PROPERTY_HINT_NONE, "ASEffectSpec"), "set_effect_spec", "get_effect_spec");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "magnitude"), "set_magnitude", "get_magnitude");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "extra_data"), "set_extra_data", "get_extra_data");
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "hit_position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_NIL_IS_VARIANT), "set_hit_position", "get_hit_position");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "target_node"), "set_target_node", "get_target_node");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source_asc", PROPERTY_HINT_NONE, "ASComponent"), "set_source_asc", "get_source_asc");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "target_asc", PROPERTY_HINT_NONE, "ASComponent"), "set_target_asc", "get_target_asc");
}

void ASCueSpec::init_from_effect(
		Ref<ASCue> p_cue,
		ASComponent *p_source,
		ASComponent *p_target,
		Ref<ASEffectSpec> p_effect_spec,
		float p_magnitude) {
	cue = p_cue;
	set_source_asc(p_source);
	set_target_asc(p_target);
	effect_spec = p_effect_spec;
	magnitude = p_magnitude;

	if (p_effect_spec.is_valid()) {
		set_hit_position(p_effect_spec->get_hit_position());
		set_target_node(p_effect_spec->get_target_node());
		set_level(p_effect_spec->get_level());
	}
}

void ASCueSpec::init_manual(
		Ref<ASCue> p_cue,
		ASComponent *p_owner,
		const Dictionary &p_extra_data) {
	cue = p_cue;
	// For a manual call, the owner is both source and target.
	set_source_asc(p_owner);
	set_target_asc(p_owner);
	extra_data = p_extra_data;
}

void ASCueSpec::set_source_asc(ASComponent *p_asc) {
	if (p_asc) {
		source_id = p_asc->get_instance_id();
	} else {
		source_id = ObjectID();
	}
}

ASComponent *ASCueSpec::get_source_asc() const {
	if (source_id.is_null()) {
		return nullptr;
	}
	return Object::cast_to<ASComponent>(ObjectDB::get_instance(source_id));
}

void ASCueSpec::set_target_asc(ASComponent *p_asc) {
	if (p_asc) {
		target_id = p_asc->get_instance_id();
	} else {
		target_id = ObjectID();
	}
}

ASComponent *ASCueSpec::get_target_asc() const {
	if (target_id.is_null()) {
		return nullptr;
	}
	return Object::cast_to<ASComponent>(ObjectDB::get_instance(target_id));
}

void ASCueSpec::set_target_node(Object *p_node) {
	if (p_node) {
		target_node_id = p_node->get_instance_id();
	} else {
		target_node_id = ObjectID();
	}
}

Object *ASCueSpec::get_target_node() const {
	if (target_node_id.is_null()) {
		return nullptr;
	}
	return ObjectDB::get_instance(target_node_id);
}

void ASCueSpec::set_cue(Ref<ASCue> p_cue) {
	cue = p_cue;
}

Ref<ASCue> ASCueSpec::get_cue() const {
	return Ref<ASCue>(cue);
}

void ASCueSpec::set_effect_spec(Ref<ASEffectSpec> p_spec) {
	effect_spec = p_spec;
}

Ref<ASEffectSpec> ASCueSpec::get_effect_spec() const {
	return Ref<ASEffectSpec>(effect_spec);
}

ASCueSpec::ASCueSpec() {
}

ASCueSpec::~ASCueSpec() {
}
} // namespace godot
