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
/* This unit seeds the runtime register counts from the primary's
   native constants; see defaults.h.  */
#define MT_NATIVE_REGISTER_CONSTANTS 1
#include "tm.h"
#include "diagnostic-core.h"
/* This unit repoints the active option tables; see opts.h.  */
#define MT_OWN_OPTION_TABLES 1
#include "opts.h"
#include "common/common-target.h"
#include "ggc.h"
#include "mode-tables.h"
#include "real.h"
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

/* Host code reaches the active hook vector here; see target.h.
   The primary's vector carries the name target-def.h gives it.  */
extern struct gcc_target mt_targetm;
struct gcc_target *mt_targetm_pnt = &mt_targetm;

/* The active target's register counts — FIRST_PSEUDO_REGISTER and
   N_REG_CLASSES for host code; see defaults.h.  Activation
   installs the selected target's values.  */
unsigned int mt_first_pseudo_register = FIRST_PSEUDO_REGISTER;
int mt_n_reg_classes = N_REG_CLASSES;

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


#if ENABLE_MULTI_TARGET
/* Copy TABLES, the activated target's machine mode value tables, over
   the shared tables host code reads.  The target's own runtime
   adjustments run through the descriptor at the established point of
   the initialization sequence (do_compile).  */

static void
install_mode_tables (const struct mode_tables *tables)
{
  for (unsigned int index = 0; index < NUM_MACHINE_MODES; index++)
    {
      mode_precision[index] = tables->mode_precision[index];
      mode_size[index] = tables->mode_size[index];
      mode_nunits[index] = tables->mode_nunits[index];
    }
  memcpy (mode_next, tables->mode_next, sizeof (mode_next));
  memcpy (mode_wider, tables->mode_wider, sizeof (mode_wider));
  memcpy (mode_2xwider, tables->mode_2xwider, sizeof (mode_2xwider));
  memcpy (mode_inner, tables->mode_inner, sizeof (mode_inner));
  memcpy (mode_unit_size, tables->mode_unit_size,
	  sizeof (mode_unit_size));
  memcpy (mode_unit_precision, tables->mode_unit_precision,
	  sizeof (mode_unit_precision));
  memcpy (mode_complex, tables->mode_complex, sizeof (mode_complex));
  memcpy (mode_mask_array, tables->mode_mask_array,
	  sizeof (mode_mask_array));
  memcpy (mode_ibit, tables->mode_ibit, sizeof (mode_ibit));
  memcpy (mode_fbit, tables->mode_fbit, sizeof (mode_fbit));
  memcpy (mode_base_align, tables->mode_base_align,
	  sizeof (mode_base_align));
  memcpy (mode_present, tables->mode_present, sizeof (mode_present));
  memcpy (class_narrowest_mode, tables->class_narrowest_mode,
	  sizeof (class_narrowest_mode));
  memcpy (real_format_for_mode, tables->real_format_for_mode,
	  sizeof (real_format_for_mode));
}

/* The backend whose GTY roots have been registered; roots register
   once per process.  */
static const struct target_backend *gt_roots_registered;

/* Install BACKEND's option machinery: the decode and enumeration
   tables, the name-order permutation, the built-in defaults image
   and the generated handlers.  Runs before anything reads an option
   table; the state the tables describe is decoded afterwards, into
   the shared padded gcc_options.  */

static void
install_target_backend (const struct target_backend *backend)
{
  this_target_backend = backend;

  mt_targetm_pnt = backend->target_vector;
  mt_targetm_common_pnt = backend->x_targetm_common;

  mt_active_cl_options = backend->x_cl_options;
  mt_active_cl_options_count = backend->x_cl_options_count;
  mt_active_cl_enums = backend->x_cl_enums;
  mt_active_cl_enums_count = backend->x_cl_enums_count;
  mt_active_cl_option_name_order = backend->x_cl_option_name_order;
  mt_active_global_options_init = backend->x_global_options_init;
  mt_active_common_handle_option_auto
    = backend->x_common_handle_option_auto;
  mt_active_cpp_handle_option_auto
    = backend->x_cpp_handle_option_auto;
  mt_active_init_global_opts_from_cpp
    = backend->x_init_global_opts_from_cpp;

  if (backend->mode_tables != NULL)
    install_mode_tables (backend->mode_tables);

  /* Register the target's GTY roots the way plugin roots register.
     The primary carries no vector here; its roots live in the host
     root tables.  Re-activation (libgccjit) must not register a
     vector twice; switching back to a previously active backend is
     a later phase's concern.  */
  if (backend->gt_ggc_roots != NULL && gt_roots_registered != backend)
    {
      gcc_assert (gt_roots_registered == NULL);
      gt_roots_registered = backend;
      for (const struct ggc_root_tab *const *table
	     = backend->gt_ggc_roots;
	   *table != NULL; table++)
	ggc_register_root_tab (*table);
    }
}
#endif

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
      install_target_backend (backend);
#endif
      return;
    }

  /* The installation of a secondary backend's surface lands with the
     activation machinery proper.  */
  fatal_error (UNKNOWN_LOCATION,
	       "activation of the secondary target %qs is not yet"
	       " implemented", by_triple);
}
