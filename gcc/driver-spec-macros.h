/* Spec fragments shared between the driver and per-target captures.
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

#ifndef GCC_DRIVER_SPEC_MACROS_H
#define GCC_DRIVER_SPEC_MACROS_H

/* The PIE-related option-matching fragments target macros compose
   their specs from.  The driver has always defined them ahead of the
   spec strings it bakes in; a multi-target build also expands them
   in driver-specs.cc, where each enabled target's spec macros are
   captured inside that target's own header context.  */

#ifdef ENABLE_DEFAULT_PIE
#define PIE_SPEC		"!no-pie"
#define NO_FPIE1_SPEC		"fno-pie"
#define FPIE1_SPEC		NO_FPIE1_SPEC ":;"
#define NO_FPIE2_SPEC		"fno-PIE"
#define FPIE2_SPEC		NO_FPIE2_SPEC ":;"
#define NO_FPIE_SPEC		NO_FPIE1_SPEC "|" NO_FPIE2_SPEC
#define FPIE_SPEC		NO_FPIE_SPEC ":;"
#define NO_FPIC1_SPEC		"fno-pic"
#define FPIC1_SPEC		NO_FPIC1_SPEC ":;"
#define NO_FPIC2_SPEC		"fno-PIC"
#define FPIC2_SPEC		NO_FPIC2_SPEC ":;"
#define NO_FPIC_SPEC		NO_FPIC1_SPEC "|" NO_FPIC2_SPEC
#define FPIC_SPEC		NO_FPIC_SPEC ":;"
#define NO_FPIE1_AND_FPIC1_SPEC	NO_FPIE1_SPEC "|" NO_FPIC1_SPEC
#define FPIE1_OR_FPIC1_SPEC	NO_FPIE1_AND_FPIC1_SPEC ":;"
#define NO_FPIE2_AND_FPIC2_SPEC	NO_FPIE2_SPEC "|" NO_FPIC2_SPEC
#define FPIE2_OR_FPIC2_SPEC	NO_FPIE2_AND_FPIC2_SPEC ":;"
#define NO_FPIE_AND_FPIC_SPEC	NO_FPIE_SPEC "|" NO_FPIC_SPEC
#define FPIE_OR_FPIC_SPEC	NO_FPIE_AND_FPIC_SPEC ":;"
#else
#define PIE_SPEC		"pie"
#define FPIE1_SPEC		"fpie"
#define NO_FPIE1_SPEC		FPIE1_SPEC ":;"
#define FPIE2_SPEC		"fPIE"
#define NO_FPIE2_SPEC		FPIE2_SPEC ":;"
#define FPIE_SPEC		FPIE1_SPEC "|" FPIE2_SPEC
#define NO_FPIE_SPEC		FPIE_SPEC ":;"
#define FPIC1_SPEC		"fpic"
#define NO_FPIC1_SPEC		FPIC1_SPEC ":;"
#define FPIC2_SPEC		"fPIC"
#define NO_FPIC2_SPEC		FPIC2_SPEC ":;"
#define FPIC_SPEC		FPIC1_SPEC "|" FPIC2_SPEC
#define NO_FPIC_SPEC		FPIC_SPEC ":;"
#define FPIE1_OR_FPIC1_SPEC	FPIE1_SPEC "|" FPIC1_SPEC
#define NO_FPIE1_AND_FPIC1_SPEC	FPIE1_OR_FPIC1_SPEC ":;"
#define FPIE2_OR_FPIC2_SPEC	FPIE2_SPEC "|" FPIC2_SPEC
#define NO_FPIE2_AND_FPIC2_SPEC	FPIE1_OR_FPIC2_SPEC ":;"
#define FPIE_OR_FPIC_SPEC	FPIE_SPEC "|" FPIC_SPEC
#define NO_FPIE_AND_FPIC_SPEC	FPIE_OR_FPIC_SPEC ":;"
#endif

#endif
