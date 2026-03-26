/**************************************************************************/
/*  test_as_ability.h                                                     */
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
#include "src/resources/as_ability.h"
#include "src/resources/as_attribute.h"
#include "src/resources/as_attribute_set.h"
#include "src/scene/as_component.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/scene/as_component.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

// Helper: builds a minimal ASComponent with Health and Mana attributes.
static ASComponent *_make_asc(float p_hp = 100.0f, float p_mana = 50.0f) {
	ASComponent *asc = memnew(ASComponent);

	Ref<ASAttributeSet> aset;
	aset.instantiate();

	Ref<ASAttribute> hp;
	hp.instantiate();
	hp->set_attribute_name("Health");
	hp->set_base_value(p_hp);
	hp->set_min_value(0.0f);
	hp->set_max_value(p_hp);
	aset->add_attribute_definition(hp);

	Ref<ASAttribute> mp;
	mp.instantiate();
	mp->set_attribute_name("Mana");
	mp->set_base_value(p_mana);
	mp->set_min_value(0.0f);
	mp->set_max_value(p_mana);
	aset->add_attribute_definition(mp);

	asc->add_attribute_set(aset);
	return asc;
}

TEST_CASE("[ASAbility] Identity & Tag Configuration") {
	Ref<ASAbility> ab;
	ab.instantiate();

	SUBCASE("ability_name / ability_tag - 3 Variations") {
		// Var 1: Basic name only
		ab->set_ability_name("Fireball");
		CHECK(ab->get_ability_name() == String("Fireball"));

		// Var 2: Tag set separately
		ab->set_ability_tag(StringName("Ability.Fireball"));
		CHECK(ab->get_ability_tag() == StringName("Ability.Fireball"));

		// Var 3: Empty tag (uninitialized state)
		Ref<ASAbility> blank;
		blank.instantiate();
		CHECK_MESSAGE(blank->get_ability_name() == String(""), "Default name should be empty.");
		CHECK_MESSAGE(blank->get_ability_tag() == StringName(""), "Default tag should be empty StringName.");
	}

	SUBCASE("Activation Tag Predicates - 3 Variations") {
		// Var 1: Required ALL (AND — must have both to cast)
		TypedArray<StringName> req_all;
		req_all.push_back(StringName("State.Grounded"));
		req_all.push_back(StringName("State.Armed"));
		ab->set_activation_required_all_tags(req_all);
		CHECK(ab->get_activation_required_all_tags().size() == 2);

		// Var 2: Blocked ANY (OR — silenced OR stunned blocks casting)
		TypedArray<StringName> blocked_any;
		blocked_any.push_back(StringName("State.Silenced"));
		blocked_any.push_back(StringName("State.Stunned"));
		ab->set_activation_blocked_any_tags(blocked_any);
		CHECK(ab->get_activation_blocked_any_tags().size() == 2);

		// Var 3: Required ANY (OR — needs fire OR arcane affinity)
		TypedArray<StringName> req_any;
		req_any.push_back(StringName("Affinity.Fire"));
		req_any.push_back(StringName("Affinity.Arcane"));
		ab->set_activation_required_any_tags(req_any);
		CHECK(ab->get_activation_required_any_tags().size() == 2);
	}

	SUBCASE("Cooldown Native API - 3 Variations") {
		// Var 1: No cooldown by default
		CHECK(ab->get_cooldown_duration() == doctest::Approx(0.0f));

		// Var 2: Fixed 5s cooldown
		ab->set_cooldown_duration(5.0f);
		CHECK(ab->get_cooldown_duration() == doctest::Approx(5.0f));

		// Var 3: Cooldown tags assigned
		TypedArray<StringName> cd_tags;
		cd_tags.push_back(StringName("Cooldown.Fireball"));
		ab->set_cooldown_tags(cd_tags);
		CHECK(ab->get_cooldown_tags().size() == 1);
		CHECK(StringName(ab->get_cooldown_tags()[0]) == StringName("Cooldown.Fireball"));
	}
}

TEST_CASE("[ASAbility] Cost API & can_afford_costs") {
	Ref<ASAbility> ab;
	ab.instantiate();
	ab->set_ability_name("BlastShot");
	ab->set_ability_tag(StringName("Ability.BlastShot"));
	ab->add_cost(StringName("Mana"), 20.0f);

	SUBCASE("add_cost / get_cost_amount / remove_cost - 3 Variations") {
		// Var 1: Cost is registered
		CHECK(ab->get_cost_amount(StringName("Mana")) == doctest::Approx(20.0f));

		// Var 2: Unknown attribute returns zero
		CHECK(ab->get_cost_amount(StringName("Stamina")) == doctest::Approx(0.0f));

		// Var 3: Removal works
		bool removed = ab->remove_cost(StringName("Mana"));
		CHECK(removed);
		CHECK(ab->get_cost_amount(StringName("Mana")) == doctest::Approx(0.0f));
	}

	SUBCASE("can_afford_costs integration - 3 Variations") {
		// Re-add cost since previous subcase removed it
		ab->add_cost(StringName("Mana"), 20.0f);

		// Var 1: Player has enough mana (50 >= 20)
		ASComponent *asc = _make_asc(100.0f, 50.0f);
		asc->unlock_ability_by_resource(ab);
		CHECK(ab->can_afford_costs(asc));

		// Var 2: Player exactly at the cost threshold (20 == 20)
		asc->set_attribute_base_value_by_tag(StringName("Mana"), 20.0f);
		CHECK(ab->can_afford_costs(asc));

		// Var 3: Insufficient mana (10 < 20)
		asc->set_attribute_base_value_by_tag(StringName("Mana"), 10.0f);
		CHECK_FALSE(ab->can_afford_costs(asc));

		memdelete(asc);
	}
}

TEST_CASE("[ASAbility] can_activate_ability via ASComponent") {
	Ref<ASAbility> ab;
	ab.instantiate();
	ab->set_ability_name("StealthStrike");
	ab->set_ability_tag(StringName("Ability.StealthStrike"));
	ab->add_cost(StringName("Mana"), 15.0f);
	ab->set_cooldown_duration(3.0f);

	TypedArray<StringName> blocked;
	blocked.push_back(StringName("State.Stunned"));
	ab->set_activation_blocked_any_tags(blocked);

	ASComponent *player = _make_asc(100.0f, 50.0f);
	player->unlock_ability_by_resource(ab);

	SUBCASE("Blocked by tag - 3 Variations") {
		// Var 1: Clean state → can activate
		CHECK(player->can_activate_ability_by_resource(ab));

		// Var 2: Stunned → blocked
		player->add_tag(StringName("State.Stunned"));
		CHECK_FALSE(player->can_activate_ability_by_resource(ab));

		// Var 3: Stun removed → unblocked
		player->remove_tag(StringName("State.Stunned"));
		CHECK(player->can_activate_ability_by_resource(ab));
	}

	SUBCASE("Blocked by insufficient cost - 3 Variations") {
		// Var 1: Enough mana (50 >= 15)
		CHECK(player->can_activate_ability_by_resource(ab));

		// Var 2: Zero mana (0 < 15)
		player->set_attribute_base_value_by_tag(StringName("Mana"), 0.0f);
		CHECK_FALSE(player->can_activate_ability_by_resource(ab));

		// Var 3: Exactly at threshold (15 == 15)
		player->set_attribute_base_value_by_tag(StringName("Mana"), 15.0f);
		CHECK(player->can_activate_ability_by_resource(ab));
	}

	SUBCASE("Cooldown enforcement - 3 Variations") {
		// Var 1: No cooldown active → can activate
		CHECK_FALSE(player->is_on_cooldown(StringName("Ability.StealthStrike")));

		// Var 2: Manually start cooldown → blocked
		TypedArray<StringName> empty_tags;
		player->start_cooldown(StringName("Ability.StealthStrike"), 3.0f, empty_tags);
		CHECK(player->is_on_cooldown(StringName("Ability.StealthStrike")));
		CHECK(player->get_cooldown_remaining(StringName("Ability.StealthStrike")) > 0.0f);

		// Var 3: Tick past cooldown → free again
		player->tick(3.5f);
		CHECK_FALSE(player->is_on_cooldown(StringName("Ability.StealthStrike")));
	}

	memdelete(player);
}
