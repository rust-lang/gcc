/* Descriptors for the targets built into the compiler.
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

#ifndef GCC_TARGET_BACKEND_H
#define GCC_TARGET_BACKEND_H

/* Generator programs compile against the target's own headers and
   never address a backend descriptor; for them this header is
   empty.  */
#ifndef GENERATOR_FILE

struct gcc_target;

/* Everything the compiler needs in order to address one built-in
   target.  A single-target build has exactly one instance, describing
   the configured target; a multi-target build registers one instance
   per enabled target.  The structure grows fields as core consumers
   are converted to reach target-specific code through it.  */

struct target_backend
{
  /* The canonical target triplet this backend was built for.  */
  const char *triple;

  /* The backend's target hook vector.  */
  struct gcc_target *target_vector;
};

#endif /* GENERATOR_FILE */
#endif
