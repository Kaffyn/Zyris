/**************************************************************************/
/*  as_effect_spec.cpp                                                    */
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
#include "src/core/as_effect_spec.h"
#include "src/core/as_tag_spec.h"
#include "src/resources/as_effect.h"
#include "src/scene/as_component.h"
#include <godot_cpp/core/class_db.hpp>
#else
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/core/as_tag_spec.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/scene/as_component.h"
#endif

namespace godot {

void ASEffectSpec::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init", "effect", "level"), &ASEffectSpec::init, DEFVAL(1.0f));
	ClassDB::bind_method(D_METHOD("get_effect"), &ASEffectSpec::get_effect);
	ClassDB::bind_method(D_METHOD("get_duration_remaining"), &ASEffectSpec::get_duration_remaining);
	ClassDB::bind_method(D_METHOD("set_duration_remaining", "value"), &ASEffectSpec::set_duration_remaining);
	ClassDB::bind_method(D_METHOD("get_total_duration"), &ASEffectSpec::get_total_duration);
	ClassDB::bind_method(D_METHOD("set_total_duration", "value"), &ASEffectSpec::set_total_duration);
	ClassDB::bind_method(D_METHOD("set_magnitude", "name", "value"), &ASEffectSpec::set_magnitude);
	ClassDB::bind_method(D_METHOD("get_magnitude", "name"), &ASEffectSpec::get_magnitude);
	ClassDB::bind_method(D_METHOD("get_level"), &ASEffectSpec::get_level);
	ClassDB::bind_method(D_METHOD("set_level", "level"), &ASEffectSpec::set_level);
	ClassDB::bind_method(D_METHOD("get_source_component"), &ASEffectSpec::get_source_component);
	ClassDB::bind_method(D_METHOD("get_target_component"), &ASEffectSpec::get_target_component);

	ClassDB::bind_method(D_METHOD("get_stack_count"), &ASEffectSpec::get_stack_count);
	ClassDB::bind_method(D_METHOD("set_stack_count", "count"), &ASEffectSpec::set_stack_count);
	ClassDB::bind_method(D_METHOD("get_period_timer"), &ASEffectSpec::get_period_timer);
	ClassDB::bind_method(D_METHOD("set_period_timer", "timer"), &ASEffectSpec::set_period_timer);

	ClassDB::bind_method(D_METHOD("get_source_attribute_value", "attribute"), &ASEffectSpec::get_source_attribute_value);
	ClassDB::bind_method(D_METHOD("get_target_attribute_value", "attribute"), &ASEffectSpec::get_target_attribute_value);

	ClassDB::bind_method(D_METHOD("set_target_node", "node"), &ASEffectSpec::set_target_node);
	ClassDB::bind_method(D_METHOD("get_target_node"), &ASEffectSpec::get_target_node);
	ClassDB::bind_method(D_METHOD("set_hit_position", "position"), &ASEffectSpec::set_hit_position);
	ClassDB::bind_method(D_METHOD("get_hit_position"), &ASEffectSpec::get_hit_position);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "hit_position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_NIL_IS_VARIANT), "set_hit_position", "get_hit_position");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "target_node"), "set_target_node", "get_target_node");
}

void ASEffectSpec::init(godot::Ref<godot::ASEffect> p_effect, float p_lvl) {
	effect = p_effect;
	level = p_lvl;
	if (effect.is_valid()) {
		if (effect->get_duration_policy() == ASEffect::POLICY_DURATION) {
			total_duration = effect->get_duration_magnitude();
			duration_remaining = total_duration;
		}
	}
}

void ASEffectSpec::set_source_component(ASComponent *p_comp) {
	if (p_comp) {
		source_id = p_comp->get_instance_id();
	} else {
		source_id = ObjectID();
	}
}

ASComponent *ASEffectSpec::get_source_component() const {
	if (source_id.is_null()) {
		return nullptr;
	}
	return Object::cast_to<ASComponent>(ObjectDB::get_instance(source_id));
}

void ASEffectSpec::set_target_component(ASComponent *p_comp) {
	if (p_comp) {
		target_id = p_comp->get_instance_id();
	} else {
		target_id = ObjectID();
	}
}

ASComponent *ASEffectSpec::get_target_component() const {
	if (target_id.is_null()) {
		return nullptr;
	}
	return Object::cast_to<ASComponent>(ObjectDB::get_instance(target_id));
}

void ASEffectSpec::set_target_node(Object *p_node) {
	if (p_node) {
		target_node_id = p_node->get_instance_id();
	} else {
		target_node_id = ObjectID();
	}
}

Object *ASEffectSpec::get_target_node() const {
	if (target_node_id.is_null()) {
		return nullptr;
	}
	return ObjectDB::get_instance(target_node_id);
}

float ASEffectSpec::get_source_attribute_value(const StringName &p_attribute) const {
	ASComponent *source = get_source_component();
	if (source) {
		return source->get_attribute_value_by_tag(p_attribute);
	}
	return 0.0f;
}

float ASEffectSpec::get_target_attribute_value(const StringName &p_attribute) const {
	ASComponent *target = get_target_component();
	if (target) {
		return target->get_attribute_value_by_tag(p_attribute);
	}
	return 0.0f;
}

ASEffectSpec::ASEffectSpec() {
}

ASEffectSpec::~ASEffectSpec() {
}

} // namespace godot
