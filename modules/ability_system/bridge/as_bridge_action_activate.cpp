/**************************************************************************/
/*  as_bridge_action_activate.cpp                                         */
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

#include "as_bridge_action_activate.h"

#include "as_bridge.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

void BTActionAS_ActivateAbility::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ability_tag", "tag"), &BTActionAS_ActivateAbility::set_ability_tag);
	ClassDB::bind_method(D_METHOD("get_ability_tag"), &BTActionAS_ActivateAbility::get_ability_tag);

	ClassDB::bind_method(D_METHOD("set_asc_node_path", "path"), &BTActionAS_ActivateAbility::set_asc_node_path);
	ClassDB::bind_method(D_METHOD("get_asc_node_path"), &BTActionAS_ActivateAbility::get_asc_node_path);

	ClassDB::bind_method(D_METHOD("set_activation_level", "level"), &BTActionAS_ActivateAbility::set_activation_level);
	ClassDB::bind_method(D_METHOD("get_activation_level"), &BTActionAS_ActivateAbility::get_activation_level);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "ability_tag"), "set_ability_tag", "get_ability_tag");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "asc_node_path"), "set_asc_node_path", "get_asc_node_path");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "activation_level"), "set_activation_level", "get_activation_level");
}

BT::Status BTActionAS_ActivateAbility::_tick(double p_delta) {
	Node *agent = get_agent();
	if (!agent) {
		WARN_PRINT("BTActionAS_ActivateAbility: No agent assigned");
		return BT::FAILURE;
	}

	ASComponent *asc = resolve_asc(agent, asc_node_path);
	if (!asc) {
		WARN_PRINT("BTActionAS_ActivateAbility: No ASComponent found on agent");
		return BT::FAILURE;
	}

	if (ability_tag.is_empty()) {
		WARN_PRINT("BTActionAS_ActivateAbility: No ability_tag specified");
		return BT::FAILURE;
	}

	// Validate tag exists
	if (!validate_tag(ability_tag)) {
		WARN_PRINT(vformat("BTActionAS_ActivateAbility: Ability tag '%s' not registered in AbilitySystem", ability_tag));
		return BT::FAILURE;
	}

	// Attempt activation using try_activate_ability_by_tag
	// This respects all requirements: tags, cooldowns, costs
	// Note: try_activate_ability_by_tag(tag, target) — second arg is Object*, not float
	bool success = asc->try_activate_ability_by_tag(ability_tag);

	if (success) {
		return BT::SUCCESS;
	} else {
		// Activation failed (requirements not met, on cooldown, etc.)
		// This is normal gameplay flow, not an error
		return BT::FAILURE;
	}
}

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE
