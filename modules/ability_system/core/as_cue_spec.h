/**************************************************************************/
/*  as_cue_spec.h                                                         */
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
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/variant.hpp>
#else
#include "core/object/ref_counted.h"
#include "core/variant/dictionary.h"
#include "core/variant/variant.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

class ASCue;
class ASComponent;
class ASEffectSpec;

// Runtime context for a single Cue execution.
class ASCueSpec : public RefCounted {
	GDCLASS(ASCueSpec, RefCounted);

protected:
	static void _bind_methods();

private:
	// The Cue definition that is being executed.
	Ref<ASCue> cue;

	// ObjectID of ASC that originated this cue.
	ObjectID source_id;

	// ObjectID of ASC that is the target / owner of this cue.
	ObjectID target_id;

	// Specific node hit (e.g. a limb or bone).
	ObjectID target_node_id;

	// Position where the hit occurred.
	Variant hit_position;

	// The EffectSpec that triggered this cue, if any.
	Ref<ASEffectSpec> effect_spec;

	float level = 1.0f;
	float magnitude = 0.0f;
	Dictionary extra_data;

public:
	// IMPORTANT: Constructor and Destructor must be in .cpp to allow Ref<T> with forward declarations
	ASCueSpec();
	~ASCueSpec();

	void init_from_effect(Ref<ASCue> p_cue,
			ASComponent *p_source,
			ASComponent *p_target,
			Ref<ASEffectSpec> p_effect_spec,
			float p_magnitude);

	void init_manual(Ref<ASCue> p_cue,
			ASComponent *p_owner,
			const Dictionary &p_extra_data);

	// --- Cue ---
	void set_cue(Ref<ASCue> p_cue);
	Ref<ASCue> get_cue() const;

	// --- Source / Target ---
	void set_source_asc(ASComponent *p_asc);
	ASComponent *get_source_asc() const;

	void set_target_asc(ASComponent *p_asc);
	ASComponent *get_target_asc() const;

	void set_target_node(Object *p_node);
	Object *get_target_node() const;

	void set_hit_position(const Variant &p_pos) { hit_position = p_pos; }
	Variant get_hit_position() const { return hit_position; }

	// --- Effect Spec ---
	void set_effect_spec(Ref<ASEffectSpec> p_spec);
	Ref<ASEffectSpec> get_effect_spec() const;

	// --- Magnitude ---
	void set_magnitude(float p_mag) { magnitude = p_mag; }
	float get_magnitude() const { return magnitude; }

	// --- Level ---
	void set_level(float p_lvl) { level = p_lvl; }
	float get_level() const { return level; }

	// --- Extra Data ---
	void set_extra_data(const Dictionary &p_data) { extra_data = p_data; }
	Dictionary get_extra_data() const { return extra_data; }
};
