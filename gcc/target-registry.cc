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
#include "diagnostic-core.h"
#include "target-registry.h"

/* The descriptor of the configured target lives in
   target-backend-def.cc; a multi-target build compiles that file
   once per enabled target.  */

#if ENABLE_MULTI_TARGET
/* The backend the compiler is currently addressing.  Activation will
   install other entries; until then the compiler behaves exactly like
   a single-target build of the primary.  */
const struct target_backend *this_target_backend
  = &default_target_backend;

/* The descriptors of the enabled targets, one per tag, compiled from
   target-backend-def.cc inside each target's own header context.  */
#define MT_BACKEND(tag) extern const struct target_backend mt_backend_##tag;
#include "mt-backend-list.h"
#undef MT_BACKEND
#endif

const struct target_backend *const target_backend_registry[] =
{
  &default_target_backend,
#if ENABLE_MULTI_TARGET
  /* The primary's own descriptor appears here too, under its tag; a
     lookup by triple finds the entry above first.  */
# define MT_BACKEND(tag) &mt_backend_##tag,
# include "mt-backend-list.h"
# undef MT_BACKEND
#endif
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

/* Make the backend built for BY_TRIPLE the one the compiler
   addresses.  Runs before option decoding, so the diagnostics
   machinery is up but nothing target-dependent has been read.  */

void
activate_target_backend (const char *by_triple)
{
  const struct target_backend *backend = find_target_backend (by_triple);

  if (backend == NULL)
    {
      /* The registry lists the primary once more under its own
	 tag; name each triplet once.  */
      char *known = NULL;
      for (unsigned int index = 0; index < target_backend_count;
	   index++)
	{
	  const char *triple = target_backend_registry[index]->triple;
	  bool seen = false;
	  for (unsigned int prior = 0; prior < index; prior++)
	    if (strcmp (target_backend_registry[prior]->triple,
			triple) == 0)
	      seen = true;
	  if (seen)
	    continue;
	  known = (known == NULL ? xstrdup (triple)
		   : reconcat (known, known, " ", triple, NULL));
	}
      fatal_error (UNKNOWN_LOCATION,
		   "target %qs is not built into this compiler"
		   " (built-in targets: %s)", by_triple, known);
    }

  if (backend == target_backend_registry[0])
    {
      /* The primary is what the compiler addresses from the start;
	 selecting it completes here.  A single-target build routes
	 every reference to the sole descriptor statically.  */
#if ENABLE_MULTI_TARGET
      this_target_backend = backend;
#endif
      return;
    }

  /* The installation of a secondary backend's surface lands with the
     activation machinery proper.  */
  fatal_error (UNKNOWN_LOCATION,
	       "activation of the secondary target %qs is not yet"
	       " implemented", by_triple);
}
