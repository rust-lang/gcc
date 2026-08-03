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

#ifndef GCC_TARGET_REGISTRY_H
#define GCC_TARGET_REGISTRY_H

#include "target-backend.h"

/* All targets built into this compiler; the configured target comes
   first.  */
extern const struct target_backend *const target_backend_registry[];
extern const unsigned int target_backend_count;

/* Look a backend up by its canonical target triplet; return null when
   the triplet is not built into this compiler.  */
extern const struct target_backend *find_target_backend (const char *);

/* Make the backend built for a triplet the one the compiler
   addresses; an unknown triplet is a fatal error.  */
extern void activate_target_backend (const char *);

/* Note the backend whose init_machine_status allocated a
   machine_function; garbage collection walks the object with its
   allocator's marker.  */
extern void mt_record_machine_function_owner (void *);

#endif
