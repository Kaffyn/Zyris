/**************************************************************************/
/*  as_state_snapshot.cpp                                                 */
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

#include "as_state_snapshot.h"
#ifdef ABILITY_SYSTEM_GDEXTENSION
#include "src/core/as_tag_spec.h"
#include "src/resources/as_attribute.h"
#include "src/resources/as_attribute_set.h"
#include "src/resources/as_container.h"
#include "src/resources/as_effect.h"
#include "src/scene/as_component.h"
#include <godot_cpp/core/class_db.hpp>
#else
#include "modules/ability_system/core/as_tag_spec.h"
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/resources/as_container.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/scene/as_component.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASStateSnapshot::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_tick", "tick"), &ASStateSnapshot::set_tick);
	ClassDB::bind_method(D_METHOD("get_tick"), &ASStateSnapshot::get_tick);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "tick"), "set_tick", "get_tick");

	ClassDB::bind_method(D_METHOD("set_attributes", "attributes"), &ASStateSnapshot::set_attributes);
	ClassDB::bind_method(D_METHOD("get_attributes"), &ASStateSnapshot::get_attributes);
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "attributes"), "set_attributes", "get_attributes");

	ClassDB::bind_method(D_METHOD("set_tags", "tags"), &ASStateSnapshot::set_tags);
	ClassDB::bind_method(D_METHOD("get_tags"), &ASStateSnapshot::get_tags);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "tags"), "set_tags", "get_tags");

	ClassDB::bind_method(D_METHOD("set_active_effects", "active_effects"), &ASStateSnapshot::set_active_effects);
	ClassDB::bind_method(D_METHOD("get_active_effects"), &ASStateSnapshot::get_active_effects);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "active_effects"), "set_active_effects", "get_active_effects");

	ClassDB::bind_method(D_METHOD("capture_from_component", "component"), &ASStateSnapshot::capture_from_component);
	ClassDB::bind_method(D_METHOD("apply_to_component", "component"), &ASStateSnapshot::apply_to_component);
}

void ASStateSnapshot::capture_from_component(ASComponent *p_component) {
	ERR_FAIL_NULL(p_component);

	attributes.clear();
	tags.clear();

	// Capture Attributes
	TypedArray<ASAttributeSet> sets = p_component->get_attribute_sets();
	for (int i = 0; i < sets.size(); i++) {
		Ref<ASAttributeSet> s = sets[i];
		if (s.is_null()) {
			continue;
		}

		TypedArray<ASAttribute> defs = s->get_attribute_definitions();
		for (int j = 0; j < defs.size(); j++) {
			Ref<ASAttribute> attr = defs[j];
			if (attr.is_valid()) {
				StringName attr_name = attr->get_attribute_name();
				attributes[attr_name] = s->get_attribute_base_value(attr_name);
			}
		}
	}

	// Capture Tags
	TypedArray<StringName> owned = p_component->get_tags();
	for (int i = 0; i < owned.size(); i++) {
		tags.push_back(owned[i]);
	}

	// Capture Effects
	active_effects.clear();
	const Vector<Ref<ASEffectSpec>> &current_effects = p_component->active_effects;
	for (int i = 0; i < current_effects.size(); i++) {
		Ref<ASEffectSpec> spec = current_effects[i];
		if (spec.is_null()) {
			continue;
		}
		Ref<ASEffect> effect = spec->get_effect();
		if (effect.is_null()) {
			continue;
		}

		Dictionary es;
		es["tag"] = effect->get_effect_tag();
		es["remaining_time"] = spec->get_duration_remaining();
		es["period_timer"] = spec->get_period_timer();
		es["stack_count"] = spec->get_stack_count();
		es["level"] = spec->get_level();
		active_effects.push_back(es);
	}
}

void ASStateSnapshot::apply_to_component(ASComponent *p_component) const {
	ERR_FAIL_NULL(p_component);

	// Apply Attributes
	Array keys = attributes.keys();
	for (int i = 0; i < keys.size(); i++) {
		StringName attr_name = keys[i];
		float value = attributes[attr_name];
		// set_attribute_base_value_by_tag is fine as it respects _is_rolling_back for triggers
		p_component->set_attribute_base_value_by_tag(attr_name, value);
	}

	// Apply Tags (Manual to be silent)
	if (p_component->owned_tags.is_valid()) {
		p_component->owned_tags->clear();
		for (int i = 0; i < tags.size(); i++) {
			p_component->owned_tags->add_tag(tags[i]);
		}
	}

	// Apply Effects (Silent clear and re-instantiate)
	p_component->active_effects.clear();
	for (int i = 0; i < active_effects.size(); i++) {
		Dictionary es = active_effects[i];
		StringName tag = es["tag"];
		Ref<ASEffect> e = p_component->find_effect_by_tag(tag);
		if (e.is_valid()) {
			Ref<ASEffectSpec> spec;
			spec.instantiate();
			spec->init(e, es["level"]);
			spec->set_duration_remaining(es["remaining_time"]);
			spec->set_period_timer(es["period_timer"]);
			spec->set_stack_count(es["stack_count"]);
			spec->set_target_component(p_component);
			p_component->active_effects.push_back(spec);
		}
	}
}

Ref<ASStateSnapshot> ASStateSnapshot::compute_diff(const Ref<ASStateSnapshot> &p_other) const {
	// Future implementation for bandwidth optimization
	return Ref<ASStateSnapshot>();
}

PackedByteArray ASStateSnapshot::serialize_binary() const {
	// Future implementation for binary efficient replication
	return PackedByteArray();
}

void ASStateSnapshot::deserialize_binary(const PackedByteArray &p_data) {
	// Future implementation
}

ASStateSnapshot::ASStateSnapshot() {}
ASStateSnapshot::~ASStateSnapshot() {}
