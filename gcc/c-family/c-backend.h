/* C-family surface of a target built into the compiler.
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

#ifndef GCC_C_BACKEND_H
#define GCC_C_BACKEND_H

struct cpp_reader;

/* The tm.h hooks the C family invokes on the target, captured as
   callables so a multi-target compiler can hold one set per built-in
   target.  Unlike the backend descriptor, this surface links into the
   C-family binaries only, keeping the targets' blobs independent of
   any front end.  */

struct mt_c_backend
{
  /* The canonical target triplet this surface was compiled for, or
     NULL for the primary target, whose surface is found by identity
     rather than by name.  */
  const char *triple;

  /* TARGET_CPU_CPP_BUILTINS, with the reader its call site in
     c_cpp_builtins passes implicitly.  */
  void (*x_cpu_cpp_builtins) (struct cpp_reader *);

  /* REGISTER_TARGET_PRAGMAS, or NULL when the target defines no
     pragmas.  */
  void (*x_register_pragmas) (void);
};

extern void mt_c_cpu_cpp_builtins (struct cpp_reader *);
extern void mt_c_register_pragmas (void);

#endif /* GCC_C_BACKEND_H */
