/**************************************************************************/
/*  test_tools.h                                                          */
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
#include "src/tests/doctest.h"
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#else
#include "core/error/error_macros.h"
#include "modules/ability_system/tests/doctest.h"
#endif

#ifdef ABILITY_SYSTEM_GDEXTENSION
using namespace godot;
#endif

namespace ability_system {
namespace tests {

// Godot-like ErrorDetector ported to AbilitySystem.
// NOTE: Natively intercepting errors via ErrorHandlerList is technically only possible in Module builds.
// For GDExtension, we dummy this class unless Godot-cpp exposes internal error hooks in the future.
struct ErrorDetector {
#ifndef ABILITY_SYSTEM_GDEXTENSION
	ErrorDetector() {
		eh.errfunc = _detect_error;
		eh.userdata = this;
		add_error_handler(&eh);
	}

	~ErrorDetector() {
		remove_error_handler(&eh);
	}

	void clear() {
		has_error = false;
	}

	static void _detect_error(void *p_self, const char *p_func, const char *p_file, int p_line, const char *p_error, const char *p_errorexp, bool p_editor_notify, ErrorHandlerType p_type) {
		ErrorDetector *self = static_cast<ErrorDetector *>(p_self);
		self->has_error = true;
	}

	ErrorHandlerList eh;
#else
	ErrorDetector() {}
	~ErrorDetector() {}
	void clear() { has_error = false; }
#endif

	bool has_error = false;
};

} // namespace tests
} // namespace ability_system

// Macro to expect errors gracefully across both modes.
#define CHECK_UNEXPECTED_ERRORS()                                                              \
	do {                                                                                       \
		ability_system::tests::ErrorDetector err_detector;                                     \
		CHECK_FALSE_MESSAGE(err_detector.has_error, "An unexpected error occurred in Godot."); \
	} while (0)
