/* Registry of the targets built into the compiler.
   Copyright (C) 2026 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target-registry.h"

/* The descriptor of the configured target lives in
   target-backend-def.cc; a multi-target build compiles that file
   once per enabled target.  */

#if ENABLE_MULTI_TARGET
/* The backend the compiler is currently addressing.  */
const struct target_backend *this_target_backend
  = &default_target_backend;
#endif

const struct target_backend *const target_backend_registry[] =
{
  &default_target_backend
};

const unsigned int target_backend_count
  = ARRAY_SIZE (target_backend_registry);

/* Look BY_TRIPLE up in the registry; return null when no built-in
   target matches.  */

const struct target_backend *
find_target_backend (const char *by_triple)
{
  for (unsigned int index = 0; index < target_backend_count; index++)
    if (strcmp (target_backend_registry[index]->triple, by_triple) == 0)
      return target_backend_registry[index];
  return NULL;
}
