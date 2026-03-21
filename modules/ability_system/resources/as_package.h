/**************************************************************************/
/*  as_package.h                                                          */
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
#include "src/resources/as_cue.h"
#include "src/resources/as_effect.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#else
#include "core/io/resource.h"
#include "core/variant/typed_array.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

/**
 * ASPackage
 *
 * A reusable container for effects and cues that can be delivered.
 * Ideal for reusable projectiles, traps, or environmental hazards.
 */
class ASPackage : public Resource {
	GDCLASS(ASPackage, Resource);

private:
	TypedArray<ASEffect> effects;
	TypedArray<StringName> effect_tags;
	TypedArray<ASCue> cues;
	TypedArray<StringName> cue_tags;

protected:
	static void _bind_methods();

public:
	void set_effects(const TypedArray<ASEffect> &p_effects) { effects = p_effects; }
	TypedArray<ASEffect> get_effects() const { return effects; }
	void add_effect(const Ref<ASEffect> &p_effect);
	void remove_effect(const Ref<ASEffect> &p_effect);
	void clear_effects();

	void set_effect_tags(const TypedArray<StringName> &p_tags) { effect_tags = p_tags; }
	TypedArray<StringName> get_effect_tags() const { return effect_tags; }
	void add_effect_tag(const StringName &p_tag);
	void remove_effect_tag(const StringName &p_tag);

	void set_cues(const TypedArray<ASCue> &p_cues) { cues = p_cues; }
	TypedArray<ASCue> get_cues() const { return cues; }
	void add_cue(const Ref<ASCue> &p_cue);
	void remove_cue(const Ref<ASCue> &p_cue);
	void clear_cues();

	void set_cue_tags(const TypedArray<StringName> &p_tags) { cue_tags = p_tags; }
	TypedArray<StringName> get_cue_tags() const { return cue_tags; }
	void add_cue_tag(const StringName &p_tag);
	void remove_cue_tag(const StringName &p_tag);

	ASPackage();
	~ASPackage();
};
