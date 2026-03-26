/**************************************************************************/
/*  test_gameplay_scenarios.h                                             */
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
#include "src/core/as_effect_spec.h"
#include "src/resources/as_ability.h"
#include "src/resources/as_attribute.h"
#include "src/resources/as_attribute_set.h"
#include "src/resources/as_effect.h"
#include "src/scene/as_component.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/core/ability_system.h"
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

// Helper: build a full ASC with Health + Mana, both starting at max
static ASComponent *_make_char(const char *p_name, float p_max_hp, float p_max_mana) {
	ASComponent *asc = memnew(ASComponent);
	asc->set_name(p_name);

	Ref<ASAttributeSet> aset;
	aset.instantiate();

	Ref<ASAttribute> hp;
	hp.instantiate();
	hp->set_attribute_name("Health");
	hp->set_base_value(p_max_hp);
	hp->set_min_value(0.0f);
	hp->set_max_value(p_max_hp);
	aset->add_attribute_definition(hp);

	if (p_max_mana > 0.0f) {
		Ref<ASAttribute> mp;
		mp.instantiate();
		mp->set_attribute_name("Mana");
		mp->set_base_value(p_max_mana);
		mp->set_min_value(0.0f);
		mp->set_max_value(p_max_mana);
		aset->add_attribute_definition(mp);
	}

	asc->add_attribute_set(aset);
	return asc;
}

// Helper: build an INSTANT heal/damage effect targeting Health
static Ref<ASEffect> _make_instant_hp_effect(float p_magnitude) {
	Ref<ASEffect> fx;
	fx.instantiate();
	fx->set_duration_policy(ASEffect::POLICY_INSTANT);
	fx->add_modifier(StringName("Health"), ASEffect::OP_ADD, p_magnitude);
	return fx;
}

// Helper: build a DURATION periodic damage effect
static Ref<ASEffect> _make_periodic_effect(float p_tick_dmg, float p_period, float p_duration) {
	Ref<ASEffect> fx;
	fx.instantiate();
	fx->set_effect_tag(StringName("Effect.DoT"));
	fx->set_duration_policy(ASEffect::POLICY_DURATION);
	fx->set_duration_magnitude(p_duration);
	fx->set_period(p_period);
	fx->add_modifier(StringName("Health"), ASEffect::OP_ADD, p_tick_dmg);
	return fx;
}

// ============================================================
// SCENARIO 1: The Tavern — NPC Dialog & Potions
// ============================================================

TEST_CASE("[Gameplay] Tavern: NPC Dialog Paralyzes Player + Potion Recovery") {
	ASComponent *player = _make_char("Hero", 100.0f, 50.0f);
	ASComponent *merchant = _make_char("Merchant", 50.0f, 0.0f);

	SUBCASE("Dialog applies State.Paralyzed, blocks ability, then releases - 3 Variations") {
		// Var 1: NPC adds paralysis tag to player
		player->add_tag(StringName("State.Paralyzed"));
		CHECK(player->has_tag(StringName("State.Paralyzed")));

		// Var 2: An attack ability blocked by Paralyzed cannot activate
		Ref<ASAbility> attack;
		attack.instantiate();
		attack->set_ability_name("Strike");
		attack->set_ability_tag(StringName("Ability.Strike"));
		TypedArray<StringName> blocked;
		blocked.push_back(StringName("State.Paralyzed"));
		attack->set_activation_blocked_any_tags(blocked);

		player->unlock_ability_by_resource(attack);
		bool failed = player->try_activate_ability_by_resource(attack);
		CHECK_FALSE_MESSAGE(failed, "Player cannot act while paralyzed by dialog.");

		// Var 3: NPC removes paralysis, player can now act
		player->remove_tag(StringName("State.Paralyzed"));
		CHECK_FALSE(player->has_tag(StringName("State.Paralyzed")));
		bool ok = player->try_activate_ability_by_resource(attack);
		CHECK_MESSAGE(ok, "After dialog ends, player regains freedom of action.");
	}

	SUBCASE("Potion: INSTANT effect heals from depleted HP - 3 Variations") {
		// Var 1: Simulate battle damage
		player->set_attribute_base_value_by_tag(StringName("Health"), 25.0f);
		CHECK(player->get_attribute_base_value_by_tag(StringName("Health")) == doctest::Approx(25.0f));

		// Var 2: Apply heal potion (+40)
		Ref<ASEffect> small_potion = _make_instant_hp_effect(40.0f);
		player->apply_effect_by_resource(small_potion);
		// Health was 25 + 40 = 65
		CHECK(player->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(65.0f));

		// Var 3: Over-heal is clamped (max 100) — apply another +80
		Ref<ASEffect> mega_potion = _make_instant_hp_effect(80.0f);
		player->apply_effect_by_resource(mega_potion);
		// 65 + 80 = 145 → clamped at 100
		CHECK_MESSAGE(player->get_attribute_value_by_tag(StringName("Health")) <= 100.0f,
				"Over-heal is clamped at max HP.");
	}

	memdelete(player);
	memdelete(merchant);
}

// ============================================================
// SCENARIO 2: The Dungeon — Poison Trap & Horde Combat
// ============================================================

TEST_CASE("[Gameplay] Dungeon: Poison Trap (DoT) Lifecycle via tick()") {
	ASComponent *player = _make_char("Hero", 100.0f, 50.0f);

	SUBCASE("Poison DURATION effect ticks and expires - 3 Variations") {
		Ref<ASEffect> poison = _make_periodic_effect(-10.0f, 1.0f, 3.0f);

		// Var 1: Effect is not active before application
		CHECK_FALSE(player->has_active_effect_by_tag(StringName("Effect.DoT")));

		// Var 2: After apply, effect is registered
		player->apply_effect_by_resource(poison);
		CHECK(player->has_active_effect_by_tag(StringName("Effect.DoT")));
		// HP not yet drained (period=1s, no tick yet)
		CHECK(player->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(100.0f));

		// Var 3: After 3 ticks of 1s each, poison has drained 30 HP and should expire
		player->tick(1.0f); // 90 HP — 1st tick
		player->tick(1.0f); // 80 HP — 2nd tick
		player->tick(1.0f); // 70 HP — 3rd tick + expiry
		float remaining_hp = player->get_attribute_value_by_tag(StringName("Health"));
		CHECK_MESSAGE(remaining_hp == doctest::Approx(70.0f), "3 ticks of -10 HP = 70 remaining.");
		CHECK_FALSE_MESSAGE(player->has_active_effect_by_tag(StringName("Effect.DoT")),
				"Poison should expire after its duration.");
	}

	memdelete(player);
}

TEST_CASE("[Gameplay] Dungeon: AoE Spell — Mana Cost + Multi-Target Damage") {
	ASComponent *player = _make_char("Hero", 100.0f, 50.0f);
	ASComponent *goblin_1 = _make_char("Goblin_1", 30.0f, 0.0f);
	ASComponent *goblin_2 = _make_char("Goblin_2", 30.0f, 0.0f);

	// Build: Mana cost 20, damage 50 to each target
	Ref<ASAbility> explosion;
	explosion.instantiate();
	explosion->set_ability_name("AoE Explosion");
	explosion->set_ability_tag(StringName("Ability.AoE"));
	explosion->add_cost(StringName("Mana"), 20.0f);

	// Damage effect applied manually to each goblin (simulates collision resolution)
	Ref<ASEffect> blast = _make_instant_hp_effect(-50.0f);

	SUBCASE("Insufficient Mana blocks explosion - 3 Variations") {
		player->unlock_ability_by_resource(explosion);

		// Var 1: Enough mana (50 ≥ 20)
		CHECK(player->can_activate_ability_by_resource(explosion));

		// Var 2: Zero mana — cannot cast
		player->set_attribute_base_value_by_tag(StringName("Mana"), 0.0f);
		CHECK_FALSE(player->can_activate_ability_by_resource(explosion));

		// Var 3: Exactly at threshold (20 == 20)
		player->set_attribute_base_value_by_tag(StringName("Mana"), 20.0f);
		CHECK(player->can_activate_ability_by_resource(explosion));
	}

	SUBCASE("Explosion fires, drains mana, obliterates goblins - 3 Variations") {
		player->unlock_ability_by_resource(explosion);
		player->set_attribute_base_value_by_tag(StringName("Mana"), 50.0f);

		// Var 1: Activate explosion (INSTANT ability → activates + deducts cost)
		bool fired = player->try_activate_ability_by_resource(explosion);
		CHECK_MESSAGE(fired, "Explosion should fire with full mana.");
		// Mana should be drained by 20
		CHECK_MESSAGE(player->get_attribute_value_by_tag(StringName("Mana")) == doctest::Approx(30.0f),
				"Mana must be deducted after activation.");

		// Var 2: Goblin 1 takes 50 damage (30 HP → 0, clamped)
		goblin_1->apply_effect_by_resource(blast);
		CHECK_MESSAGE(goblin_1->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(0.0f),
				"Goblin 1 must be slain by the explosion.");

		// Var 3: Goblin 2 also takes 50 damage
		goblin_2->apply_effect_by_resource(blast);
		CHECK_MESSAGE(goblin_2->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(0.0f),
				"Goblin 2 must also be slain.");
	}

	memdelete(player);
	memdelete(goblin_1);
	memdelete(goblin_2);
}

// ============================================================
// SCENARIO 3: Boss Arena — INFINITE Aura + Immunity Interaction
// ============================================================

TEST_CASE("[Gameplay] Boss Arena: Immunity negates fire aura") {
	ASComponent *boss = _make_char("FireBoss", 999.0f, 0.0f);
	ASComponent *player = _make_char("Hero", 100.0f, 50.0f);
	ASComponent *paladin = _make_char("Paladin", 120.0f, 30.0f);

	// Boss emits INFINITE fire aura that damages any non-immune actor
	Ref<ASEffect> fire_aura;
	fire_aura.instantiate();
	fire_aura->set_effect_name("FireAura");
	fire_aura->set_effect_tag(StringName("Effect.FireAura"));
	fire_aura->set_duration_policy(ASEffect::POLICY_INSTANT);
	fire_aura->add_modifier(StringName("Health"), ASEffect::OP_ADD, -30.0f);
	// Blocked if target has fire immunity
	TypedArray<StringName> immune_block;
	immune_block.push_back(StringName("Status.FireImmune"));
	fire_aura->set_activation_blocked_any_tags(immune_block);

	SUBCASE("Unprotected player takes aura damage - 3 Variations") {
		// Var 1: Player has no immunity → try_activate succeeds → takes 30 dmg
		bool hit = player->try_activate_effect_by_resource(fire_aura);
		CHECK(hit);
		CHECK(player->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(70.0f));

		// Var 2: Apply fire immunity tag
		player->add_tag(StringName("Status.FireImmune"));
		bool blocked = player->try_activate_effect_by_resource(fire_aura);
		CHECK_FALSE_MESSAGE(blocked, "Fire immune player is protected.");
		// HP still at 70 (not hit again)
		CHECK(player->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(70.0f));

		// Var 3: Paladin without immunity also gets hit
		bool paladin_hit = paladin->try_activate_effect_by_resource(fire_aura);
		CHECK(paladin_hit);
		CHECK(paladin->get_attribute_value_by_tag(StringName("Health")) == doctest::Approx(90.0f));
	}

	memdelete(boss);
	memdelete(player);
	memdelete(paladin);
}

// ============================================================
// SCENARIO 4: Multiplayer — Event-based Parry Window
// ============================================================

TEST_CASE("[Gameplay] Combat: Event Dispatch + Reactive Window (Parry)") {
	ASComponent *warrior = _make_char("Warrior", 150.0f, 20.0f);

	SUBCASE("dispatch_event creates reactive window for parry counter-attack - 3 Variations") {
		// Var 1: No event in history — parry window closed
		CHECK_FALSE(warrior->has_event_occurred(StringName("Event.Damage.Block"), 1.0f));

		// Var 2: Enemy hits, event dispatched — parry window now open
		warrior->dispatch_event(StringName("Event.Damage.Block"), nullptr, 0.0f);
		CHECK(warrior->has_event_occurred(StringName("Event.Damage.Block"), 1.0f));

		// Var 3: Different event not confused with block
		CHECK_FALSE(warrior->has_event_occurred(StringName("Event.Damage.Dealt"), 1.0f));

		warrior->clear_event_history();
		CHECK_FALSE(warrior->has_event_occurred(StringName("Event.Damage.Block"), 1.0f));
	}

	memdelete(warrior);
}
