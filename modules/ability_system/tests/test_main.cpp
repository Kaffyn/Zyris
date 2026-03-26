/**************************************************************************/
/*  test_main.cpp                                                         */
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

#ifdef ABILITY_SYSTEM_GDEXTENSION

// Only define DOCTEST_CONFIG_IMPLEMENT once, in this single translation unit.
#define DOCTEST_CONFIG_IMPLEMENT
#include "src/tests/doctest.h"

// --- Unit: Data Resources ---
#include "src/tests/unit/test_as_ability.h"
#include "src/tests/unit/test_as_attribute.h"
#include "src/tests/unit/test_as_attribute_set.h"
#include "src/tests/unit/test_as_container_package.h"
#include "src/tests/unit/test_as_effect.h"

// --- Unit: Runtime Specs & Tag System ---
#include "src/tests/unit/test_as_ability_spec.h"
#include "src/tests/unit/test_as_effect_spec.h"
#include "src/tests/unit/test_as_tag_spec.h"
#include "src/tests/unit/test_as_tag_utils.h"

// --- Unit: Core Component Hub ---
#include "src/tests/unit/test_as_component.h"

// --- Unit: Base infrastructure ---
#include "src/tests/unit/test_as_base.h"

// --- Integration: Gameplay Scenarios ---
#include "src/tests/unit/test_gameplay_scenarios.h"

int run_gdextension_tests() {
	doctest::Context context;
	context.setOption("force-colors", true);
	int res = context.run();
	return res;
}

#endif // ABILITY_SYSTEM_GDEXTENSION
