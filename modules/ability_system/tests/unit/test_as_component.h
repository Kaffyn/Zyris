/**************************************************************************/
/*  test_as_component.h                                                   */
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
#include "src/resources/as_ability.h"
#include "src/resources/as_attribute.h"
#include "src/resources/as_attribute_set.h"
#include "src/resources/as_effect.h"
#include "src/scene/as_component.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/core/as_effect_spec.h"
#include "modules/ability_system/resources/as_ability.h"
#include "modules/ability_system/resources/as_attribute.h"
#include "modules/ability_system/resources/as_attribute_set.h"
#include "modules/ability_system/resources/as_effect.h"
#include "modules/ability_system/scene/as_component.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

// local helper to build a component with Health + Mana
static ASComponent *_make_asc_full(float p_hp = 100.0f, float p_mana = 50.0f) {
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

TEST_CASE("[ASComponent] Tag System") {
	ASComponent *asc = _make_asc_full();

	SUBCASE("add_tag / has_tag / remove_tag - 3 Variations") {
		// Var 1: Simple add and query
		asc->add_tag(StringName("State.Grounded"));
		CHECK(asc->has_tag(StringName("State.Grounded")));

		// Var 2: Two simultaneous tags coexist
		asc->add_tag(StringName("State.Armed"));
		CHECK(asc->has_tag(StringName("State.Grounded")));
		CHECK(asc->has_tag(StringName("State.Armed")));
		CHECK(asc->get_tags().size() >= 2);

		// Var 3: Remove one, other persists
		asc->remove_tag(StringName("State.Grounded"));
		CHECK_FALSE(asc->has_tag(StringName("State.Grounded")));
		CHECK(asc->has_tag(StringName("State.Armed")));
	}

	SUBCASE("remove_all_tags - 3 Variations") {
		asc->add_tag(StringName("State.Stunned"));
		asc->add_tag(StringName("State.Poisoned"));
		asc->add_tag(StringName("State.Burning"));

		// Var 1: All three exist before wipe
		CHECK(asc->get_tags().size() >= 3);

		// Var 2: After remove_all_tags, none remain
		asc->remove_all_tags();
		CHECK(asc->get_tags().size() == 0);

		// Var 3: Double wipe is safe (idempotent)
		asc->remove_all_tags();
		CHECK(asc->get_tags().size() == 0);
	}

	memdelete(asc);
}

TEST_CASE("[ASComponent] Attribute API") {
	ASComponent *asc = _make_asc_full(100.0f, 50.0f);

	SUBCASE("get/set attribute_base_value_by_tag - 3 Variations") {
		// Var 1: Initial value matches attribute definition
		CHECK(asc->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(100.0f));

		// Var 2: Mutate and verify
		asc->set_attribute_base_value_by_tag(StringName("Health"), 60.0f);
		CHECK(asc->get_attribute_base_value_by_tag(StringName("Health")) == doctest::Approx(60.0f));

		// Var 3: Zero is a valid value (dead edge)
		asc->set_attribute_base_value_by_tag(StringName("Health"), 0.0f);
		CHECK(asc->get_attribute_base_value_by_tag(StringName("Health")) == doctest::Approx(0.0f));
	}

	SUBCASE("has_attribute_by_tag - 3 Variations") {
		// Var 1: Known attribute exists
		CHECK(asc->has_attribute_by_tag(StringName("Health")));

		// Var 2: Other known attribute also exists
		CHECK(asc->has_attribute_by_tag(StringName("Mana")));

		// Var 3: Nonexistent attribute returns false
		CHECK_FALSE(asc->has_attribute_by_tag(StringName("Rage")));
	}

	memdelete(asc);
}

TEST_CASE("[ASComponent] Effect Lifecycle") {
	ASComponent *asc = _make_asc_full(100.0f, 50.0f);

	// Build a generic INFINITE HP drain effect (tagged)
	Ref<ASEffect> drain;
	drain.instantiate();
	drain->set_effect_name("HealthDrain");
	drain->set_effect_tag(StringName("Effect.HealthDrain"));
	drain->set_duration_policy(ASEffect::POLICY_INFINITE);
	drain->add_modifier(StringName("Health"), ASEffect::OP_ADD, -10.0f);

	SUBCASE("apply_effect_by_resource / has_active_effect_by_resource - 3 Variations") {
		// Var 1: Effect not active before application
		CHECK_FALSE(asc->has_active_effect_by_resource(drain));

		// Var 2: Apply force-bypasses checks — effect is now active
		asc->apply_effect_by_resource(drain);
		CHECK(asc->has_active_effect_by_resource(drain));

		// Var 3: By tag query also confirms presence
		CHECK(asc->has_active_effect_by_tag(StringName("Effect.HealthDrain")));
	}

	SUBCASE("try_activate_effect_by_resource - 3 Variations") {
		// Build a blocked fire effect
		Ref<ASEffect> fire;
		fire.instantiate();
		fire->set_effect_name("FireDot");
		fire->set_effect_tag(StringName("Effect.FireDot"));
		fire->set_duration_policy(ASEffect::POLICY_INFINITE);
		fire->add_modifier(StringName("Health"), ASEffect::OP_ADD, -5.0f);

		TypedArray<StringName> blocked;
		blocked.push_back(StringName("Status.FireImmune"));
		fire->set_activation_blocked_any_tags(blocked);

		// Var 1: No immunity — try_activate succeeds
		bool ok = asc->try_activate_effect_by_resource(fire);
		CHECK(ok);

		// Var 2: Add immunity tag — try_activate fails
		asc->add_tag(StringName("Status.FireImmune"));
		asc->remove_effect_by_resource(fire);
		bool blocked_result = asc->try_activate_effect_by_resource(fire);
		CHECK_FALSE(blocked_result);

		// Var 3: Remove immunity — try_activate succeeds again
		asc->remove_tag(StringName("Status.FireImmune"));
		bool ok2 = asc->try_activate_effect_by_resource(fire);
		CHECK(ok2);
	}

	SUBCASE("clear_effects / remove_effect_by_tag - 3 Variations") {
		// Var 1: Apply and verify presence
		asc->apply_effect_by_resource(drain);
		CHECK(asc->has_active_effect_by_resource(drain));

		// Var 2: Remove by tag
		asc->remove_effect_by_tag(StringName("Effect.HealthDrain"));
		CHECK_FALSE(asc->has_active_effect_by_resource(drain));

		// Var 3: Clear all at once
		asc->apply_effect_by_resource(drain);
		CHECK(asc->has_active_effect_by_resource(drain));
		asc->clear_effects();
		CHECK_FALSE(asc->has_active_effect_by_resource(drain));
	}

	memdelete(asc);
}

TEST_CASE("[ASComponent] Event System (dispatch_event / has_event_occurred)") {
	ASComponent *asc = _make_asc_full();

	SUBCASE("dispatch_event + has_event_occurred - 3 Variations") {
		// Var 1: Event not in history before dispatch
		CHECK_FALSE(asc->has_event_occurred(StringName("Event.Weapon.Hit"), 5.0f));

		// Var 2: After dispatch, is in lookback window
		asc->dispatch_event(StringName("Event.Weapon.Hit"), nullptr, 25.0f);
		CHECK(asc->has_event_occurred(StringName("Event.Weapon.Hit"), 5.0f));

		// Var 3: Different event tag is still absent
		CHECK_FALSE(asc->has_event_occurred(StringName("Event.Damage.Critical"), 5.0f));
	}

	SUBCASE("clear_event_history - 3 Variations") {
		asc->dispatch_event(StringName("Event.Weapon.Hit"));
		asc->dispatch_event(StringName("Event.Damage.Dealt"));

		// Var 1: Both events present
		CHECK(asc->has_event_occurred(StringName("Event.Weapon.Hit"), 5.0f));

		// Var 2: History cleared
		asc->clear_event_history();
		CHECK_FALSE(asc->has_event_occurred(StringName("Event.Weapon.Hit"), 5.0f));

		// Var 3: Clear is idempotent
		asc->clear_event_history();
		CHECK(asc->get_event_history_size() == 0);
	}

	memdelete(asc);
}

TEST_CASE("[ASComponent] Cooldown API") {
	ASComponent *asc = _make_asc_full();

	SUBCASE("start_cooldown / is_on_cooldown / get_cooldown_remaining - 3 Variations") {
		TypedArray<StringName> tags;
		tags.push_back(StringName("Cooldown.Fireball"));

		// Var 1: Not on cooldown before start
		CHECK_FALSE(asc->is_on_cooldown(StringName("Ability.Fireball")));

		// Var 2: After start, is on cooldown
		asc->start_cooldown(StringName("Ability.Fireball"), 5.0f, tags);
		CHECK(asc->is_on_cooldown(StringName("Ability.Fireball")));
		CHECK(asc->get_cooldown_remaining(StringName("Ability.Fireball")) > 0.0f);

		// Var 3: After full tick duration, cooldown expires
		asc->tick(6.0f);
		CHECK_FALSE(asc->is_on_cooldown(StringName("Ability.Fireball")));
		CHECK(asc->get_cooldown_remaining(StringName("Ability.Fireball")) == doctest::Approx(0.0f));
	}

	memdelete(asc);
}

TEST_CASE("[ASComponent] Ability Unlock & Lock") {
	ASComponent *asc = _make_asc_full(100.0f, 50.0f);

	Ref<ASAbility> fireball;
	fireball.instantiate();
	fireball->set_ability_name("Fireball");
	fireball->set_ability_tag(StringName("Ability.Fireball"));

	SUBCASE("unlock_ability_by_resource / is_ability_unlocked / lock_ability_by_resource - 3 Variations") {
		// Var 1: Not unlocked by default
		CHECK_FALSE(asc->is_ability_unlocked(StringName("Ability.Fireball")));

		// Var 2: Unlock adds to roster
		asc->unlock_ability_by_resource(fireball);
		CHECK(asc->is_ability_unlocked(StringName("Ability.Fireball")));

		// Var 3: Lock removes from roster
		asc->lock_ability_by_resource(fireball);
		CHECK_FALSE(asc->is_ability_unlocked(StringName("Ability.Fireball")));
	}

	SUBCASE("try_activate requires unlock first - 3 Variations") {
		// Var 1: Not unlocked — try_activate fails
		bool fail = asc->try_activate_ability_by_resource(fireball);
		CHECK_FALSE(fail);

		// Var 2: After unlock — can activate (INSTANT ability, no blocked tags)
		asc->unlock_ability_by_resource(fireball);
		bool ok = asc->try_activate_ability_by_resource(fireball);
		CHECK(ok);

		// Var 3: Lock + retrial — fails again
		asc->lock_ability_by_resource(fireball);
		bool fail2 = asc->try_activate_ability_by_resource(fireball);
		CHECK_FALSE(fail2);
	}

	memdelete(asc);
}

TEST_CASE("[ASComponent] Node Registry") {
	ASComponent *asc = _make_asc_full();

	SUBCASE("register_node / get_node_ptr / unregister_node - 3 Variations") {
		// We need a Node to register
		Node *dummy = memnew(Node);

		// Var 1: Not registered returns nullptr
		CHECK(asc->get_node_ptr(StringName("Muzzle")) == nullptr);

		// Var 2: After register, returns the node
		asc->register_node(StringName("Muzzle"), dummy);
		CHECK(asc->get_node_ptr(StringName("Muzzle")) == dummy);

		// Var 3: After unregister, returns nullptr again
		asc->unregister_node(StringName("Muzzle"));
		CHECK(asc->get_node_ptr(StringName("Muzzle")) == nullptr);

		memdelete(dummy);
	}

	memdelete(asc);
}
