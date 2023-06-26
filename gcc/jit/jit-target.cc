/* jit-target.cc -- Target interface for the jit front end.
   Copyright (C) 2023 Free Software Foundation, Inc.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"

#include "tree.h"
#include "memmodel.h"
#include "fold-const.h"
#include "diagnostic.h"
#include "stor-layout.h"
#include "tm.h"
#include "tm_p.h"
#include "target.h"
#include "calls.h"

#include "jit-target.h"

#include <algorithm>

static target_info jit_target_info;

/* Initialize all variables of the Target structure.  */

void
jit_target_init ()
{
  /* Initialize target info tables, the keys required by the language are added
     last, so that the OS and CPU handlers can override.  */
  targetjitm.jit_register_cpu_target_info ();
  targetjitm.jit_register_os_target_info ();
}

/* Add all target info in HANDLERS to JIT_TARGET_INFO for use by
   jit_has_target_value().  */

void
jit_add_target_info (const char *key, const char *value)
{
  if (jit_target_info.m_info.find (key) == jit_target_info.m_info.end())
    jit_target_info.m_info.insert ({key, {value}});
  else
    jit_target_info.m_info[key].insert(value);
}

void
jit_target_set_arch (const char* arch)
{
  jit_target_info.m_arch = arch;
}

void
jit_target_set_128bit_int_support (bool support)
{
  jit_target_info.m_supports_128bit_int = support;
}

target_info::~target_info()
{
  free (const_cast<void *> ((const void *) m_arch));
}

target_info *
jit_get_target_info ()
{
  target_info *info = new target_info {std::move(jit_target_info)};
  jit_target_info = target_info{};
  return info;
}

bool
target_info::has_target_value (const char *key, const char *value)
{
  if (m_info.find (key) == m_info.end ())
    return false;

  auto& set = m_info[key];
  return set.find (value) != set.end ();
}
