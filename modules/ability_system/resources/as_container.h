/**************************************************************************/
/*  as_container.h                                                        */
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
#include "src/resources/as_ability.h"
#include "src/resources/as_attribute_set.h"
#include "src/resources/as_cue.h"
#include "src/resources/as_effect.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#else
#include "core/io/resource.h"
#include "core/variant/typed_array.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

/**
 * ASContainer
 *
 * Pure data container that defines the archetype of an actor.
 * It stores the full catalog of abilities, effects, tags,
 * the AttributeSet, and cues. It does NOT execute any logic —
 * the AttributeSet is the one that actually grants abilities
 * and initializes attribute values.
 */
class ASContainer : public Resource {
	GDCLASS(ASContainer, Resource);

private:
	// Full catalog of abilities this archetype CAN use.
	TypedArray<ASAbility> abilities;

	// Effects that are always applied when this container is loaded.
	TypedArray<ASEffect> effects;

	// The AttributeSet responsible for initializing the actor.
	Ref<ASAttributeSet> attribute_set;

	// Cue resources for visual/audio feedback.
	TypedArray<ASCue> cues;

	// Event tags this archetype is subscribed to.
	TypedArray<StringName> events;

protected:
	static void _bind_methods();

public:
	// Abilities (catalog)
	void set_abilities(const TypedArray<ASAbility> &p_abilities);
	TypedArray<ASAbility> get_abilities() const;

	// Effects
	void set_effects(const TypedArray<ASEffect> &p_effects);
	TypedArray<ASEffect> get_effects() const;

	// AttributeSet
	void set_attribute_set(const Ref<ASAttributeSet> &p_set);
	Ref<ASAttributeSet> get_attribute_set() const;

	// Cues
	void set_cues(const TypedArray<ASCue> &p_cues);
	TypedArray<ASCue> get_cues() const;

	// Events (Subscription)
	void set_events(const TypedArray<StringName> &p_events);
	TypedArray<StringName> get_events() const;

	// Dynamic modification
	void add_ability(const Ref<ASAbility> &p_ability);
	void add_effect(const Ref<ASEffect> &p_effect);
	void add_cue(const Ref<ASCue> &p_cue);

	// Resource Verification (Archetype Contract)
	bool has_ability(const Ref<ASAbility> &p_ability) const;
	bool has_effect(const Ref<ASEffect> &p_effect) const;
	bool has_cue(const StringName &p_tag) const;
	bool has_cue_resource(const Ref<ASCue> &p_cue) const;

	ASContainer();
	~ASContainer();
};
