/**************************************************************************/
/*  as_inspector_plugin.cpp                                               */
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
#include "src/editor/as_inspector_plugin.h"
#include "src/editor/as_editor_property.h"
#include "src/resources/as_ability.h"
#include "src/resources/as_container.h"
#include "src/resources/as_cue.h"
#include "src/resources/as_effect.h"
#include "src/scene/as_component.h"
#include <godot_cpp/classes/editor_inspector.hpp>
#else
#include "editor/inspector/editor_inspector.h"
#include "modules/ability_system/editor/as_editor_property.h"
#include "modules/ability_system/editor/as_inspector_plugin.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_container.h"
#include "modules/ability_system/resources/as_cue.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/scene/as_component.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

#ifndef ABILITY_SYSTEM_GDEXTENSION
bool ASInspectorPlugin::can_handle(Object *p_object) {
#else
bool ASInspectorPlugin::_can_handle(Object *p_object) const {
#endif
	return Object::cast_to<ASComponent>(p_object) != nullptr ||
			Object::cast_to<ASAbility>(p_object) != nullptr ||
			Object::cast_to<ASEffect>(p_object) != nullptr ||
			Object::cast_to<ASCue>(p_object) != nullptr ||
			Object::cast_to<ASContainer>(p_object) != nullptr;
}

void ASInspectorPlugin::_bind_methods() {
}

#ifndef ABILITY_SYSTEM_GDEXTENSION
bool ASInspectorPlugin::parse_property(Object *p_object, Variant::Type p_type, const String &p_path, PropertyHint p_hint, const String &p_hint_text, BitField<PropertyUsageFlags> p_usage, bool p_wide) {
#else
bool ASInspectorPlugin::_parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
#endif
	if (p_path.ends_with("_name")) {
		ASEditorPropertyName *property_editor = memnew(ASEditorPropertyName);
		add_property_editor(p_path, property_editor);
		return true;
	}

	if (p_path.ends_with("_tags")) {
		ASEditorPropertySelector *property_editor = memnew(ASEditorPropertySelector);
		add_property_editor(p_path, property_editor);
		return true;
	}

	if (p_path.ends_with("_tag") || (p_path.contains("/") && p_path.get_slice("/", 0).ends_with("_tags"))) {
		ASEditorPropertyTagSelector *property_editor = memnew(ASEditorPropertyTagSelector);
		add_property_editor(p_path, property_editor);
		return true;
	}

	return false;
}
