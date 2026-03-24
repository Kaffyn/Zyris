/**************************************************************************/
/*  as_bridge_condition_has_tag.cpp                                       */
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

#include "as_bridge_condition_has_tag.h"

#include "as_bridge.h"

#if AS_BRIDGE_LIMBOAI_AVAILABLE

void BTConditionAS_HasTag::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_tag", "tag"), &BTConditionAS_HasTag::set_tag);
	ClassDB::bind_method(D_METHOD("get_tag"), &BTConditionAS_HasTag::get_tag);

	ClassDB::bind_method(D_METHOD("set_exact_match", "exact"), &BTConditionAS_HasTag::set_exact_match);
	ClassDB::bind_method(D_METHOD("get_exact_match"), &BTConditionAS_HasTag::get_exact_match);

	ClassDB::bind_method(D_METHOD("set_asc_node_path", "path"), &BTConditionAS_HasTag::set_asc_node_path);
	ClassDB::bind_method(D_METHOD("get_asc_node_path"), &BTConditionAS_HasTag::get_asc_node_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "tag"), "set_tag", "get_tag");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "exact_match"), "set_exact_match", "get_exact_match");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "asc_node_path"), "set_asc_node_path", "get_asc_node_path");
}

BT::Status BTConditionAS_HasTag::_tick(double p_delta) {
	Node *agent = get_agent();
	if (!agent) {
		return BT::FAILURE;
	}

	ASComponent *asc = resolve_asc(agent, asc_node_path);
	if (!asc) {
		return BT::FAILURE;
	}

	if (tag.is_empty()) {
		return BT::FAILURE;
	}

	// ASComponent::has_tag checks tag presence
	// Note: exact_match parameter is not yet supported in ASComponent API
	bool result = asc->has_tag(tag);
	return result ? BT::SUCCESS : BT::FAILURE;
}

#endif // AS_BRIDGE_LIMBOAI_AVAILABLE
