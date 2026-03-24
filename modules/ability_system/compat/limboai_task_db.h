/**************************************************************************/
/*  limboai_task_db.h                                                     */
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

/**
 * compat/limboai_task_db.h
 * =============================================================================
 * Compatibility wrapper for LimboAI Task Database
 *
 * When LIMBOAI_MODULE is defined, includes the real LimboAI headers.
 * When not available, provides stub implementations for compilation.
 * =============================================================================
 */

#ifndef COMPAT_LIMBOAI_TASK_DB_H
#define COMPAT_LIMBOAI_TASK_DB_H

#if defined(LIMBOAI_MODULE) || defined(LIMBOAI_GDEXTENSION)
// Real LimboAI available — use actual headers
#ifdef LIMBOAI_GDEXTENSION
#include "util/limbo_task_db.h"
#else
#include "modules/limboai/util/limbo_task_db.h"
#endif
#else
// Stub implementations follow

#include "../scene/as_component.h"

#ifdef ABILITY_SYSTEM_GDEXTENSION
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
using namespace godot;
#else
#include "core/object/ref_counted.h"
#include "core/string/string_name.h"
#include "core/string/ustring.h"
#endif

/**
 * LimboTaskDB - Task registration and discovery system
 *
 * Compatibility wrapper for LimboAI's task database.
 * When LimboAI is available, forwards to the real TaskDB.
 * When not available, provides stub implementations for compilation.
 */
class LimboTaskDB : public RefCounted {
	GDCLASS(LimboTaskDB, RefCounted)

private:
	static LimboTaskDB *singleton;

protected:
	static void _bind_methods();

public:
	/**
	 * Returns the global LimboTaskDB singleton.
	 * Returns nullptr if LimboAI is not available.
	 */
	static LimboTaskDB *get_singleton() { return singleton; }

	/**
	 * Registers a custom task class with the LimboAI system.
	 * Only works when LimboAI is available.
	 */
	virtual void register_task(const StringName &p_class_name);

	/**
	 * Checks if a task class is registered.
	 */
	virtual bool is_task_registered(const StringName &p_class_name) const;

	/**
	 * Gets all registered task class names.
	 */
	virtual Array get_registered_tasks() const;

	/**
	 * Checks if LimboAI is available and TaskDB is functional.
	 */
	virtual bool is_available() const;

	LimboTaskDB();
	~LimboTaskDB();
};

#endif // !LIMBOAI_MODULE

#endif // COMPAT_LIMBOAI_TASK_DB_H
