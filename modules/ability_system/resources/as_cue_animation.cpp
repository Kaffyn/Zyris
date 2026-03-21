/**************************************************************************/
/*  as_cue_animation.cpp                                                  */
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
#include "src/resources/as_cue_animation.h"
#include "src/core/as_cue_spec.h"
#include "src/scene/as_component.h"
#else
#include "modules/ability_system/core/as_cue_spec.h"
#include "modules/ability_system/resources/as_cue_animation.h"
#include "modules/ability_system/scene/as_component.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASCueAnimation::execute(Ref<ASCueSpec> p_spec) {
	ASComponent *asc = p_spec->get_target_asc();
	if (!asc) {
		return;
	}

	// Play animation using the component's play_montage method
	if (!animation_name.is_empty()) {
		Node *target = asc->get_node_ptr(get_node_name());
		asc->play_montage(animation_name, target);
	}

	// Call parent for GDVirtual support
	ASCue::execute(p_spec);
}

void ASCueAnimation::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_animation_name", "name"), &ASCueAnimation::set_animation_name);
	ClassDB::bind_method(D_METHOD("get_animation_name"), &ASCueAnimation::get_animation_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "animation_name"), "set_animation_name", "get_animation_name");
}

ASCueAnimation::ASCueAnimation() {
}

ASCueAnimation::~ASCueAnimation() {
}
