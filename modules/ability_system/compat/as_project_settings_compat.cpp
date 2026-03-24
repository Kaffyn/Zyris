/**************************************************************************/
/*  as_project_settings_compat.cpp                                        */
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

#include "as_project_settings_compat.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#else
#include "editor/settings/project_settings_editor.h"
#endif

TabContainer *ASProjectSettingsCompat::get_project_settings_tabs() {
#ifndef ABILITY_SYSTEM_GDEXTENSION
	ProjectSettingsEditor *ps_editor = ProjectSettingsEditor::get_singleton();
	if (ps_editor) {
		return ps_editor->get_tabs();
	}
	return nullptr;
#else
	EditorInterface *editor = EditorInterface::get_singleton();
	if (!editor) {
		return nullptr;
	}

	Control *base_control = editor->get_base_control();
	if (!base_control) {
		return nullptr;
	}

	// Find the Project Settings Editor window dynamically
	TypedArray<Node> ps_editors = base_control->find_children("*", "ProjectSettingsEditor", true, false);
	if (ps_editors.is_empty()) {
		return nullptr;
	}

	Node *ps_editor = Object::cast_to<Node>(ps_editors[0]);
	if (!ps_editor) {
		return nullptr;
	}

	// Inside it, find the general TabContainer that holds the different settings tabs
	TypedArray<Node> tab_containers = ps_editor->find_children("*", "TabContainer", true, false);
	if (tab_containers.is_empty()) {
		return nullptr;
	}

	return Object::cast_to<TabContainer>(tab_containers[0]);
#endif
}
