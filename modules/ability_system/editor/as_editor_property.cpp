/**************************************************************************/
/*  as_editor_property.cpp                                                */
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

#ifdef TOOLS_ENABLED

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include "src/editor/as_editor_property.h"
#include "src/core/ability_system.h"
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/editor/as_editor_property.h"
#include "scene/gui/box_container.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

void ASEditorPropertySelector::_edit_pressed() {
	_update_tree();
	dialog->popup_centered_ratio(0.4);
}

void ASEditorPropertySelector::_update_tree() {
	updating = true;
	tags_tree->clear();
	TreeItem *root = tags_tree->create_item();

	AbilitySystem *as = AbilitySystem::get_singleton();
	if (!as) {
		return;
	}

	TypedArray<StringName> current_tags = get_edited_object()->get(get_edited_property());
	String filter = search_edit->get_text().to_lower();

	auto create_items = [&](TreeItem *p_parent, const String &p_prefix, const TypedArray<StringName> &p_tags, const String &p_search) {
		// This is a simplified version of the logic in TagsPanel
		// For the selector, we primarily want to show registered tags as checkboxes
		for (int i = 0; i < p_tags.size(); i++) {
			String tag = p_tags[i];
			if (!p_search.is_empty() && !tag.to_lower().contains(p_search)) {
				continue;
			}

			TreeItem *item = tags_tree->create_item(p_parent);
			item->set_cell_mode(0, TreeItem::CELL_MODE_CHECK);
			item->set_text(0, tag);
			item->set_editable(0, true);
			item->set_selectable(0, true);
			item->set_metadata(0, tag);

			bool is_checked = false;
			for (int j = 0; j < current_tags.size(); j++) {
				if (StringName(current_tags[j]) == tag) {
					is_checked = true;
					break;
				}
			}
			item->set_checked(0, is_checked);
		}
	};

	// Predicate lists (required/blocked) only accept CONDITIONAL tags.
	// Event lists only accept EVENT tags.
	TreeItem *group_root = tags_tree->create_item(root);
	if (filter_type == ASTagType::EVENT) {
		group_root->set_text(0, "Event Tags (Communication)");
	} else if (filter_type == ASTagType::NAME) {
		group_root->set_text(0, "Name Tags (Identity)");
	} else {
		group_root->set_text(0, "Conditional Tags (State)");
	}
	group_root->set_selectable(0, false);
	group_root->set_custom_bg_color(0, Color(1, 1, 1, 0.05));

	create_items(group_root, "", as->get_registered_tags_of_type(filter_type), filter);

	updating = false;
}

void ASEditorPropertySelector::_tag_toggled() {
	if (updating) {
		return;
	}

	TreeItem *item = tags_tree->get_edited();
	if (!item) {
		return;
	}

	StringName tag = item->get_metadata(0);
	bool is_checked = item->is_checked(0);

	TypedArray<StringName> current_tags = get_edited_object()->get(get_edited_property());
	if (is_checked) {
		bool found = false;
		for (int i = 0; i < current_tags.size(); i++) {
			if (StringName(current_tags[i]) == tag) {
				found = true;
				break;
			}
		}
		if (!found) {
			current_tags.push_back(tag);
		}
	} else {
		for (int i = 0; i < current_tags.size(); i++) {
			if (StringName(current_tags[i]) == tag) {
				current_tags.remove_at(i);
				break;
			}
		}
	}

	emit_changed(get_edited_property(), current_tags);
	_update_button_text();
}

void ASEditorPropertySelector::_update_button_text() {
	TypedArray<StringName> current_tags = get_edited_object()->get(get_edited_property());
	if (current_tags.is_empty()) {
		edit_button->set_text("(Empty Tags)");
	} else {
		edit_button->set_text(vformat("%d Tags Selected", current_tags.size()));
	}
}

void ASEditorPropertySelector::_search_changed(const String &p_text) {
	_update_tree();
}

void ASEditorPropertySelector::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_edit_pressed"), &ASEditorPropertySelector::_edit_pressed);
	ClassDB::bind_method(D_METHOD("_tag_toggled"), &ASEditorPropertySelector::_tag_toggled);
	ClassDB::bind_method(D_METHOD("_search_changed"), &ASEditorPropertySelector::_search_changed);
}

#ifndef ABILITY_SYSTEM_GDEXTENSION
void ASEditorPropertySelector::update_property() {
#else
void ASEditorPropertySelector::_update_property() {
#endif
	_update_button_text();
}

ASEditorPropertySelector::ASEditorPropertySelector() {
	edit_button = memnew(Button);
	edit_button->set_clip_text(true);
	edit_button->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	add_child(edit_button);
	add_focusable(edit_button);
	edit_button->connect("pressed", callable_mp(this, &ASEditorPropertySelector::_edit_pressed));

	dialog = memnew(AcceptDialog);
	dialog->set_title("Select Ability System Tags");
	add_child(dialog);

	VBoxContainer *vbox = memnew(VBoxContainer);
	dialog->add_child(vbox);

	search_edit = memnew(LineEdit);
	search_edit->set_placeholder("Filter Tags...");
	search_edit->set_clear_button_enabled(true);
	vbox->add_child(search_edit);
	search_edit->connect("text_changed", callable_mp(this, &ASEditorPropertySelector::_search_changed));

	tags_tree = memnew(Tree);
	tags_tree->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	tags_tree->set_columns(1);
	tags_tree->set_column_expand(0, true);
	tags_tree->set_hide_root(true);
	vbox->add_child(tags_tree);
	tags_tree->connect("item_edited", callable_mp(this, &ASEditorPropertySelector::_tag_toggled));
}

// ASEditorPropertyName implementation
void ASEditorPropertyName::_text_changed(const String &p_text) {
	if (updating) {
		return;
	}
	emit_changed(get_edited_property(), p_text);
	_check_uniqueness(p_text);
}

void ASEditorPropertyName::_check_uniqueness(const String &p_text) {
	if (p_text.is_empty()) {
		warning_label->hide();
		return;
	}

	AbilitySystem *as = AbilitySystem::get_singleton();
	if (as) {
		uint64_t owner = as->get_resource_name_owner(p_text);
		if (owner != 0 && owner != (uint64_t)get_edited_object()->get_instance_id()) {
			warning_label->show();
			return;
		}
	}
	warning_label->hide();
}

void ASEditorPropertyName::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_text_changed", "text"), &ASEditorPropertyName::_text_changed);
}

#ifndef ABILITY_SYSTEM_GDEXTENSION
void ASEditorPropertyName::update_property() {
#else
void ASEditorPropertyName::_update_property() {
#endif
	Variant v = get_edited_object()->get(get_edited_property());
	String text = v.operator String();
	updating = true;
	if (edit->get_text() != text) {
		edit->set_text(text);
	}
	_check_uniqueness(text);
	updating = false;
}

ASEditorPropertyName::ASEditorPropertyName() {
	edit = memnew(LineEdit);
	edit->set_placeholder("Resource Name");
	edit->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	add_child(edit);
	add_focusable(edit);
	edit->connect("text_changed", callable_mp(this, &ASEditorPropertyName::_text_changed));

	warning_label = memnew(Label);
	warning_label->set_text("Warning: Duplicate resource name!");
	warning_label->set_modulate(Color(1, 0.5, 0)); // Orange color
	warning_label->hide();
	add_child(warning_label);
	_check_uniqueness("");
}

// ASEditorPropertyTagSelector implementation
void ASEditorPropertyTagSelector::_option_selected(int p_index) {
	if (updating) {
		return;
	}

	String selected_tag;
	if (p_index > 0) { // Index 0 is "(None)"
		selected_tag = options->get_item_text(p_index);
	}

	emit_changed(get_edited_property(), selected_tag);
}

void ASEditorPropertyTagSelector::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_option_selected", "index"), &ASEditorPropertyTagSelector::_option_selected);
}

#ifndef ABILITY_SYSTEM_GDEXTENSION
void ASEditorPropertyTagSelector::update_property() {
#else
void ASEditorPropertyTagSelector::_update_property() {
#endif
	Variant v = get_edited_object()->get(get_edited_property());
	String current_tag = v.operator String();

	updating = true;

	// Clear and repopulate options
	options->clear();
	options->add_item("(None)"); // Index 0

	AbilitySystem *as = AbilitySystem::get_singleton();
	if (as) {
		// Only show tags of the type configured by the InspectorPlugin.
		// NAME     -> identity/naming selectors (ability_tag, cue_tag, effect_tag)
		// CONDITIONAL -> state predicate selectors (items inside _tags arrays)
		// EVENT    -> event trigger selectors (trigger tag in TRIGGER_ON_EVENT)
		TypedArray<StringName> tags = as->get_registered_tags_of_type(filter_type);
		int selected_index = 0;

		for (int i = 0; i < tags.size(); i++) {
			String tag = tags[i];
			options->add_item(tag);
			if (tag == current_tag) {
				selected_index = options->get_item_count() - 1;
			}
		}

		options->select(selected_index);
	}

	updating = false;
}

ASEditorPropertyTagSelector::ASEditorPropertyTagSelector() {
	options = memnew(OptionButton);
	options->set_clip_text(true);
	options->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
	add_child(options);
	add_focusable(options);
	options->connect("item_selected", callable_mp(this, &ASEditorPropertyTagSelector::_option_selected));

	// Connect to AbilitySystem for tag updates
	AbilitySystem *as = AbilitySystem::get_singleton();
	if (as) {
#ifndef ABILITY_SYSTEM_GDEXTENSION
		as->connect("tags_changed", callable_mp(this, &ASEditorPropertyTagSelector::update_property));
#else
		as->connect("tags_changed", callable_mp(this, &ASEditorPropertyTagSelector::_update_property));
#endif
	}
}

#endif // TOOLS_ENABLED
