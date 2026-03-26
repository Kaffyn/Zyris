/**************************************************************************/
/*  test_as_tag_spec.h                                                    */
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
#include "src/core/as_tag_spec.h"
#include "src/tests/doctest.h"
#else
#include "modules/ability_system/core/as_tag_spec.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

TEST_CASE("[ASTagSpec] add_tag / remove_tag (reference counting)") {
	Ref<ASTagSpec> spec;
	spec.instantiate();

	SUBCASE("add_tag returns true on first reference, false on duplicates - 3 Variations") {
		// Var 1: First addition returns true (new entry)
		bool added_first = spec->add_tag(StringName("State.Burning"));
		CHECK(added_first);

		// Var 2: Second addition of same tag returns false (ref-count increment)
		bool added_second = spec->add_tag(StringName("State.Burning"));
		CHECK_FALSE(added_second);
		CHECK(spec->has_tag(StringName("State.Burning")));

		// Var 3: A different tag returns true again
		bool added_new = spec->add_tag(StringName("State.Stunned"));
		CHECK(added_new);
	}

	SUBCASE("remove_tag decrements refcount — only removed when count = 0 - 3 Variations") {
		spec->add_tag(StringName("State.Poison"));
		spec->add_tag(StringName("State.Poison")); // refcount = 2

		// Var 1: First removal doesn't actually remove (refcount → 1)
		bool actually_removed = spec->remove_tag(StringName("State.Poison"));
		CHECK_FALSE(actually_removed);
		CHECK(spec->has_tag(StringName("State.Poison")));

		// Var 2: Second removal triggers real deletion (refcount → 0)
		bool gone = spec->remove_tag(StringName("State.Poison"));
		CHECK(gone);
		CHECK_FALSE(spec->has_tag(StringName("State.Poison")));

		// Var 3: Non-existent tag removal returns false gracefully
		bool phantom = spec->remove_tag(StringName("State.Frozen"));
		CHECK_FALSE(phantom);
	}
}

TEST_CASE("[ASTagSpec] has_tag (hierarchical matching)") {
	Ref<ASTagSpec> spec;
	spec.instantiate();
	spec->add_tag(StringName("State.Combat.Bleeding"));

	SUBCASE("has_tag exact vs hierarchical - 3 Variations") {
		// Var 1: Exact match succeeds with exact=true
		CHECK(spec->has_tag(StringName("State.Combat.Bleeding"), true));

		// Var 2: Parent prefix matches with exact=false
		CHECK(spec->has_tag(StringName("State.Combat"), false));

		// Var 3: Unrelated tag doesn't match even hierarchically
		CHECK_FALSE(spec->has_tag(StringName("State.Stunned"), false));
	}
}

TEST_CASE("[ASTagSpec] has_all_tags and has_any_tags") {
	Ref<ASTagSpec> spec;
	spec.instantiate();
	spec->add_tag(StringName("State.Grounded"));
	spec->add_tag(StringName("State.Armed"));

	SUBCASE("has_all_tags (AND logic) - 3 Variations") {
		TypedArray<StringName> req_both;
		req_both.push_back(StringName("State.Grounded"));
		req_both.push_back(StringName("State.Armed"));

		TypedArray<StringName> req_missing;
		req_missing.push_back(StringName("State.Grounded"));
		req_missing.push_back(StringName("State.Silenced")); // not present

		TypedArray<StringName> req_empty;

		// Var 1: Has both → true
		CHECK(spec->has_all_tags(req_both));

		// Var 2: Missing one → false
		CHECK_FALSE(spec->has_all_tags(req_missing));

		// Var 3: Empty requirement → trivially true
		CHECK(spec->has_all_tags(req_empty));
	}

	SUBCASE("has_any_tags (OR logic) - 3 Variations") {
		TypedArray<StringName> any_match;
		any_match.push_back(StringName("State.Silenced")); // not present
		any_match.push_back(StringName("State.Armed")); // present

		TypedArray<StringName> none_match;
		none_match.push_back(StringName("State.Dead"));
		none_match.push_back(StringName("State.Frozen"));

		TypedArray<StringName> single;
		single.push_back(StringName("State.Grounded"));

		// Var 1: One of two matches → true
		CHECK(spec->has_any_tags(any_match));

		// Var 2: None match → false
		CHECK_FALSE(spec->has_any_tags(none_match));

		// Var 3: Single present tag → true
		CHECK(spec->has_any_tags(single));
	}
}

TEST_CASE("[ASTagSpec] get_all_tags and clear") {
	Ref<ASTagSpec> spec;
	spec.instantiate();

	SUBCASE("get_all_tags / clear - 3 Variations") {
		// Var 1: Empty spec has no tags
		CHECK(spec->get_all_tags().size() == 0);

		// Var 2: After additions, correct count
		spec->add_tag(StringName("State.A"));
		spec->add_tag(StringName("State.B"));
		spec->add_tag(StringName("State.C"));
		CHECK(spec->get_all_tags().size() == 3);

		// Var 3: Clear empties it
		spec->clear();
		CHECK(spec->get_all_tags().size() == 0);
	}
}
