/**************************************************************************/
/*  as_delivery.h                                                         */
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

#pragma once

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include "src/resources/as_package.h"
#include <godot_cpp/classes/node.hpp>
#else
#include "modules/ability_system/resources/as_package.h"
#include "scene/main/node.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class ASComponent;

/**
 * ASDelivery
 *
 * A node that handles the delivery of ASPackages (effects/cues) to targets.
 * Supports lifecycle management (timer/validity), target filtering (groups),
 * and automatic Area collision detection.
 */
class ASDelivery : public Node {
	GDCLASS(ASDelivery, Node);

private:
	ObjectID source_id;
	Ref<ASPackage> package;
	float level = 1.0f;

	// Lifecycle & Validation
	bool is_active = false;
	float life_span = 0.0f; // 0 = infinite
	float timer = 0.0f;
	bool one_shot = false;

	// Filtering
	TypedArray<StringName> target_groups;

	// Automation
	bool auto_connect = false;
	bool connected = false;

	void _check_auto_connect();
	void _on_area_body_entered(Node *p_body);
	void _on_area_area_entered(Node *p_area);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	// Payload
	void set_source_component(ASComponent *p_source);
	ASComponent *get_source_component() const;

	void set_package(const Ref<ASPackage> &p_package);
	Ref<ASPackage> get_package() const;

	void set_level(float p_level) { level = p_level; }
	float get_level() const { return level; }

	// Lifecycle
	void set_active(bool p_active);
	bool get_active() const { return is_active; }

	void set_life_span(float p_life_span) { life_span = p_life_span; }
	float get_life_span() const { return life_span; }

	void set_one_shot(bool p_one_shot) { one_shot = p_one_shot; }
	bool get_one_shot() const { return one_shot; }

	void activate(float p_duration = -1.0f);
	void deactivate();
	bool is_delivery_valid() const;

	// Filtering
	void set_target_groups(const TypedArray<StringName> &p_groups) { target_groups = p_groups; }
	TypedArray<StringName> get_target_groups() const { return target_groups; }
	bool can_deliver_to(Node *p_target) const;

	// Automation
	void set_auto_connect(bool p_auto) { auto_connect = p_auto; }
	bool get_auto_connect() const { return auto_connect; }

	// Execution
	void deliver(Node *p_target);

	ASDelivery();
	~ASDelivery();
};
