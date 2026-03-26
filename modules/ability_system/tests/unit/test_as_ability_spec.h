/**************************************************************************/
/*  test_as_ability_spec.h                                                */
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
#include "src/core/as_ability_spec.h"
#include "src/resources/as_ability.h"
#include "src/resources/as_effect.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/core/as_ability_spec.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

TEST_CASE("[ASAbilitySpec] init / get_ability / get_level") {
	Ref<ASAbility> ab;
	ab.instantiate();
	ab->set_ability_name("HeroLeap");
	ab->set_ability_tag(StringName("Ability.HeroLeap"));
	ab->set_cooldown_duration(3.0f);

	Ref<ASAbilitySpec> spec;
	spec.instantiate();

	SUBCASE("init stores ability + level - 3 Variations") {
		// Var 1: Level 1 (standard)
		spec->init(ab, 1);
		CHECK(spec->get_ability().ptr() == ab.ptr());
		CHECK(spec->get_level() == 1);

		// Var 2: Level 5 (powerful variant)
		Ref<ASAbilitySpec> spec2;
		spec2.instantiate();
		spec2->init(ab, 5);
		CHECK(spec2->get_level() == 5);

		// Var 3: Level 10 (max tier)
		Ref<ASAbilitySpec> spec3;
		spec3.instantiate();
		spec3->init(ab, 10);
		CHECK(spec3->get_level() == 10);
	}

	SUBCASE("get_cooldown_duration from ability - 3 Variations") {
		spec->init(ab, 1);

		// Var 1: Cooldown matches the ability definition
		CHECK(spec->get_cooldown_duration() == doctest::Approx(3.0f));

		// Var 2: Different ability with no cooldown
		Ref<ASAbility> free_cast;
		free_cast.instantiate();
		free_cast->set_ability_tag(StringName("Ability.FreeCast"));
		free_cast->set_cooldown_duration(0.0f);
		Ref<ASAbilitySpec> spec_free;
		spec_free.instantiate();
		spec_free->init(free_cast, 1);
		CHECK(spec_free->get_cooldown_duration() == doctest::Approx(0.0f));

		// Var 3: Very long cooldown ability
		Ref<ASAbility> ultimate;
		ultimate.instantiate();
		ultimate->set_ability_tag(StringName("Ability.Ultimate"));
		ultimate->set_cooldown_duration(90.0f);
		Ref<ASAbilitySpec> spec_ult;
		spec_ult.instantiate();
		spec_ult->init(ultimate, 1);
		CHECK(spec_ult->get_cooldown_duration() == doctest::Approx(90.0f));
	}
}

TEST_CASE("[ASAbilitySpec] is_active lifecycle") {
	Ref<ASAbility> ab;
	ab.instantiate();
	ab->set_ability_tag(StringName("Ability.Sprint"));

	Ref<ASAbilitySpec> spec;
	spec.instantiate();
	spec->init(ab, 1);

	SUBCASE("set_is_active / get_is_active - 3 Variations") {
		// Var 1: Default is NOT active
		CHECK_FALSE(spec->get_is_active());

		// Var 2: Set to active
		spec->set_is_active(true);
		CHECK(spec->get_is_active());

		// Var 3: Deactivate
		spec->set_is_active(false);
		CHECK_FALSE(spec->get_is_active());
	}
}

TEST_CASE("[ASAbilitySpec] Duration management") {
	Ref<ASAbility> ab;
	ab.instantiate();
	ab->set_ability_tag(StringName("Ability.Shield"));
	ab->set_duration_policy(ASAbility::POLICY_DURATION);
	ab->set_ability_duration(5.0f);

	Ref<ASAbilitySpec> spec;
	spec.instantiate();
	spec->init(ab, 1);

	SUBCASE("set/get total_duration and duration_remaining - 3 Variations") {
		// Var 1: Set full duration
		spec->set_total_duration(5.0f);
		spec->set_duration_remaining(5.0f);
		CHECK(spec->get_total_duration() == doctest::Approx(5.0f));
		CHECK(spec->get_duration_remaining() == doctest::Approx(5.0f));

		// Var 2: Partially consumed
		spec->set_duration_remaining(2.0f);
		CHECK(spec->get_duration_remaining() == doctest::Approx(2.0f));

		// Var 3: Zero = expired
		spec->set_duration_remaining(0.0f);
		CHECK(spec->get_duration_remaining() == doctest::Approx(0.0f));
	}
}

TEST_CASE("[ASAbilitySpec] tick() advances duration") {
	Ref<ASAbility> ab;
	ab.instantiate();
	ab->set_ability_tag(StringName("Ability.TimedBuff"));
	ab->set_duration_policy(ASAbility::POLICY_DURATION);
	ab->set_ability_duration(2.0f);

	Ref<ASAbilitySpec> spec;
	spec.instantiate();
	spec->init(ab, 1);
	spec->set_total_duration(2.0f);
	spec->set_duration_remaining(2.0f);
	spec->set_is_active(true);

	SUBCASE("tick decrements duration - 3 Variations") {
		// Var 1: 1s tick → 1s remaining
		bool still_running = spec->tick(1.0f);
		CHECK(still_running);
		CHECK(spec->get_duration_remaining() == doctest::Approx(1.0f));

		// Var 2: Another 0.5s → 0.5s remaining
		spec->tick(0.5f);
		CHECK(spec->get_duration_remaining() == doctest::Approx(0.5f));

		// Var 3: Final tick causes expiration → returns false
		bool expired = spec->tick(1.0f);
		CHECK_FALSE(expired);
		CHECK(spec->get_duration_remaining() <= 0.0f);
	}
}

TEST_CASE("[ASAbilitySpec] get_effects_for_target filtering") {
	Ref<ASEffect> self_fx;
	self_fx.instantiate();
	self_fx->set_target_type(ASEffect::TARGET_SELF);
	self_fx->set_effect_tag(StringName("Effect.SelfBuff"));

	Ref<ASEffect> other_fx;
	other_fx.instantiate();
	other_fx->set_target_type(ASEffect::TARGET_OTHERS);
	other_fx->set_effect_tag(StringName("Effect.Projectile"));

	Ref<ASAbility> ab;
	ab.instantiate();
	ab->set_ability_tag(StringName("Ability.FireLance"));
	TypedArray<ASEffect> effects;
	effects.push_back(self_fx);
	effects.push_back(other_fx);
	ab->set_effects(effects);

	Ref<ASAbilitySpec> spec;
	spec.instantiate();
	spec->init(ab, 1);

	SUBCASE("get_effects_for_target - 3 Variations") {
		// Var 1: TARGET_SELF returns only self-targeted effects
		TypedArray<ASEffect> self_list = spec->get_effects_for_target(ASEffect::TARGET_SELF);
		CHECK(self_list.size() == 1);

		// Var 2: TARGET_OTHERS returns only outgoing effects
		TypedArray<ASEffect> other_list = spec->get_effects_for_target(ASEffect::TARGET_OTHERS);
		CHECK(other_list.size() == 1);

		// Var 3: Both lists sum to total effect count
		CHECK_MESSAGE(self_list.size() + other_list.size() == 2,
				"Self + Others must cover all ability effects.");
	}
}

TEST_CASE("[ASAbilitySpec] sub-spec hierarchy") {
	Ref<ASAbility> parent_ab;
	parent_ab.instantiate();
	parent_ab->set_ability_tag(StringName("Ability.Parent"));

	Ref<ASAbility> child_ab;
	child_ab.instantiate();
	child_ab->set_ability_tag(StringName("Ability.Child"));

	Ref<ASAbilitySpec> parent_spec;
	parent_spec.instantiate();
	parent_spec->init(parent_ab, 1);

	Ref<ASAbilitySpec> child_spec;
	child_spec.instantiate();
	child_spec->init(child_ab, 1);

	SUBCASE("add_sub_spec / get_sub_specs / remove_sub_spec - 3 Variations") {
		// Var 1: No sub-specs initially
		CHECK(parent_spec->get_sub_specs().size() == 0);

		// Var 2: Add child spec
		parent_spec->add_sub_spec(child_spec);
		CHECK(parent_spec->get_sub_specs().size() == 1);

		// Var 3: Remove child spec
		parent_spec->remove_sub_spec(child_spec);
		CHECK(parent_spec->get_sub_specs().size() == 0);
	}
}
