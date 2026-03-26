/**************************************************************************/
/*  test_as_tag_utils.h                                                   */
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
#include "src/core/as_tag_utils.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/as_tag_utils.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

TEST_CASE("[ASTagUtils] Naming convention predicates") {
	SUBCASE("is_state_tag - 3 Variations") {
		// Var 1: Canonical state tag
		CHECK(ASTagUtils::is_state_tag(StringName("State.Stunned")));

		// Var 2: Nested state tag
		CHECK(ASTagUtils::is_state_tag(StringName("State.Combat.Bleeding")));

		// Var 3: Random name — not a state
		CHECK_FALSE(ASTagUtils::is_state_tag(StringName("Ability.Fireball")));
	}

	SUBCASE("is_event_tag - 3 Variations") {
		// Var 1: Root event tag
		CHECK(ASTagUtils::is_event_tag(StringName("Event.Damage")));

		// Var 2: Nested event tag
		CHECK(ASTagUtils::is_event_tag(StringName("Event.Ability.Activated")));

		// Var 3: State tag is NOT an event
		CHECK_FALSE(ASTagUtils::is_event_tag(StringName("State.Burning")));
	}

	SUBCASE("is_immune_tag - 3 Variations") {
		// Var 1: Fire immunity
		CHECK(ASTagUtils::is_immune_tag(StringName("Immune.Fire")));

		// Var 2: Magic immunity
		CHECK(ASTagUtils::is_immune_tag(StringName("Immune.Magic")));

		// Var 3: State tag is not immunity
		CHECK_FALSE(ASTagUtils::is_immune_tag(StringName("State.Immune")));
	}

	SUBCASE("is_can_tag - 3 Variations") {
		// Var 1: Can.Parry permission tag
		CHECK(ASTagUtils::is_can_tag(StringName("Can.Parry")));

		// Var 2: Can.Dash
		CHECK(ASTagUtils::is_can_tag(StringName("Can.Dash")));

		// Var 3: Ability tag is not a Can. tag
		CHECK_FALSE(ASTagUtils::is_can_tag(StringName("Ability.Dash")));
	}

	SUBCASE("is_class_tag - 3 Variations") {
		// Var 1: Warrior class
		CHECK(ASTagUtils::is_class_tag(StringName("Class.Warrior")));

		// Var 2: Mage subclass
		CHECK(ASTagUtils::is_class_tag(StringName("Class.Mage.Pyromancer")));

		// Var 3: State tag is not a class
		CHECK_FALSE(ASTagUtils::is_class_tag(StringName("State.Warrior")));
	}

	SUBCASE("is_team_tag - 3 Variations") {
		// Var 1: Team.Alliance
		CHECK(ASTagUtils::is_team_tag(StringName("Team.Alliance")));

		// Var 2: Team.Horde
		CHECK(ASTagUtils::is_team_tag(StringName("Team.Horde")));

		// Var 3: Class tag is not a team tag
		CHECK_FALSE(ASTagUtils::is_team_tag(StringName("Class.Alliance")));
	}
}

TEST_CASE("[ASTagUtils] detect_tag_type from naming convention") {
	SUBCASE("Event.* → EVENT - 3 Variations") {
		// Var 1: Root
		CHECK(ASTagUtils::detect_tag_type(StringName("Event.Damage")) == ASTagType::EVENT);

		// Var 2: Nested
		CHECK(ASTagUtils::detect_tag_type(StringName("Event.Weapon.Hit")) == ASTagType::EVENT);

		// Var 3: State is NAME, not EVENT
		CHECK(ASTagUtils::detect_tag_type(StringName("State.Burning")) == ASTagType::NAME);
	}

	SUBCASE("Can.* / Immune.* → CONDITIONAL - 3 Variations") {
		// Var 1: Can.Parry is CONDITIONAL
		CHECK(ASTagUtils::detect_tag_type(StringName("Can.Parry")) == ASTagType::CONDITIONAL);

		// Var 2: Immune.Fire is CONDITIONAL
		CHECK(ASTagUtils::detect_tag_type(StringName("Immune.Fire")) == ASTagType::CONDITIONAL);

		// Var 3: Ability.Fireball defaults to NAME
		CHECK(ASTagUtils::detect_tag_type(StringName("Ability.Fireball")) == ASTagType::NAME);
	}

	SUBCASE("Generic tags → NAME - 3 Variations") {
		// Var 1: State tag → NAME
		CHECK(ASTagUtils::detect_tag_type(StringName("State.Grounded")) == ASTagType::NAME);

		// Var 2: Class tag → NAME
		CHECK(ASTagUtils::detect_tag_type(StringName("Class.Warrior")) == ASTagType::NAME);

		// Var 3: Team tag → NAME
		CHECK(ASTagUtils::detect_tag_type(StringName("Team.Alliance")) == ASTagType::NAME);
	}
}

TEST_CASE("[AbilitySystem] Tag Registry (Singleton API)") {
	// The Singleton must be initialized for unit tests to call it.
	// In headless GDExtension mode this is set up by the test runner.
	AbilitySystem *as = AbilitySystem::get_singleton();
	REQUIRE(as != nullptr);

	SUBCASE("register_tag / is_tag_registered / unregister_tag - 3 Variations") {
		// Var 1: Fresh tag should not be registered
		as->unregister_tag(StringName("Test.TagA")); // ensure clean state
		CHECK_FALSE(as->is_tag_registered(StringName("Test.TagA")));

		// Var 2: After register, it is found
		as->register_tag(StringName("Test.TagA"), AbilitySystem::NAME);
		CHECK(as->is_tag_registered(StringName("Test.TagA")));

		// Var 3: After unregister, gone again
		as->unregister_tag(StringName("Test.TagA"));
		CHECK_FALSE(as->is_tag_registered(StringName("Test.TagA")));
	}

	SUBCASE("get_tag_type returns correct classification - 3 Variations") {
		as->register_tag(StringName("Test.NameTag"), AbilitySystem::NAME);
		as->register_tag(StringName("Test.EventTag"), AbilitySystem::EVENT);
		as->register_tag(StringName("Test.CondTag"), AbilitySystem::CONDITIONAL);

		// Var 1: NAME type round-trips correctly
		CHECK(as->get_tag_type(StringName("Test.NameTag")) == AbilitySystem::NAME);

		// Var 2: EVENT type round-trips correctly
		CHECK(as->get_tag_type(StringName("Test.EventTag")) == AbilitySystem::EVENT);

		// Var 3: CONDITIONAL type round-trips correctly
		CHECK(as->get_tag_type(StringName("Test.CondTag")) == AbilitySystem::CONDITIONAL);

		// Cleanup
		as->unregister_tag(StringName("Test.NameTag"));
		as->unregister_tag(StringName("Test.EventTag"));
		as->unregister_tag(StringName("Test.CondTag"));
	}

	SUBCASE("tag_matches (hierarchical) - 3 Variations") {
		// Var 1: Exact match → true
		CHECK(AbilitySystem::tag_matches(StringName("State.Stunned"), StringName("State.Stunned"), true));

		// Var 2: Hierarchical match (parent prefix) → true when exact=false
		CHECK(AbilitySystem::tag_matches(StringName("State.Stunned"), StringName("State"), false));

		// Var 3: Sibling tag does NOT match (State.Burning ≠ State.Stunned exact)
		CHECK_FALSE(AbilitySystem::tag_matches(StringName("State.Burning"), StringName("State.Stunned"), true));
	}

	SUBCASE("remove_tag_branch removes subtree - 3 Variations") {
		as->register_tag(StringName("Branch.Root"), AbilitySystem::NAME);
		as->register_tag(StringName("Branch.Root.Child"), AbilitySystem::NAME);
		as->register_tag(StringName("Branch.Root.Child.Leaf"), AbilitySystem::NAME);

		// Var 1: All registered before removal
		CHECK(as->is_tag_registered(StringName("Branch.Root")));
		CHECK(as->is_tag_registered(StringName("Branch.Root.Child")));

		// Var 2: Remove branch wipes root + all its descendants
		as->remove_tag_branch(StringName("Branch.Root"));
		CHECK_FALSE(as->is_tag_registered(StringName("Branch.Root")));
		CHECK_FALSE(as->is_tag_registered(StringName("Branch.Root.Child")));

		// Var 3: Leaf also wiped
		CHECK_FALSE(as->is_tag_registered(StringName("Branch.Root.Child.Leaf")));
	}

	SUBCASE("rename_tag updates hierarchy - 3 Variations") {
		as->register_tag(StringName("Old.State"), AbilitySystem::NAME);
		as->register_tag(StringName("Old.State.Active"), AbilitySystem::NAME);

		// Var 1: Old name registered before rename
		CHECK(as->is_tag_registered(StringName("Old.State")));

		// Var 2: After rename, new name is registered
		as->rename_tag(StringName("Old.State"), StringName("New.State"));
		CHECK(as->is_tag_registered(StringName("New.State")));

		// Var 3: Old name no longer registered
		CHECK_FALSE(as->is_tag_registered(StringName("Old.State")));

		// Cleanup
		as->remove_tag_branch(StringName("New.State"));
	}
}
