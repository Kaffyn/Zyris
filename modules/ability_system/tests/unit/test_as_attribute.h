/**************************************************************************/
/*  test_as_attribute.h                                                   */
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
#include "src/resources/as_attribute.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

TEST_CASE("[ASAttribute] Name and base value") {
	Ref<ASAttribute> attr;
	attr.instantiate();

	SUBCASE("set/get attribute_name - 3 Variations") {
		// Var 1: Standard name
		attr->set_attribute_name("Health");
		CHECK(attr->get_attribute_name() == String("Health"));

		// Var 2: Compound name
		attr->set_attribute_name("Physical.Armor");
		CHECK(attr->get_attribute_name() == String("Physical.Armor"));

		// Var 3: Empty name (uninitialized state)
		Ref<ASAttribute> blank;
		blank.instantiate();
		CHECK(blank->get_attribute_name() == String(""));
	}

	SUBCASE("set/get base_value with clamping - 3 Variations") {
		attr->set_min_value(0.0f);
		attr->set_max_value(100.0f);

		// Var 1: Valid mid-range value
		attr->set_base_value(50.0f);
		CHECK(attr->get_base_value() == doctest::Approx(50.0f));

		// Var 2: Value at max
		attr->set_base_value(100.0f);
		CHECK(attr->get_base_value() == doctest::Approx(100.0f));

		// Var 3: Over-max is clamped
		attr->set_base_value(150.0f);
		CHECK_MESSAGE(attr->get_base_value() <= 100.0f,
				"Base value above max_value must be clamped.");
	}
}

TEST_CASE("[ASAttribute] Min/Max limits") {
	Ref<ASAttribute> attr;
	attr.instantiate();

	SUBCASE("set/get min_value and max_value - 3 Variations") {
		// Var 1: Standard RPG health range
		attr->set_min_value(0.0f);
		attr->set_max_value(500.0f);
		CHECK(attr->get_min_value() == doctest::Approx(0.0f));
		CHECK(attr->get_max_value() == doctest::Approx(500.0f));

		// Var 2: Negative min (e.g., temperature or debt)
		attr->set_min_value(-100.0f);
		CHECK(attr->get_min_value() == doctest::Approx(-100.0f));

		// Var 3: Default max (1000.0)
		Ref<ASAttribute> def;
		def.instantiate();
		CHECK(def->get_max_value() == doctest::Approx(1000.0f));
	}
}

TEST_CASE("[ASAttribute] clamp_value and is_valid_value") {
	Ref<ASAttribute> attr;
	attr.instantiate();
	attr->set_attribute_name("Mana");
	attr->set_min_value(0.0f);
	attr->set_max_value(100.0f);
	attr->set_base_value(50.0f);

	SUBCASE("clamp_value - 3 Variations") {
		// Var 1: Value within bounds returns unchanged
		CHECK(attr->clamp_value(75.0f) == doctest::Approx(75.0f));

		// Var 2: Value above max clamped to max
		CHECK(attr->clamp_value(200.0f) == doctest::Approx(100.0f));

		// Var 3: Value below min clamped to min
		CHECK(attr->clamp_value(-50.0f) == doctest::Approx(0.0f));
	}

	SUBCASE("is_valid_value - 3 Variations") {
		// Var 1: Within bounds → valid
		CHECK(attr->is_valid_value(50.0f));

		// Var 2: At exact bounds → valid
		CHECK(attr->is_valid_value(0.0f));
		CHECK(attr->is_valid_value(100.0f));

		// Var 3: Out of bounds → invalid
		CHECK_FALSE(attr->is_valid_value(101.0f));
		CHECK_FALSE(attr->is_valid_value(-1.0f));
	}
}
