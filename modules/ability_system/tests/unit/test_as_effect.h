/**************************************************************************/
/*  test_as_effect.h                                                      */
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
#include "src/resources/as_effect.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

TEST_CASE("[ASEffect] Modifier Definition API") {
	Ref<ASEffect> fx;
	fx.instantiate();

	SUBCASE("add_modifier / get_modifier_* - 3 Variations") {
		// Var 1: Happy path - flat HP damage
		fx->add_modifier(StringName("Health"), ASEffect::OP_ADD, -20.0f);
		CHECK(fx->get_modifier_count() == 1);
		CHECK(fx->get_modifier_attribute(0) == StringName("Health"));
		CHECK(fx->get_modifier_operation(0) == ASEffect::OP_ADD);
		CHECK(fx->get_modifier_magnitude(0) == doctest::Approx(-20.0f));

		// Var 2: Edge case - multiply by zero (nullify attribute)
		fx->add_modifier(StringName("Armor"), ASEffect::OP_MULTIPLY, 0.0f);
		CHECK(fx->get_modifier_count() == 2);
		CHECK(fx->get_modifier_operation(1) == ASEffect::OP_MULTIPLY);
		CHECK(fx->get_modifier_magnitude(1) == doctest::Approx(0.0f));

		// Var 3: OP_OVERRIDE replacing the value entirely
		fx->add_modifier(StringName("Speed"), ASEffect::OP_OVERRIDE, 999.0f);
		CHECK(fx->get_modifier_count() == 3);
		CHECK(fx->get_modifier_operation(2) == ASEffect::OP_OVERRIDE);
	}

	SUBCASE("Duration Policy - 3 Variations") {
		// Var 1: POLICY_INSTANT (default)
		CHECK(fx->get_duration_policy() == ASEffect::POLICY_INSTANT);

		// Var 2: POLICY_DURATION with magnitude
		fx->set_duration_policy(ASEffect::POLICY_DURATION);
		fx->set_duration_magnitude(5.0f);
		CHECK(fx->get_duration_policy() == ASEffect::POLICY_DURATION);
		CHECK(fx->get_duration_magnitude() == doctest::Approx(5.0f));

		// Var 3: POLICY_INFINITE (lasting aura)
		fx->set_duration_policy(ASEffect::POLICY_INFINITE);
		CHECK(fx->get_duration_policy() == ASEffect::POLICY_INFINITE);
		CHECK_MESSAGE(fx->get_duration_magnitude() == doctest::Approx(5.0f), "Magnitude persists when switching policy.");
	}

	SUBCASE("Stacking Policy - 3 Variations") {
		// Var 1: Default is STACK_NEW_INSTANCE
		CHECK(fx->get_stacking_policy() == ASEffect::STACK_NEW_INSTANCE);

		// Var 2: Override mode (timer resets)
		fx->set_stacking_policy(ASEffect::STACK_OVERRIDE);
		CHECK(fx->get_stacking_policy() == ASEffect::STACK_OVERRIDE);

		// Var 3: Intensity stacking (stack_count grows)
		fx->set_stacking_policy(ASEffect::STACK_INTENSITY);
		CHECK(fx->get_stacking_policy() == ASEffect::STACK_INTENSITY);
	}

	SUBCASE("Tag-based Activation Requirements - 3 Variations") {
		TypedArray<StringName> req_all;
		req_all.push_back(StringName("State.Grounded"));
		req_all.push_back(StringName("State.Armed"));

		// Var 1: Required ALL (AND logic)
		fx->set_activation_required_all_tags(req_all);
		CHECK(fx->get_activation_required_all_tags().size() == 2);

		// Var 2: Blocked ANY (OR logic)
		TypedArray<StringName> blocked_any;
		blocked_any.push_back(StringName("State.Dead"));
		fx->set_activation_blocked_any_tags(blocked_any);
		CHECK(fx->get_activation_blocked_any_tags().size() == 1);
		CHECK(StringName(fx->get_activation_blocked_any_tags()[0]) == StringName("State.Dead"));

		// Var 3: Blocked ALL (AND logic - only blocks if BOTH present simultaneously)
		TypedArray<StringName> blocked_all;
		blocked_all.push_back(StringName("State.Stunned"));
		blocked_all.push_back(StringName("State.Frozen"));
		fx->set_activation_blocked_all_tags(blocked_all);
		CHECK(fx->get_activation_blocked_all_tags().size() == 2);
	}

	SUBCASE("Granted / Removed Tags while active - 3 Variations") {
		// Var 1: Grant a state while effect is active
		TypedArray<StringName> grant;
		grant.push_back(StringName("State.Burning"));
		fx->set_granted_tags(grant);
		CHECK(fx->get_granted_tags().size() == 1);

		// Var 2: Removing tags on application (e.g. poison cures)
		TypedArray<StringName> remove;
		remove.push_back(StringName("State.Poisoned"));
		fx->set_removed_tags(remove);
		CHECK(fx->get_removed_tags().size() == 1);

		// Var 3: Both operate independently on same effect
		CHECK_MESSAGE(fx->get_granted_tags().size() == 1, "Granted stays at 1.");
		CHECK_MESSAGE(fx->get_removed_tags().size() == 1, "Removed stays at 1.");
	}

	SUBCASE("Attribute Requirements - 3 Variations") {
		// Var 1: Single requirement
		fx->add_requirement(StringName("Mana"), 30.0f);
		CHECK(fx->get_requirement_count() == 1);
		CHECK(fx->get_requirement_attribute(0) == StringName("Mana"));
		CHECK(fx->get_requirement_amount(0) == doctest::Approx(30.0f));

		// Var 2: Zero amount requirement (always satisfied)
		fx->add_requirement(StringName("Stamina"), 0.0f);
		CHECK(fx->get_requirement_count() == 2);
		CHECK(fx->get_requirement_amount(1) == doctest::Approx(0.0f));

		// Var 3: Large requirement (intentionally hard to satisfy)
		fx->add_requirement(StringName("Rage"), 9999.0f);
		CHECK(fx->get_requirement_count() == 3);
		CHECK(fx->get_requirement_amount(2) == doctest::Approx(9999.0f));
	}

	SUBCASE("Period (Periodic Tick) - 3 Variations") {
		// Var 1: Default is non-periodic
		CHECK(fx->get_period() == doctest::Approx(0.0f));

		// Var 2: 1 second period (damage per second)
		fx->set_period(1.0f);
		CHECK(fx->get_period() == doctest::Approx(1.0f));

		// Var 3: Instant tick on application flag
		fx->set_execute_periodic_tick_on_application(true);
		CHECK(fx->get_execute_periodic_tick_on_application() == true);
	}
}
