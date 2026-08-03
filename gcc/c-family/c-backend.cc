/* Registry of the C-family surfaces of the targets built into the
   compiler.
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
#include "diagnostic-core.h"
#include "target-backend.h"
#include "c-family/c-backend.h"

/* The primary target's surface, compiled from c-backend-def.cc in
   the host's own header context.  */
extern const struct mt_c_backend default_c_backend;

/* The other targets' surfaces.  The declarations are weak: a surface
   only links into the binaries of the front ends that route through
   it, and a binary that does not carry a target's C-family objects
   reports the target as unsupported instead of failing to link.  */
#define MT_BACKEND(tag) \
  extern const struct mt_c_backend mt_c_backend_##tag \
    __attribute__ ((weak));
#include "mt-backend-list.h"
#undef MT_BACKEND

static const struct mt_c_backend *const mt_c_backend_registry[] =
{
#define MT_BACKEND(tag) &mt_c_backend_##tag,
#include "mt-backend-list.h"
#undef MT_BACKEND
};

/* Find the active target's C-family surface.  The primary target's
   surface is matched by identity, mirroring the backend registry,
   which resolves the primary triplet to the default descriptor.
   A missing surface is reported to the user: it means this front
   end does not carry the requested target's C-family objects.  */

static const struct mt_c_backend *
find_c_backend (void)
{
  if (this_target_backend == &default_target_backend)
    return &default_c_backend;
  for (size_t index = 0; index < ARRAY_SIZE (mt_c_backend_registry);
       index++)
    {
      const struct mt_c_backend *backend = mt_c_backend_registry[index];
      if (backend != NULL
	  && backend->triple != NULL
	  && strcmp (backend->triple, this_target_backend->triple) == 0)
	return backend;
    }
  fatal_error (UNKNOWN_LOCATION,
	       "target %qs has no C-family support in this compiler",
	       this_target_backend->triple);
}

/* Invoke TARGET_CPU_CPP_BUILTINS for the active target.  */

void
mt_c_cpu_cpp_builtins (struct cpp_reader *pfile)
{
  find_c_backend ()->x_cpu_cpp_builtins (pfile);
}

/* Invoke REGISTER_TARGET_PRAGMAS for the active target.  */

void
mt_c_register_pragmas (void)
{
  const struct mt_c_backend *backend = find_c_backend ();
  if (backend->x_register_pragmas != NULL)
    backend->x_register_pragmas ();
}
