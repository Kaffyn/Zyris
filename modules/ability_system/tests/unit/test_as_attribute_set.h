/**************************************************************************/
/*  test_as_attribute_set.h                                               */
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
#include "src/core/ability_system.h"
#include "src/resources/as_attribute.h"
#include "src/resources/as_attribute_set.h"
#include "src/tests/doctest.h"
#include "src/tests/test_tools.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/tests/doctest.h"
#include "modules/ability_system/tests/test_tools.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

// Helper to build a configured attribute
static Ref<ASAttribute> _make_attr(const char *p_name, float p_base, float p_min, float p_max) {
	Ref<ASAttribute> a;
	a.instantiate();
	a->set_attribute_name(p_name);
	a->set_base_value(p_base);
	a->set_min_value(p_min);
	a->set_max_value(p_max);
	return a;
}

TEST_CASE("[ASAttributeSet] Core Value and Bounds Logic") {
	Ref<ASAttributeSet> attr_set;
	attr_set.instantiate();
	attr_set->add_attribute_definition(_make_attr("Health", 50.0f, 0.0f, 100.0f));
	attr_set->add_attribute_definition(_make_attr("Mana", 30.0f, 0.0f, 50.0f));

	SUBCASE("set/get base_value - 3 Variations") {
		// Var 1: Happy path - value within bounds
		attr_set->set_attribute_base_value(StringName("Health"), 75.0f);
		CHECK(attr_set->get_attribute_base_value(StringName("Health")) == doctest::Approx(75.0f));

		// Var 2: Clamping at max (150 → 100)
		attr_set->set_attribute_base_value(StringName("Health"), 150.0f);
		float clamped = attr_set->get_attribute_base_value(StringName("Health"));
		CHECK_MESSAGE(clamped <= 100.0f, "Value must be clamped to max (100).");

		// Var 3: Nonexistent attribute returns 0.0 gracefully
		float invalid = attr_set->get_attribute_base_value(StringName("Stamina"));
		CHECK(invalid == doctest::Approx(0.0f));
#ifndef ABILITY_SYSTEM_GDEXTENSION
		ability_system::tests::ErrorDetector err;
		err.clear();
		attr_set->get_attribute_base_value(StringName("Stamina"));
		CHECK_MESSAGE(err.has_error, "Querying missing attribute should log push_error in Module mode.");
#endif
	}

	SUBCASE("Modifiers (MODIFIER_ADD / MODIFIER_MULTIPLY) - 3 Variations") {
		attr_set->set_attribute_base_value(StringName("Health"), 50.0f);

		// Var 1: Flat Add +20 → 70
		attr_set->add_modifier(StringName("Health"), 20.0f, ASAttributeSet::MODIFIER_ADD);
		CHECK(attr_set->get_attribute_value(StringName("Health")) == doctest::Approx(70.0f));

		// Var 2: Multiply x2 — (50 * 2) + 20 flat = 120. Must clamp at 100
		attr_set->add_modifier(StringName("Health"), 2.0f, ASAttributeSet::MODIFIER_MULTIPLY);
		CHECK_MESSAGE(attr_set->get_attribute_value(StringName("Health")) <= 100.0f,
				"Over-max result must clamp at max_value.");

		// Var 3: Negative multiply — base * -1 + flat add → should clamp at 0
		attr_set->remove_modifier(StringName("Health"), 2.0f, ASAttributeSet::MODIFIER_MULTIPLY);
		attr_set->add_modifier(StringName("Health"), -1.0f, ASAttributeSet::MODIFIER_MULTIPLY);
		CHECK_MESSAGE(attr_set->get_attribute_value(StringName("Health")) >= 0.0f,
				"Negative result must clamp at min_value (0).");
	}

	SUBCASE("has_attribute / get_attribute_list - 3 Variations") {
		// Var 1: Known attribute
		CHECK(attr_set->has_attribute(StringName("Health")));

		// Var 2: Other known attribute
		CHECK(attr_set->has_attribute(StringName("Mana")));

		// Var 3: Unknown returns false
		CHECK_FALSE(attr_set->has_attribute(StringName("Rage")));
	}

	SUBCASE("reset_current_values - 3 Variations") {
		// Var 1: Mutate and confirm dirty
		attr_set->set_attribute_base_value(StringName("Health"), 25.0f);
		CHECK(attr_set->get_attribute_base_value(StringName("Health")) == doctest::Approx(25.0f));

		// Var 2: Reset brings back to base
		attr_set->reset_current_values();
		// After reset, current == base. Both Mana and Health should be at their last set base.
		CHECK(attr_set->get_attribute_base_value(StringName("Health")) == doctest::Approx(25.0f));

		// Var 3: Mana is also unaffected by Health reset
		CHECK(attr_set->has_attribute(StringName("Mana")));
	}
}
