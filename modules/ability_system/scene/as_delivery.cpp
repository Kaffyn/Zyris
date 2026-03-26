/**************************************************************************/
/*  as_delivery.cpp                                                       */
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
#include "src/scene/as_delivery.h"
#include "src/core/as_effect_spec.h"
#include "src/resources/as_container.h"
#include "src/resources/as_package.h"
#include "src/scene/as_component.h"
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/area3d.hpp>
#else
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/resources/as_container.h"
#include "modules/ability_system/resources/as_package.h"
#include "modules/ability_system/scene/as_component.h"
#include "modules/ability_system/scene/as_delivery.h"
#include "scene/2d/physics/area_2d.h"
#include "scene/3d/physics/area_3d.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASDelivery::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_source_component", "source"), &ASDelivery::set_source_component);
	ClassDB::bind_method(D_METHOD("get_source_component"), &ASDelivery::get_source_component);

	ClassDB::bind_method(D_METHOD("set_package", "package"), &ASDelivery::set_package);
	ClassDB::bind_method(D_METHOD("get_package"), &ASDelivery::get_package);

	ClassDB::bind_method(D_METHOD("set_level", "level"), &ASDelivery::set_level);
	ClassDB::bind_method(D_METHOD("get_level"), &ASDelivery::get_level);

	ClassDB::bind_method(D_METHOD("set_active", "active"), &ASDelivery::set_active);
	ClassDB::bind_method(D_METHOD("get_active"), &ASDelivery::get_active);

	ClassDB::bind_method(D_METHOD("set_life_span", "life_span"), &ASDelivery::set_life_span);
	ClassDB::bind_method(D_METHOD("get_life_span"), &ASDelivery::get_life_span);

	ClassDB::bind_method(D_METHOD("set_one_shot", "one_shot"), &ASDelivery::set_one_shot);
	ClassDB::bind_method(D_METHOD("get_one_shot"), &ASDelivery::get_one_shot);

	ClassDB::bind_method(D_METHOD("activate", "duration"), &ASDelivery::activate, DEFVAL(-1.0f));
	ClassDB::bind_method(D_METHOD("deactivate"), &ASDelivery::deactivate);
	ClassDB::bind_method(D_METHOD("is_delivery_valid"), &ASDelivery::is_delivery_valid);

	ClassDB::bind_method(D_METHOD("set_target_groups", "groups"), &ASDelivery::set_target_groups);
	ClassDB::bind_method(D_METHOD("get_target_groups"), &ASDelivery::get_target_groups);

	ClassDB::bind_method(D_METHOD("set_auto_connect", "auto_connect"), &ASDelivery::set_auto_connect);
	ClassDB::bind_method(D_METHOD("get_auto_connect"), &ASDelivery::get_auto_connect);

	ClassDB::bind_method(D_METHOD("deliver", "target"), &ASDelivery::deliver);
	ClassDB::bind_method(D_METHOD("can_deliver_to", "target"), &ASDelivery::can_deliver_to);

	ClassDB::bind_method(D_METHOD("_on_area_body_entered", "body"), &ASDelivery::_on_area_body_entered);
	ClassDB::bind_method(D_METHOD("_on_area_area_entered", "area"), &ASDelivery::_on_area_area_entered);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_active"), "set_active", "get_active");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source_component", PROPERTY_HINT_NODE_TYPE, "ASComponent"), "set_source_component", "get_source_component");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "package", PROPERTY_HINT_RESOURCE_TYPE, "ASPackage"), "set_package", "get_package");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "level"), "set_level", "get_level");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "life_span"), "set_life_span", "get_life_span");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "one_shot"), "set_one_shot", "get_one_shot");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "target_groups", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_target_groups", "get_target_groups");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_connect"), "set_auto_connect", "get_auto_connect");

	ADD_SIGNAL(MethodInfo("delivered", PropertyInfo(Variant::OBJECT, "target")));
	ADD_SIGNAL(MethodInfo("expired"));
}

void ASDelivery::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_POST_ENTER_TREE: {
			if (auto_connect) {
				_check_auto_connect();
			}
		} break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			if (is_active && life_span > 0.0f) {
				timer -= get_physics_process_delta_time();
				if (timer <= 0.0f) {
					deactivate();
					emit_signal("expired");
				}
			}
		} break;
		case NOTIFICATION_EXIT_TREE: {
			connected = false;
		} break;
	}
}

void ASDelivery::_check_auto_connect() {
	if (connected) {
		return;
	}

	Node *parent = get_parent();
	if (!parent) {
		return;
	}

	Area2D *a2d = Object::cast_to<Area2D>(parent);
	if (a2d) {
		a2d->connect("body_entered", Callable(this, "_on_area_body_entered"));
		a2d->connect("area_entered", Callable(this, "_on_area_area_entered"));
		connected = true;
		return;
	}

	Area3D *a3d = Object::cast_to<Area3D>(parent);
	if (a3d) {
		a3d->connect("body_entered", Callable(this, "_on_area_body_entered"));
		a3d->connect("area_entered", Callable(this, "_on_area_area_entered"));
		connected = true;
	}
}

void ASDelivery::_on_area_body_entered(Node *p_body) {
	deliver(p_body);
}

void ASDelivery::_on_area_area_entered(Node *p_area) {
	deliver(p_area);
}

void ASDelivery::activate(float p_duration) {
	if (p_duration > 0.0f) {
		life_span = p_duration;
	}
	timer = life_span;
	set_active(true);
}

void ASDelivery::deactivate() {
	set_active(false);
}

bool ASDelivery::is_delivery_valid() const {
	if (!is_active) {
		return false;
	}
	if (life_span > 0.0f && timer <= 0.0f) {
		return false;
	}
	return true;
}

bool ASDelivery::can_deliver_to(Node *p_target) const {
	if (!is_delivery_valid()) {
		return false;
	}
	if (!p_target) {
		return false;
	}

	if (target_groups.size() > 0) {
		bool in_group = false;
		for (int i = 0; i < target_groups.size(); i++) {
			if (p_target->is_in_group(target_groups[i])) {
				in_group = true;
				break;
			}
		}
		if (!in_group) {
			return false;
		}
	}

	return true;
}

void ASDelivery::deliver(Node *p_target) {
	if (!can_deliver_to(p_target)) {
		return;
	}

	ASComponent *target_asc = Object::cast_to<ASComponent>(p_target);
	if (!target_asc) {
		// Try children
		for (int i = 0; i < p_target->get_child_count(); i++) {
			target_asc = Object::cast_to<ASComponent>(p_target->get_child(i));
			if (target_asc) {
				break;
			}
		}
	}

	if (!target_asc) {
		// Try parent
		Node *parent = p_target->get_parent();
		if (parent) {
			target_asc = Object::cast_to<ASComponent>(parent);
			if (!target_asc) {
				for (int i = 0; i < parent->get_child_count(); i++) {
					target_asc = Object::cast_to<ASComponent>(parent->get_child(i));
					if (target_asc) {
						break;
					}
				}
			}
		}
	}

	if (!target_asc) {
		// Try owner
		Node *owner = p_target->get_owner();
		if (owner) {
			target_asc = Object::cast_to<ASComponent>(owner);
			if (!target_asc) {
				for (int i = 0; i < owner->get_child_count(); i++) {
					target_asc = Object::cast_to<ASComponent>(owner->get_child(i));
					if (target_asc) {
						break;
					}
				}
			}
		}
	}

	if (!target_asc) {
		return;
	}

	if (package.is_null()) {
		return;
	}

	// Deliver the package via the target component
	ASComponent *source_component = get_source_component();
	target_asc->apply_package(package, level, source_component);

	// Dispatch Historical Event
	StringName package_tag = package->get_package_tag();
	if (package_tag != StringName()) {
		target_asc->dispatch_event(package_tag, source_component, level);
	}

	// Dispatch Package Events
	TypedArray<StringName> deliver_events = package->get_events_on_deliver();
	for (int i = 0; i < deliver_events.size(); i++) {
		target_asc->dispatch_event(deliver_events[i], source_component, level);
	}

	emit_signal("delivered", p_target);

	if (one_shot) {
		deactivate();
	}
}

ASDelivery::ASDelivery() {
}

ASDelivery::~ASDelivery() {
}

void ASDelivery::set_source_component(ASComponent *p_source) {
	if (p_source) {
		source_id = p_source->get_instance_id();
	} else {
		source_id = ObjectID();
	}
}

ASComponent *ASDelivery::get_source_component() const {
	if (source_id.is_null()) {
		return nullptr;
	}
	return Object::cast_to<ASComponent>(ObjectDB::get_instance(source_id));
}

void ASDelivery::set_package(const Ref<ASPackage> &p_package) {
	package = p_package;
}

Ref<ASPackage> ASDelivery::get_package() const {
	return package;
}

void ASDelivery::set_active(bool p_active) {
	is_active = p_active;
	set_physics_process(is_active && life_span > 0.0f);
}
