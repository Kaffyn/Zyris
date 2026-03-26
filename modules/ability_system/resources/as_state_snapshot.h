/**************************************************************************/
/*  as_state_snapshot.h                                                   */
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
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#else
#include "core/io/resource.h"
#include "core/variant/dictionary.h"
#include "core/variant/variant.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class ASComponent;

/**
 * ASStateSnapshot
 * A resource that captures the full state (attributes and tags) of an ASComponent at a specific tick.
 * Used for replication, rollback, and game saves.
 */
class ASStateSnapshot : public Resource {
	GDCLASS(ASStateSnapshot, Resource);

private:
	uint32_t tick = 0;
	Dictionary attributes;
	PackedStringArray tags;
	Array active_effects;

protected:
	static void _bind_methods();

public:
	void set_tick(uint32_t p_tick) { tick = p_tick; }
	uint32_t get_tick() const { return tick; }

	void set_attributes(const Dictionary &p_attributes) { attributes = p_attributes; }
	Dictionary get_attributes() const { return attributes; }

	void set_tags(const PackedStringArray &p_tags) { tags = p_tags; }
	PackedStringArray get_tags() const { return tags; }

	void set_active_effects(const Array &p_effects) { active_effects = p_effects; }
	Array get_active_effects() const { return active_effects; }

	// Logic
	void capture_from_component(ASComponent *p_component);
	void apply_to_component(ASComponent *p_component) const;

	Ref<ASStateSnapshot> compute_diff(const Ref<ASStateSnapshot> &p_other) const;
	PackedByteArray serialize_binary() const;
	void deserialize_binary(const PackedByteArray &p_data);

	ASStateSnapshot();
	~ASStateSnapshot();
};
