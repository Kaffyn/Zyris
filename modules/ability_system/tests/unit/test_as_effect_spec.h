/**************************************************************************/
/*  test_as_effect_spec.h                                                 */
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
#include "src/core/as_effect_spec.h"
#include "src/resources/as_effect.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

TEST_CASE("[ASEffectSpec] init / get_effect / get_level") {
	Ref<ASEffect> e;
	e.instantiate();
	e->set_effect_name("PoisonDot");
	e->set_duration_policy(ASEffect::POLICY_DURATION);
	e->set_duration_magnitude(5.0f);

	Ref<ASEffectSpec> spec;
	spec.instantiate();

	SUBCASE("init populates the spec - 3 Variations") {
		// Var 1: At level 1 by default
		spec->init(e, 1.0f);
		CHECK(spec->get_effect().ptr() == e.ptr());
		CHECK(spec->get_level() == doctest::Approx(1.0f));

		// Var 2: At level 5 (power-scaled)
		Ref<ASEffectSpec> spec2;
		spec2.instantiate();
		spec2->init(e, 5.0f);
		CHECK(spec2->get_level() == doctest::Approx(5.0f));

		// Var 3: Level 0 (disabled power)
		Ref<ASEffectSpec> spec3;
		spec3.instantiate();
		spec3->init(e, 0.0f);
		CHECK(spec3->get_level() == doctest::Approx(0.0f));
	}

	SUBCASE("set/get duration_remaining - 3 Variations") {
		spec->init(e, 1.0f);

		// Var 1: Full duration
		spec->set_duration_remaining(5.0f);
		CHECK(spec->get_duration_remaining() == doctest::Approx(5.0f));

		// Var 2: Half duration
		spec->set_duration_remaining(2.5f);
		CHECK(spec->get_duration_remaining() == doctest::Approx(2.5f));

		// Var 3: Zero (expired state)
		spec->set_duration_remaining(0.0f);
		CHECK(spec->get_duration_remaining() == doctest::Approx(0.0f));
	}

	SUBCASE("set/get total_duration - 3 Variations") {
		spec->init(e, 1.0f);

		// Var 1: Standard duration
		spec->set_total_duration(5.0f);
		CHECK(spec->get_total_duration() == doctest::Approx(5.0f));

		// Var 2: Short duration
		spec->set_total_duration(0.5f);
		CHECK(spec->get_total_duration() == doctest::Approx(0.5f));

		// Var 3: Very long (infinite-like)
		spec->set_total_duration(9999.0f);
		CHECK(spec->get_total_duration() == doctest::Approx(9999.0f));
	}
}

TEST_CASE("[ASEffectSpec] Magnitude overrides") {
	Ref<ASEffect> e;
	e.instantiate();
	e->add_modifier(StringName("Health"), ASEffect::OP_ADD, -10.0f);

	Ref<ASEffectSpec> spec;
	spec.instantiate();
	spec->init(e, 1.0f);

	SUBCASE("set_magnitude / get_magnitude - 3 Variations") {
		// Var 1: Override exists and is readable
		spec->set_magnitude(StringName("Health"), -25.0f);
		CHECK(spec->get_magnitude(StringName("Health")) == doctest::Approx(-25.0f));

		// Var 2: Positive heal override
		spec->set_magnitude(StringName("Health"), 50.0f);
		CHECK(spec->get_magnitude(StringName("Health")) == doctest::Approx(50.0f));

		// Var 3: Unset attribute returns 0.0
		CHECK(spec->get_magnitude(StringName("Mana")) == doctest::Approx(0.0f));
	}
}

TEST_CASE("[ASEffectSpec] Stack count and period timer") {
	Ref<ASEffect> e;
	e.instantiate();
	e->set_stacking_policy(ASEffect::STACK_INTENSITY);
	e->set_period(1.0f);

	Ref<ASEffectSpec> spec;
	spec.instantiate();
	spec->init(e, 1.0f);

	SUBCASE("set/get stack_count - 3 Variations") {
		// Var 1: Default stack count = 1
		CHECK(spec->get_stack_count() == 1);

		// Var 2: Stacked 3 times
		spec->set_stack_count(3);
		CHECK(spec->get_stack_count() == 3);

		// Var 3: Reset to 1
		spec->set_stack_count(1);
		CHECK(spec->get_stack_count() == 1);
	}

	SUBCASE("set/get period_timer - 3 Variations") {
		// Var 1: Timer starts at 0
		CHECK(spec->get_period_timer() == doctest::Approx(0.0f));

		// Var 2: Set to mid-period
		spec->set_period_timer(0.5f);
		CHECK(spec->get_period_timer() == doctest::Approx(0.5f));

		// Var 3: Set to full period
		spec->set_period_timer(1.0f);
		CHECK(spec->get_period_timer() == doctest::Approx(1.0f));
	}
}
