/**************************************************************************/
/*  as_bridge_condition_can_activate.cpp                                  */
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

#include "as_bridge_condition_can_activate.h"

#include "as_bridge.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

void BTConditionAS_CanActivate::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ability_tag", "tag"), &BTConditionAS_CanActivate::set_ability_tag);
	ClassDB::bind_method(D_METHOD("get_ability_tag"), &BTConditionAS_CanActivate::get_ability_tag);

	ClassDB::bind_method(D_METHOD("set_asc_node_path", "path"), &BTConditionAS_CanActivate::set_asc_node_path);
	ClassDB::bind_method(D_METHOD("get_asc_node_path"), &BTConditionAS_CanActivate::get_asc_node_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "ability_tag"), "set_ability_tag", "get_ability_tag");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "asc_node_path"), "set_asc_node_path", "get_asc_node_path");
}

BT::Status BTConditionAS_CanActivate::_tick(double p_delta) {
	Node *agent = get_agent();
	if (!agent) {
		return BT::FAILURE;
	}

	ASComponent *asc = resolve_asc(agent, asc_node_path);
	if (!asc) {
		return BT::FAILURE;
	}

	if (ability_tag.is_empty()) {
		return BT::FAILURE;
	}

	// Check if ability can be activated without actually activating it
	// Uses can_activate_ability_by_tag API
	bool can_activate = asc->can_activate_ability_by_tag(ability_tag);
	return can_activate ? BT::SUCCESS : BT::FAILURE;
}

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE
