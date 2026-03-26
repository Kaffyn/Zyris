/**************************************************************************/
/*  test_as_container_package.h                                           */
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
#include "src/resources/as_container.h"
#include "src/resources/as_effect.h"
#include "src/resources/as_package.h"
#include "src/scene/as_component.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/resources/as_container.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/resources/as_package.h"
#include "modules/ability_system/scene/as_component.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

// ============================================================
// ASContainer Tests
// ============================================================

TEST_CASE("[ASContainer] ability catalog") {
	Ref<ASContainer> container;
	container.instantiate();

	Ref<ASAbility> fireball;
	fireball.instantiate();
	fireball->set_ability_tag(StringName("Ability.Fireball"));

	Ref<ASAbility> shield;
	shield.instantiate();
	shield->set_ability_tag(StringName("Ability.Shield"));

	SUBCASE("add_ability / has_ability - 3 Variations") {
		// Var 1: Empty container has no abilities
		CHECK_FALSE(container->has_ability(fireball));

		// Var 2: After add, has_ability returns true
		container->add_ability(fireball);
		CHECK(container->has_ability(fireball));

		// Var 3: A second, unrelated ability is independent
		CHECK_FALSE(container->has_ability(shield));
		container->add_ability(shield);
		CHECK(container->has_ability(shield));
	}

	SUBCASE("attribute_set binding on container - 3 Variations") {
		// Var 1: No attribute set by default
		CHECK(container->get_attribute_set().is_null());

		// Var 2: Bind an attribute set
		Ref<ASAttributeSet> aset;
		aset.instantiate();
		Ref<ASAttribute> hp;
		hp.instantiate();
		hp->set_attribute_name("Health");
		hp->set_base_value(100.0f);
		aset->add_attribute_definition(hp);
		container->set_attribute_set(aset);
		CHECK_FALSE(container->get_attribute_set().is_null());

		// Var 3: Attribute set has the health attribute
		CHECK(container->get_attribute_set()->has_attribute(StringName("Health")));
	}

	SUBCASE("add_effect / has_effect - 3 Variations") {
		Ref<ASEffect> poison;
		poison.instantiate();
		poison->set_effect_tag(StringName("Effect.Poison"));

		// Var 1: Not present before add
		CHECK_FALSE(container->has_effect(poison));

		// Var 2: Present after add
		container->add_effect(poison);
		CHECK(container->has_effect(poison));

		// Var 3: Get effects list reflects it
		CHECK_MESSAGE(container->get_effects().size() >= 1, "Effects array must contain at least the added effect.");
	}
}

TEST_CASE("[ASContainer] apply_container to ASComponent initializes state") {
	// Build a container with an attribute set and an initial effect
	Ref<ASContainer> container;
	container.instantiate();

	Ref<ASAttributeSet> aset;
	aset.instantiate();
	Ref<ASAttribute> hp;
	hp.instantiate();
	hp->set_attribute_name("Health");
	hp->set_base_value(150.0f);
	hp->set_max_value(150.0f);
	hp->set_min_value(0.0f);
	aset->add_attribute_definition(hp);
	container->set_attribute_set(aset);

	Ref<ASAbility> sprint;
	sprint.instantiate();
	sprint->set_ability_tag(StringName("Ability.Sprint"));
	container->add_ability(sprint);

	SUBCASE("apply_container provides attribute and catalog - 3 Variations") {
		ASComponent *asc = memnew(ASComponent);

		// Var 1: apply at level 1
		asc->apply_container(container, 1);
		CHECK_MESSAGE(asc->has_attribute_by_tag(StringName("Health")),
				"After apply_container, Health attribute must be present.");

		// Var 2: ability catalog is available for unlock
		asc->unlock_ability_by_tag(StringName("Ability.Sprint"));
		CHECK(asc->is_ability_unlocked(StringName("Ability.Sprint")));

		// Var 3: Applying the same container again doesn't crash (idempotent-ish)
		asc->apply_container(container, 1);
		CHECK(asc->has_attribute_by_tag(StringName("Health")));

		memdelete(asc);
	}
}

// ============================================================
// ASPackage Tests
// ============================================================

TEST_CASE("[ASPackage] effect and cue bundling") {
	Ref<ASPackage> pkg;
	pkg.instantiate();

	Ref<ASEffect> damage;
	damage.instantiate();
	damage->set_effect_tag(StringName("Effect.Damage"));

	Ref<ASEffect> slow;
	slow.instantiate();
	slow->set_effect_tag(StringName("Effect.Slow"));

	SUBCASE("add_effect / get_effects / remove_effect - 3 Variations") {
		// Var 1: No effects by default
		CHECK(pkg->get_effects().size() == 0);

		// Var 2: Add two effects
		pkg->add_effect(damage);
		pkg->add_effect(slow);
		CHECK(pkg->get_effects().size() == 2);

		// Var 3: Remove one, verify count
		pkg->remove_effect(damage);
		CHECK(pkg->get_effects().size() == 1);
	}

	SUBCASE("add_effect_tag / get_effect_tags / remove_effect_tag - 3 Variations") {
		// Var 1: No effect tags by default
		CHECK(pkg->get_effect_tags().size() == 0);

		// Var 2: Add tags by string
		pkg->add_effect_tag(StringName("Effect.Poison"));
		pkg->add_effect_tag(StringName("Effect.Stun"));
		CHECK(pkg->get_effect_tags().size() == 2);

		// Var 3: Remove one tag
		pkg->remove_effect_tag(StringName("Effect.Poison"));
		CHECK(pkg->get_effect_tags().size() == 1);
	}

	SUBCASE("clear_effects / clear_cues - 3 Variations") {
		pkg->add_effect(damage);
		pkg->add_effect_tag(StringName("Effect.Dot"));

		// Var 1: Effects present before clear
		CHECK(pkg->get_effects().size() >= 1);

		// Var 2: After clear, effects gone
		pkg->clear_effects();
		CHECK(pkg->get_effects().size() == 0);

		// Var 3: Tags are cleared independently
		pkg->clear_effects(); // idempotent
		CHECK_MESSAGE(pkg->get_effects().size() == 0, "Double clear must be safe.");
	}
}

TEST_CASE("[ASPackage] apply_package to ASComponent via ASComponent::apply_package") {
	Ref<ASPackage> pkg;
	pkg.instantiate();

	Ref<ASEffect> damage;
	damage.instantiate();
	damage->set_effect_name("PackageDamage");
	damage->set_effect_tag(StringName("Effect.PackageDmg"));
	damage->set_duration_policy(ASEffect::POLICY_INSTANT);
	damage->add_modifier(StringName("Health"), ASEffect::OP_ADD, -30.0f);
	pkg->add_effect(damage);

	// Build the target component
	ASComponent *target = memnew(ASComponent);
	Ref<ASAttributeSet> aset;
	aset.instantiate();
	Ref<ASAttribute> hp;
	hp.instantiate();
	hp->set_attribute_name("Health");
	hp->set_base_value(100.0f);
	hp->set_max_value(100.0f);
	hp->set_min_value(0.0f);
	aset->add_attribute_definition(hp);
	target->add_attribute_set(aset);

	SUBCASE("apply_package delivers all bundled effects - 3 Variations") {
		// Var 1: HP at 100 before delivery
		CHECK(target->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(100.0f));

		// Var 2: Package delivers -30 → 70 HP
		target->apply_package(pkg);
		CHECK(target->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(70.0f));

		// Var 3: Apply twice → 40 HP
		target->apply_package(pkg);
		CHECK(target->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(40.0f));
	}

	memdelete(target);
}
