/* Merge the mode tables of several targets into one union table.
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

/* A multi-target build has one genmodes binary per enabled target, and
   each dumps its complete mode table with -X.  This tool reads those
   dumps and computes the union table: every mode that any target
   defines, with a class conflict being a hard error, and for every
   mode the classification of each property as uniform (every
   contributing target agrees and none adjusts it at run time) or
   per-target.  The link chains (component, wider, complex) and the
   floating point formats are per-target data by design: the prototype
   targets already disagree on both.

   With -h the tool emits the union insn-modes.h, with -i the
   matching insn-modes-inline.h, and with -d <target> the value
   tables of one target at the union numbering (mode-tables.h);
   without a flag, a report of the union.

   Usage: genmodes-merge [-h|-i|-d <target>] <name>=<dump> ...  */

#define INCLUDE_STRING
#define INCLUDE_VECTOR
#define INCLUDE_MAP
#define INCLUDE_ALGORITHM
#include "bconfig.h"
#include "system.h"
#include "errors.h"

/* enum mode_class is normally defined by machmode.h; genmodes and this
   tool define it for themselves from the same master file.  */
#include "mode-classes.def"

#define DEF_MODE_CLASS(M) M
enum mode_class { MODE_CLASSES, MAX_MODE_CLASS };
#undef DEF_MODE_CLASS

/* Text names of mode classes, for input and output.  */
#define DEF_MODE_CLASS(M) #M
static const char *const mode_class_names[MAX_MODE_CLASS] =
{
  MODE_CLASSES
};
#undef DEF_MODE_CLASS
#undef MODE_CLASSES

/* One target's view of one mode, as read from its dump.  */

struct target_mode
{
  bool present;
  unsigned int precision;
  unsigned int bytesize;
  unsigned int ncomponents;
  unsigned int alignment;
  unsigned int ibit;
  unsigned int fbit;
  unsigned int int_n;
  unsigned int boolean_flag;
  std::string format;
  std::string component;
  std::string wider;
  std::string complex_mode;
  bool adjusted_nunits;
  bool adjusted_bytesize;
  bool adjusted_alignment;
  bool adjusted_format;
  bool adjusted_ibit;
  bool adjusted_fbit;
  bool adjusted_precision;
};

/* One mode of the union table.  */

struct union_mode
{
  std::string name;
  int cl;
  size_t first_seen;
  std::vector<target_mode> per_target;
};

static std::vector<std::string> target_names;
static std::vector<union_mode> union_modes;
static std::map<std::string, size_t> union_index_by_name;

/* Global parameters of the union table, from the dumps' param
   records: the unit size must agree, the maxima and the poly_int
   coefficient count are superset maxima.  */
static unsigned int bits_per_unit_param;
static unsigned int max_bitsize_any_int_param;
static unsigned int max_bitsize_any_mode_param;
static unsigned int poly_int_coeffs_param = 1;

/* Split LINE into whitespace-separated tokens.  */

static std::vector<std::string>
split_tokens (const std::string &line)
{
  std::vector<std::string> tokens;
  size_t position = 0;
  while (position < line.size ())
    {
      size_t start = line.find_first_not_of (' ', position);
      if (start == std::string::npos)
	break;
      size_t end = line.find (' ', start);
      if (end == std::string::npos)
	end = line.size ();
      tokens.push_back (line.substr (start, end - start));
      position = end;
    }
  return tokens;
}

/* Return the class whose name is TEXT, or fail.  */

static int
class_by_name (const char *file, const std::string &text)
{
  for (int c = 0; c < MAX_MODE_CLASS; c++)
    if (text == mode_class_names[c])
      return c;
  fatal ("%s: unknown mode class %s", file, text.c_str ());
}

/* Return the union slot for mode NAME of class CL contributed by the
   dump FILE, creating it on first sight and checking the class on
   every later one.  */

static union_mode *
union_slot (const char *file, const std::string &name, int cl)
{
  std::map<std::string, size_t>::iterator found
    = union_index_by_name.find (name);
  if (found == union_index_by_name.end ())
    {
      union_index_by_name[name] = union_modes.size ();
      union_modes.push_back (union_mode ());
      union_mode *mode = &union_modes.back ();
      mode->name = name;
      mode->cl = cl;
      mode->first_seen = union_modes.size () - 1;
      mode->per_target.resize (target_names.size ());
      return mode;
    }
  union_mode *mode = &union_modes[found->second];
  if (mode->cl != cl)
    fatal ("%s: mode %s has class %s here but class %s elsewhere",
	   file, name.c_str (), mode_class_names[cl],
	   mode_class_names[mode->cl]);
  return mode;
}

/* Read one -X dump for the target numbered TARGET from FILE.  */

static void
read_dump (const char *file, size_t target)
{
  FILE *stream = fopen (file, "r");
  if (!stream)
    fatal ("cannot open %s", file);

  char buffer[4096];
  bool saw_version = false;
  unsigned int line_number = 0;
  while (fgets (buffer, sizeof buffer, stream))
    {
      line_number++;
      std::string line (buffer);
      if (!line.empty () && line.back () == '\n')
	line.pop_back ();
      else if (!feof (stream))
	fatal ("%s:%u: line too long", file, line_number);

      if (!saw_version)
	{
	  if (line != "genmodes-dump 1")
	    fatal ("%s is not a genmodes -X dump", file);
	  saw_version = true;
	  continue;
	}

      std::vector<std::string> tokens = split_tokens (line);
      if (tokens.empty ())
	continue;
      if (tokens[0] == "mode")
	{
	  if (tokens.size () < 3)
	    fatal ("%s:%u: truncated mode record", file, line_number);
	  std::map<std::string, std::string> fields;
	  for (size_t i = 2; i < tokens.size (); i++)
	    {
	      size_t equals = tokens[i].find ('=');
	      if (equals == std::string::npos)
		fatal ("%s:%u: malformed field %s", file, line_number,
		       tokens[i].c_str ());
	      fields[tokens[i].substr (0, equals)]
		= tokens[i].substr (equals + 1);
	    }
	  union_mode *mode
	    = union_slot (file, tokens[1],
			  class_by_name (file, fields["class"]));
	  target_mode *record = &mode->per_target[target];
	  if (record->present)
	    fatal ("%s:%u: duplicate mode %s", file, line_number,
		   tokens[1].c_str ());
	  record->present = true;
	  record->precision = strtoul (fields["precision"].c_str (), NULL, 10);
	  record->bytesize = strtoul (fields["bytesize"].c_str (), NULL, 10);
	  record->ncomponents
	    = strtoul (fields["ncomponents"].c_str (), NULL, 10);
	  record->alignment = strtoul (fields["alignment"].c_str (), NULL, 10);
	  record->ibit = strtoul (fields["ibit"].c_str (), NULL, 10);
	  record->fbit = strtoul (fields["fbit"].c_str (), NULL, 10);
	  record->int_n = strtoul (fields["int_n"].c_str (), NULL, 10);
	  record->boolean_flag
	    = strtoul (fields["boolean"].c_str (), NULL, 10);
	  record->format = fields["format"];
	  record->component = fields["component"];
	  record->wider = fields["wider"];
	  record->complex_mode = fields["complex"];
	}
      else if (tokens[0] == "adjust")
	{
	  if (tokens.size () < 4)
	    fatal ("%s:%u: truncated adjust record", file, line_number);
	  std::map<std::string, size_t>::iterator found
	    = union_index_by_name.find (tokens[2]);
	  if (found == union_index_by_name.end ())
	    fatal ("%s:%u: adjustment of unknown mode %s", file,
		   line_number, tokens[2].c_str ());
	  target_mode *record
	    = &union_modes[found->second].per_target[target];
	  if (tokens[1] == "nunits")
	    record->adjusted_nunits = true;
	  else if (tokens[1] == "bytesize")
	    record->adjusted_bytesize = true;
	  else if (tokens[1] == "alignment")
	    record->adjusted_alignment = true;
	  else if (tokens[1] == "format")
	    record->adjusted_format = true;
	  else if (tokens[1] == "ibit")
	    record->adjusted_ibit = true;
	  else if (tokens[1] == "fbit")
	    record->adjusted_fbit = true;
	  else if (tokens[1] == "precision")
	    record->adjusted_precision = true;
	  else
	    fatal ("%s:%u: unknown adjustment kind %s", file, line_number,
		   tokens[1].c_str ());
	}
      else if (tokens[0] == "param")
	{
	  if (tokens.size () != 3)
	    fatal ("%s:%u: truncated param record", file, line_number);
	  unsigned int value = strtoul (tokens[2].c_str (), NULL, 10);
	  if (tokens[1] == "bits_per_unit")
	    {
	      if (bits_per_unit_param && bits_per_unit_param != value)
		fatal ("%s: bits_per_unit %u does not match %u", file,
		       value, bits_per_unit_param);
	      bits_per_unit_param = value;
	    }
	  else if (tokens[1] == "max_bitsize_mode_any_int")
	    max_bitsize_any_int_param = MAX (max_bitsize_any_int_param, value);
	  else if (tokens[1] == "max_bitsize_mode_any_mode")
	    max_bitsize_any_mode_param
	      = MAX (max_bitsize_any_mode_param, value);
	  else if (tokens[1] == "poly_int_coeffs")
	    poly_int_coeffs_param = MAX (poly_int_coeffs_param, value);
	  else
	    fatal ("%s:%u: unknown param %s", file, line_number,
		   tokens[1].c_str ());
	}
      else
	fatal ("%s:%u: unrecognized record %s", file, line_number,
	       tokens[0].c_str ());
    }
  if (!saw_version)
    fatal ("%s is empty", file);
  fclose (stream);
}

/* Append PROPERTY to VARIANTS.  */

static void
add_variant (std::string *variants, const char *property)
{
  if (!variants->empty ())
    *variants += ' ';
  *variants += property;
}

/* Compute the properties of MODE that are not uniform across the
   contributing targets, excluding the link chains.  */

static std::string
variant_properties (const union_mode &mode)
{
  const target_mode *first = NULL;
  bool precision_varies = false, bytesize_varies = false;
  bool ncomponents_varies = false, alignment_varies = false;
  bool ibit_varies = false, fbit_varies = false;
  bool int_n_varies = false, boolean_varies = false;
  bool format_varies = false;
  bool adjusted_nunits = false, adjusted_bytesize = false;
  bool adjusted_alignment = false, adjusted_format = false;
  bool adjusted_ibit = false, adjusted_fbit = false;
  bool adjusted_precision = false;

  for (size_t target = 0; target < target_names.size (); target++)
    {
      const target_mode *record = &mode.per_target[target];
      if (!record->present)
	continue;
      adjusted_nunits |= record->adjusted_nunits;
      adjusted_bytesize |= record->adjusted_bytesize;
      adjusted_alignment |= record->adjusted_alignment;
      adjusted_format |= record->adjusted_format;
      adjusted_ibit |= record->adjusted_ibit;
      adjusted_fbit |= record->adjusted_fbit;
      adjusted_precision |= record->adjusted_precision;
      if (!first)
	{
	  first = record;
	  continue;
	}
      precision_varies |= record->precision != first->precision;
      bytesize_varies |= record->bytesize != first->bytesize;
      ncomponents_varies |= record->ncomponents != first->ncomponents;
      alignment_varies |= record->alignment != first->alignment;
      ibit_varies |= record->ibit != first->ibit;
      fbit_varies |= record->fbit != first->fbit;
      int_n_varies |= record->int_n != first->int_n;
      boolean_varies |= record->boolean_flag != first->boolean_flag;
      format_varies |= record->format != first->format;
    }

  std::string variants;
  if (precision_varies || adjusted_precision)
    add_variant (&variants, "precision");
  if (bytesize_varies || adjusted_bytesize)
    add_variant (&variants, "bytesize");
  if (ncomponents_varies || adjusted_nunits)
    add_variant (&variants, "ncomponents");
  if (alignment_varies || adjusted_alignment)
    add_variant (&variants, "alignment");
  if (ibit_varies || adjusted_ibit)
    add_variant (&variants, "ibit");
  if (fbit_varies || adjusted_fbit)
    add_variant (&variants, "fbit");
  if (int_n_varies)
    add_variant (&variants, "int_n");
  if (boolean_varies)
    add_variant (&variants, "boolean");
  if (format_varies || adjusted_format)
    add_variant (&variants, "format");
  return variants;
}

/* True if the link chains of MODE differ between contributing
   targets.  */

static bool
links_vary_p (const union_mode &mode)
{
  const target_mode *first = NULL;
  for (size_t target = 0; target < target_names.size (); target++)
    {
      const target_mode *record = &mode.per_target[target];
      if (!record->present)
	continue;
      if (!first)
	{
	  first = record;
	  continue;
	}
      if (record->component != first->component
	  || record->wider != first->wider
	  || record->complex_mode != first->complex_mode)
	return true;
    }
  return false;
}

/* The union index of NAME, which must exist.  */

static size_t
union_index_of (const std::string &name)
{
  std::map<std::string, size_t>::iterator found
    = union_index_by_name.find (name);
  if (found == union_index_by_name.end ())
    fatal ("mode %s is not in the union", name.c_str ());
  return found->second;
}

/* The union index of the mode that MODE_INDEX's unit size and
   precision come from for TARGET: the component, except that
   MODE_PARTIAL_INT modes are their own unit, as in genmodes.cc.  */

static size_t
unit_index (size_t mode_index, size_t target)
{
  const union_mode &mode = union_modes[mode_index];
  const target_mode &record = mode.per_target[target];
  if (mode.cl != MODE_PARTIAL_INT && record.component != "-")
    return union_index_of (record.component);
  return mode_index;
}

/* True if MODE_INDEX's byte size is adjusted at run time for TARGET:
   directly, through a unit count adjustment, or through a size
   adjustment of its component — the propagation to containing modes
   of genmodes.cc's emit_mode_size_inline.  */

static bool
bytesize_adjusted_p (size_t mode_index, size_t target)
{
  const union_mode &mode = union_modes[mode_index];
  const target_mode &record = mode.per_target[target];
  if (!record.present)
    return false;
  if (record.adjusted_bytesize || record.adjusted_nunits)
    return true;
  if (record.component != "-")
    {
      size_t component = union_index_of (record.component);
      if (union_modes[component].per_target[target].adjusted_bytesize)
	return true;
    }
  return false;
}

/* Emit the shared head of one inline accessor: RESULT_TYPE
   FUNCTION_NAME, falling back to the table declared as TABLE_EXTERN.  */

static void
emit_inline_head (const char *result_type, const char *function_name,
		  const std::string &table_extern, bool with_assert)
{
  printf ("#ifdef __cplusplus\n"
	  "inline __attribute__((__always_inline__))\n"
	  "#else\n"
	  "extern __inline__ __attribute__((__always_inline__,"
	  " __gnu_inline__))\n"
	  "#endif\n"
	  "%s\n"
	  "%s (machine_mode mode)\n"
	  "{\n"
	  "  extern %s[NUM_MACHINE_MODES];\n", result_type, function_name,
	  table_extern.c_str ());
  if (with_assert)
    printf ("  gcc_assert (mode >= 0 && mode < NUM_MACHINE_MODES);\n");
  printf ("  switch (mode)\n    {\n");
}

/* Emit the shared tail: the fall back to TABLE_NAME.  */

static void
emit_inline_tail (const char *table_name)
{
  printf ("    default: return %s[mode];\n    }\n}\n\n", table_name);
}

/* Emit the union insn-modes-inline.h, mirroring genmodes.cc's
   emit_insn_modes_inline_h.  A mode gets an inline fast path only when
   every contributing target computes the same value and none adjusts
   it at run time; everything else falls back to the table, which
   activation fills with the active target's values.  */

static void
emit_union_inline_header (void)
{
  printf ("/* Generated automatically by genmodes-merge from the mode"
	  " tables of:");
  for (size_t target = 0; target < target_names.size (); target++)
    printf (" %s", target_names[target].c_str ());
  printf (".  */\n\n#ifndef GCC_INSN_MODES_INLINE_H\n"
	  "#define GCC_INSN_MODES_INLINE_H\n"
	  "\n#if !defined (USED_FOR_TARGET) && GCC_VERSION >= 4001\n\n");

  /* Nothing here is const; see the union header's qualifiers.  */
  std::string table_extern ("poly_uint16 mode_size");
  emit_inline_head ("poly_uint16", "mode_size_inline", table_extern, true);
  for (size_t i = 0; i < union_modes.size (); i++)
    {
      bool can_inline = true, first = true;
      unsigned int value = 0;
      for (size_t target = 0; target < target_names.size (); target++)
	{
	  const target_mode &record = union_modes[i].per_target[target];
	  if (!record.present)
	    continue;
	  if (bytesize_adjusted_p (i, target))
	    can_inline = false;
	  if (first)
	    value = record.bytesize;
	  else if (value != record.bytesize)
	    can_inline = false;
	  first = false;
	}
      if (can_inline)
	printf ("    case E_%smode: return %u;\n",
		union_modes[i].name.c_str (), value);
    }
  emit_inline_tail ("mode_size");

  table_extern = "poly_uint16 mode_nunits";
  emit_inline_head ("poly_uint16", "mode_nunits_inline", table_extern, false);
  for (size_t i = 0; i < union_modes.size (); i++)
    {
      bool can_inline = true, first = true;
      unsigned int value = 0;
      for (size_t target = 0; target < target_names.size (); target++)
	{
	  const target_mode &record = union_modes[i].per_target[target];
	  if (!record.present)
	    continue;
	  if (record.adjusted_nunits)
	    can_inline = false;
	  if (first)
	    value = record.ncomponents;
	  else if (value != record.ncomponents)
	    can_inline = false;
	  first = false;
	}
      if (can_inline)
	printf ("    case E_%smode: return %u;\n",
		union_modes[i].name.c_str (), value);
    }
  emit_inline_tail ("mode_nunits");

  table_extern = "unsigned short mode_inner";
  emit_inline_head ("unsigned short", "mode_inner_inline", table_extern,
		    true);
  for (size_t i = 0; i < union_modes.size (); i++)
    {
      bool can_inline = true, first = true;
      std::string inner;
      for (size_t target = 0; target < target_names.size (); target++)
	{
	  const target_mode &record = union_modes[i].per_target[target];
	  if (!record.present)
	    continue;
	  std::string unit = union_modes[unit_index (i, target)].name;
	  if (first)
	    inner = unit;
	  else if (inner != unit)
	    can_inline = false;
	  first = false;
	}
      if (can_inline)
	printf ("    case E_%smode: return E_%smode;\n",
		union_modes[i].name.c_str (), inner.c_str ());
    }
  emit_inline_tail ("mode_inner");

  table_extern = "unsigned char mode_unit_size";
  emit_inline_head ("unsigned char", "mode_unit_size_inline", table_extern,
		    true);
  for (size_t i = 0; i < union_modes.size (); i++)
    {
      bool can_inline = true, first = true;
      unsigned int value = 0;
      for (size_t target = 0; target < target_names.size (); target++)
	{
	  const target_mode &record = union_modes[i].per_target[target];
	  if (!record.present)
	    continue;
	  size_t unit = unit_index (i, target);
	  if (bytesize_adjusted_p (unit, target))
	    can_inline = false;
	  unsigned int unit_size
	    = union_modes[unit].per_target[target].bytesize;
	  if (first)
	    value = unit_size;
	  else if (value != unit_size)
	    can_inline = false;
	  first = false;
	}
      if (can_inline)
	printf ("    case E_%smode: return %u;\n",
		union_modes[i].name.c_str (), value);
    }
  emit_inline_tail ("mode_unit_size");

  table_extern = "unsigned short mode_unit_precision";
  emit_inline_head ("unsigned short", "mode_unit_precision_inline",
		    table_extern, true);
  for (size_t i = 0; i < union_modes.size (); i++)
    {
      bool can_inline = true, first = true;
      std::string value;
      for (size_t target = 0; target < target_names.size (); target++)
	{
	  const target_mode &record = union_modes[i].per_target[target];
	  if (!record.present)
	    continue;
	  const target_mode &unit
	    = union_modes[unit_index (i, target)].per_target[target];
	  char text[32];
	  if (unit.precision != (unsigned int) -1)
	    snprintf (text, sizeof text, "%u", unit.precision);
	  else
	    snprintf (text, sizeof text, "%u*BITS_PER_UNIT", unit.bytesize);
	  if (first)
	    value = text;
	  else if (value != text)
	    can_inline = false;
	  first = false;
	}
      if (can_inline)
	printf ("    case E_%smode: return %s;\n",
		union_modes[i].name.c_str (), value.c_str ());
    }
  emit_inline_tail ("mode_unit_precision");

  puts ("#endif /* GCC_VERSION >= 4001 */");
  puts ("\n#endif /* insn-modes-inline.h */");
}

/* Format one polynomial row value at the union's coefficient count.  */

static std::string
poly_row (unsigned int value)
{
  char text[64];
  if (poly_int_coeffs_param == 2)
    snprintf (text, sizeof text, "{ %u, 0 }", value);
  else
    snprintf (text, sizeof text, "{ %u }", value);
  return text;
}

/* Emit one table of the per-target data file: NAME, with one row per
   union mode produced by ROW, which receives the union index and the
   target and returns the row text; an absent-mode row is filled with
   FILLER, or produced by ROW anyway when FILLER is null.  */

static void
emit_target_table (const char *name, size_t target, const char *filler,
		   std::string (*row) (size_t, size_t))
{
  printf ("  /* %s */\n  {\n", name);
  for (size_t i = 0; i < union_modes.size (); i++)
    {
      std::string text;
      if (filler == NULL || union_modes[i].per_target[target].present)
	text = row (i, target);
      else
	text = filler;
      printf ("    %s,\t\t/* %s */\n", text.c_str (),
	      union_modes[i].name.c_str ());
    }
  printf ("  },\n");
}

/* Row producers for emit_target_table.  */

/* TARGET if it has union mode I, else the first target that does.
   The value rows use it so a mode absent on a target still carries
   its intrinsic properties: MAX_MODE_INT-style widest-mode idioms
   index the tables directly, and a zeroed row breaks them — a zero
   precision makes wide-int comparison folds return nonsense.  The
   properties do not vary between defining targets.  The link rows
   keep the absent filler, so mode iteration never leaves the
   target.  */

static size_t
value_source_target (size_t i, size_t target)
{
  if (union_modes[i].per_target[target].present)
    return target;
  for (size_t other = 0; other < target_names.size (); other++)
    if (union_modes[i].per_target[other].present)
      return other;
  return target;
}

static std::string
precision_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  const target_mode &record = union_modes[i].per_target[target];
  if (record.precision != (unsigned int) -1)
    return poly_row (record.precision);
  char text[64];
  if (poly_int_coeffs_param == 2)
    snprintf (text, sizeof text, "{ %u * BITS_PER_UNIT, 0 }",
	      record.bytesize);
  else
    snprintf (text, sizeof text, "{ %u * BITS_PER_UNIT }", record.bytesize);
  return text;
}

static std::string
size_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  return poly_row (union_modes[i].per_target[target].bytesize);
}

static std::string
nunits_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  return poly_row (union_modes[i].per_target[target].ncomponents);
}

static std::string
mode_reference (const std::string &name)
{
  if (name == "-")
    return "E_VOIDmode";
  return "E_" + name + "mode";
}

static std::string
next_row (size_t i, size_t target)
{
  return mode_reference (union_modes[i].per_target[target].wider);
}

/* The skip-same-size wider walk of genmodes.cc's emit_mode_wider, on
   TARGET's chain.  */

static std::string
wider_row (size_t i, size_t target)
{
  const union_mode &mode = union_modes[i];
  int cl = mode.cl;
  if (cl != MODE_INT && cl != MODE_PARTIAL_INT && cl != MODE_FLOAT
      && cl != MODE_DECIMAL_FLOAT && cl != MODE_COMPLEX_FLOAT
      && cl != MODE_FRACT && cl != MODE_UFRACT && cl != MODE_ACCUM
      && cl != MODE_UACCUM)
    return "E_VOIDmode";
  const target_mode *record = &mode.per_target[target];
  std::string candidate = record->wider;
  while (candidate != "-" && candidate != "VOID")
    {
      const target_mode &next_record
	= union_modes[union_index_of (candidate)].per_target[target];
      if (next_record.bytesize == record->bytesize
	  && next_record.precision == record->precision)
	{
	  candidate = next_record.wider;
	  continue;
	}
      break;
    }
  if (candidate == "VOID")
    candidate = "-";
  return mode_reference (candidate);
}

/* The double-size walk of genmodes.cc's emit_mode_wider, on TARGET's
   chain, starting from the mode itself.  */

static std::string
wider_2x_row (size_t i, size_t target)
{
  const union_mode &mode = union_modes[i];
  const target_mode &record = mode.per_target[target];
  bool vector_class = (mode.cl == MODE_VECTOR_BOOL
		       || mode.cl == MODE_VECTOR_INT
		       || mode.cl == MODE_VECTOR_FLOAT
		       || mode.cl == MODE_VECTOR_FRACT
		       || mode.cl == MODE_VECTOR_UFRACT
		       || mode.cl == MODE_VECTOR_ACCUM
		       || mode.cl == MODE_VECTOR_UACCUM);
  std::string candidate = mode.name;
  while (candidate != "-" && candidate != "VOID")
    {
      const target_mode &walk
	= union_modes[union_index_of (candidate)].per_target[target];
      if (walk.bytesize < 2 * record.bytesize
	  || (record.precision != (unsigned int) -1
	      ? walk.precision != 2 * record.precision
	      : walk.precision != (unsigned int) -1)
	  || (vector_class
	      && (walk.ncomponents != 2 * record.ncomponents
		  || walk.component != record.component)))
	{
	  candidate = walk.wider;
	  continue;
	}
      break;
    }
  if (candidate == "VOID")
    candidate = "-";
  return mode_reference (candidate);
}

static std::string
inner_row (size_t i, size_t target)
{
  return mode_reference (union_modes[unit_index (i, target)].name);
}

static std::string
unit_size_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  size_t unit = unit_index (i, target);
  char text[32];
  snprintf (text, sizeof text, "%u",
	    union_modes[unit].per_target[target].bytesize);
  return text;
}

static std::string
unit_precision_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  const target_mode &unit
    = union_modes[unit_index (i, target)].per_target[target];
  char text[32];
  if (unit.precision != (unsigned int) -1)
    snprintf (text, sizeof text, "%u", unit.precision);
  else
    snprintf (text, sizeof text, "%u*BITS_PER_UNIT", unit.bytesize);
  return text;
}

static std::string
complex_row (size_t i, size_t target)
{
  return mode_reference (union_modes[i].per_target[target].complex_mode);
}

static std::string
mask_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  const target_mode &record = union_modes[i].per_target[target];
  char text[48];
  if (record.precision != (unsigned int) -1)
    snprintf (text, sizeof text, "MODE_MASK (%u)", record.precision);
  else
    snprintf (text, sizeof text, "MODE_MASK (%u*BITS_PER_UNIT)",
	      record.bytesize);
  return text;
}

static std::string
ibit_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  char text[16];
  snprintf (text, sizeof text, "%u", union_modes[i].per_target[target].ibit);
  return text;
}

static std::string
fbit_row (size_t i, size_t target)
{
  target = value_source_target (i, target);
  char text[16];
  snprintf (text, sizeof text, "%u", union_modes[i].per_target[target].fbit);
  return text;
}

static std::string
present_row (size_t i, size_t target)
{
  return union_modes[i].per_target[target].present ? "1" : "0";
}

static std::string
base_align_row (size_t i, size_t target)
{
  char text[16];
  snprintf (text, sizeof text, "%u",
	    union_modes[i].per_target[target].alignment);
  return text;
}

/* Emit the value tables of the target named TARGET_NAME at the union
   mode numbering, as a mode_tables instance.  */

static void
emit_target_tables (const std::string &target_name)
{
  size_t target = target_names.size ();
  for (size_t i = 0; i < target_names.size (); i++)
    if (target_names[i] == target_name)
      target = i;
  if (target == target_names.size ())
    fatal ("%s is not one of the dumped targets", target_name.c_str ());

  printf ("/* Generated automatically by genmodes-merge from the mode"
	  " tables of:");
  for (size_t i = 0; i < target_names.size (); i++)
    printf (" %s", target_names[i].c_str ());
  printf (".\n   The value tables of target %s at the union mode"
	  " numbering.  */\n\n", target_name.c_str ());
  printf ("#include \"config.h\"\n"
	  "#include \"system.h\"\n"
	  "#include \"coretypes.h\"\n"
	  "#include \"real.h\"\n"
	  "#include \"mode-tables.h\"\n\n");
  puts ("#define MODE_MASK(m) \\\n"
	"  ((m) >= HOST_BITS_PER_WIDE_INT) \\\n"
	"   ? HOST_WIDE_INT_M1U \\\n"
	"   : (HOST_WIDE_INT_1U << (m)) - 1\n");
  /* C++ gives a const object internal linkage unless it is declared
     extern first.  */
  printf ("extern const struct mode_tables mt_mode_tables_%s;\n\n",
	  target_name.c_str ());
  printf ("const struct mode_tables mt_mode_tables_%s =\n{\n",
	  target_name.c_str ());

  emit_target_table ("mode_precision", target, NULL, precision_row);
  emit_target_table ("mode_size", target, NULL, size_row);
  emit_target_table ("mode_nunits", target, NULL, nunits_row);
  emit_target_table ("mode_next", target, "E_VOIDmode", next_row);
  emit_target_table ("mode_wider", target, "E_VOIDmode", wider_row);
  emit_target_table ("mode_2xwider", target, "E_VOIDmode", wider_2x_row);
  emit_target_table ("mode_inner", target, "E_VOIDmode", inner_row);
  emit_target_table ("mode_unit_size", target, NULL, unit_size_row);
  emit_target_table ("mode_unit_precision", target, NULL,
		     unit_precision_row);
  emit_target_table ("mode_complex", target, "E_VOIDmode", complex_row);
  emit_target_table ("mode_mask_array", target, NULL, mask_row);
  emit_target_table ("mode_ibit", target, NULL, ibit_row);
  emit_target_table ("mode_fbit", target, NULL, fbit_row);
  emit_target_table ("mode_base_align", target, "0", base_align_row);
  emit_target_table ("mode_present", target, NULL, present_row);

  printf ("  /* class_narrowest_mode: this target's narrowest mode of\n"
	  "     each class, not the union's.  */\n  {\n");
  for (int c = 0; c < MAX_MODE_CLASS; c++)
    {
      std::string narrowest ("-");
      /* genmodes never counts a boolean mode as a class's narrowest,
	 keeping BImode out of the MODE_INT iteration range.  */
      for (size_t i = 0; i < union_modes.size (); i++)
	if (union_modes[i].cl == c
	    && union_modes[i].per_target[target].present
	    && !union_modes[i].per_target[target].boolean_flag)
	  {
	    narrowest = union_modes[i].name;
	    break;
	  }
      printf ("    %s,\t\t/* %s */\n", mode_reference (narrowest).c_str (),
	      mode_class_names[c]);
    }
  printf ("  },\n");

  printf ("  /* real_format_for_mode */\n  {\n");
  for (int c = 0; c < MAX_MODE_CLASS; c++)
    {
      if (c != MODE_FLOAT && c != MODE_DECIMAL_FLOAT)
	continue;
      for (size_t i = 0; i < union_modes.size (); i++)
	{
	  if (union_modes[i].cl != c)
	    continue;
	  const target_mode &record = union_modes[i].per_target[target];
	  if (record.present && record.format != "-"
	      && record.format != "0")
	    printf ("    &%s,\t\t/* %s */\n", record.format.c_str (),
		    union_modes[i].name.c_str ());
	  else
	    printf ("    0,\t\t/* %s */\n",
		    union_modes[i].name.c_str ());
	}
    }
  printf ("  }\n};\n");
  printf ("\n#undef MODE_MASK\n");
}

/* True if MODE is a boolean mode (for the first target that has
   it; divergence shows up in the report as a non-uniform boolean
   property).  */

static bool
boolean_mode_p (const union_mode &mode)
{
  for (size_t target = 0; target < target_names.size (); target++)
    if (mode.per_target[target].present)
      return mode.per_target[target].boolean_flag != 0;
  return false;
}

/* The machmode.h wrapper class for modes of class CL, as in
   genmodes.cc's get_mode_class.  */

static const char *
wrapper_class (int cl)
{
  switch (cl)
    {
    case MODE_INT:
    case MODE_PARTIAL_INT:
      return "scalar_int_mode";

    case MODE_FRACT:
    case MODE_UFRACT:
    case MODE_ACCUM:
    case MODE_UACCUM:
      return "scalar_mode";

    case MODE_FLOAT:
    case MODE_DECIMAL_FLOAT:
      return "scalar_float_mode";

    case MODE_COMPLEX_INT:
    case MODE_COMPLEX_FLOAT:
      return "complex_mode";

    default:
      return NULL;
    }
}

/* Emit the union insn-modes.h, mirroring genmodes.cc's
   emit_insn_modes_h.  The union enum is laid out class-major, with
   every class's modes in order of first appearance across the dumps,
   so the primary target's layout leads and later targets append.  */

static void
emit_union_header (void)
{
  printf ("/* Generated automatically by genmodes-merge from the mode"
	  " tables of:");
  for (size_t target = 0; target < target_names.size (); target++)
    printf (" %s", target_names[target].c_str ());
  printf (".  */\n\n#ifndef GCC_INSN_MODES_H\n#define GCC_INSN_MODES_H\n"
	  "\nenum machine_mode\n{\n");

  for (size_t i = 0; i < union_modes.size (); i++)
    {
      const union_mode &mode = union_modes[i];
      int count = printf ("  E_%smode,", mode.name.c_str ());
      printf ("%*s/*", count < 27 ? 27 - count : 1, "");
      for (size_t target = 0; target < target_names.size (); target++)
	if (mode.per_target[target].present)
	  printf (" %s", target_names[target].c_str ());
      printf (" */\n");
      printf ("#define HAVE_%smode\n", mode.name.c_str ());
      printf ("#ifdef USE_ENUM_MODES\n");
      printf ("#define %smode E_%smode\n", mode.name.c_str (),
	      mode.name.c_str ());
      printf ("#else\n");
      if (const char *wrapper = wrapper_class (mode.cl))
	printf ("#define %smode (%s ((%s::from_int) E_%smode))\n",
		mode.name.c_str (), wrapper, wrapper, mode.name.c_str ());
      else
	printf ("#define %smode ((void) 0, E_%smode)\n",
		mode.name.c_str (), mode.name.c_str ());
      printf ("#endif\n");
    }

  puts ("  MAX_MACHINE_MODE,\n");

  for (int c = 0; c < MAX_MODE_CLASS; c++)
    {
      /* Find the class's slice and check that its boolean modes lead
	 it; they get their own range, as in genmodes.cc (see the
	 MIN_MODE_INT comment there).  */
      size_t first = 0, last = 0;
      bool have_modes = false, saw_non_boolean = false;
      for (size_t i = 0; i < union_modes.size (); i++)
	{
	  if (union_modes[i].cl != c)
	    continue;
	  if (!have_modes)
	    first = i;
	  have_modes = true;
	  last = i;
	  if (boolean_mode_p (union_modes[i]))
	    {
	      if (saw_non_boolean)
		fatal ("boolean mode %s does not lead its class",
		       union_modes[i].name.c_str ());
	    }
	  else
	    saw_non_boolean = true;
	}

      if (have_modes && boolean_mode_p (union_modes[first]))
	{
	  size_t last_boolean = first;
	  printf ("  MIN_MODE_BOOL = E_%smode,\n",
		  union_modes[first].name.c_str ());
	  while (first <= last && boolean_mode_p (union_modes[first]))
	    {
	      last_boolean = first;
	      first++;
	    }
	  printf ("  MAX_MODE_BOOL = E_%smode,\n\n",
		  union_modes[last_boolean].name.c_str ());
	  if (first > last)
	    have_modes = false;
	}

      if (have_modes)
	printf ("  MIN_%s = E_%smode,\n  MAX_%s = E_%smode,\n\n",
		mode_class_names[c], union_modes[first].name.c_str (),
		mode_class_names[c], union_modes[last].name.c_str ());
      else
	printf ("  MIN_%s = E_VOIDmode,\n  MAX_%s = E_VOIDmode,\n\n",
		mode_class_names[c], mode_class_names[c]);
    }

  puts ("  NUM_MACHINE_MODES = MAX_MACHINE_MODE\n};\n");

  for (int c = 0; c < MAX_MODE_CLASS; c++)
    {
      bool have_modes = false;
      for (size_t i = 0; i < union_modes.size (); i++)
	if (union_modes[i].cl == c)
	  have_modes = true;
      printf ("#define NUM_%s ", mode_class_names[c]);
      if (have_modes)
	printf ("(MAX_%s - MIN_%s + 1)\n", mode_class_names[c],
		mode_class_names[c]);
      else
	printf ("0\n");
    }
  printf ("\n");

  /* Activation installs the active target's values into every
     table, so none of them is const in a multi-target build.  */
  printf ("#define CONST_MODE_NUNITS\n");
  printf ("#define CONST_MODE_PRECISION\n");
  printf ("#define CONST_MODE_SIZE\n");
  printf ("#define CONST_MODE_UNIT_SIZE\n");
  printf ("#define CONST_MODE_BASE_ALIGN\n");
  printf ("#define CONST_MODE_IBIT\n");
  printf ("#define CONST_MODE_FBIT\n");
  printf ("#define CONST_MODE_MASK\n");
  printf ("#define CONST_MODE_WIDER\n");
  printf ("#define CONST_MODE_INNER\n");
  printf ("#define CONST_MODE_UNIT_PRECISION\n");
  printf ("#define CONST_MODE_COMPLEX\n");
  printf ("#define CONST_MODE_NARROWEST\n");

  printf ("\n#define BITS_PER_UNIT (%u)\n", bits_per_unit_param);
  printf ("#define MAX_BITSIZE_MODE_ANY_INT %u\n",
	  max_bitsize_any_int_param);
  printf ("#define MAX_BITSIZE_MODE_ANY_MODE %u\n",
	  max_bitsize_any_mode_param);

  unsigned int int_n_entries = 0;
  for (size_t i = 0; i < union_modes.size (); i++)
    for (size_t target = 0; target < target_names.size (); target++)
      if (union_modes[i].per_target[target].present
	  && union_modes[i].per_target[target].int_n)
	{
	  int_n_entries++;
	  break;
	}
  printf ("#define NUM_INT_N_ENTS %u\n", int_n_entries);
  printf ("#define NUM_POLY_INT_COEFFS %u\n", poly_int_coeffs_param);

  puts ("\n#endif /* insn-modes.h */");
}

/* Order the union class-major, then by first appearance across the
   dumps, so the primary target's enum layout leads within every
   class and later targets append.  */

static bool
union_order (const union_mode &first, const union_mode &second)
{
  if (first.cl != second.cl)
    return first.cl < second.cl;
  return first.first_seen < second.first_seen;
}

int
main (int argc, char **argv)
{
  progname = argv[0];
  bool emit_header = false, emit_inline = false;
  std::string tables_target;
  int first_argument = 1;
  if (argc > 1 && !strcmp (argv[1], "-h"))
    {
      emit_header = true;
      first_argument = 2;
    }
  else if (argc > 1 && !strcmp (argv[1], "-i"))
    {
      emit_inline = true;
      first_argument = 2;
    }
  else if (argc > 2 && !strcmp (argv[1], "-d"))
    {
      tables_target = argv[2];
      first_argument = 3;
    }
  if (argc - first_argument < 1)
    fatal ("usage: %s [-h|-i|-d <target>] <name>=<dump> ...",
	   progname);

  std::vector<std::string> dump_files;
  for (int i = first_argument; i < argc; i++)
    {
      std::string argument (argv[i]);
      size_t equals = argument.find ('=');
      if (equals == std::string::npos)
	fatal ("argument %s is not of the form <name>=<dump>", argv[i]);
      target_names.push_back (argument.substr (0, equals));
      dump_files.push_back (argument.substr (equals + 1));
    }

  for (size_t target = 0; target < target_names.size (); target++)
    read_dump (dump_files[target].c_str (), target);

  std::sort (union_modes.begin (), union_modes.end (), union_order);
  /* The sort shuffled the indexes the name map holds.  */
  for (size_t i = 0; i < union_modes.size (); i++)
    union_index_by_name[union_modes[i].name] = i;

  if (emit_header || emit_inline || !tables_target.empty ())
    {
      if (emit_header)
	emit_union_header ();
      else if (emit_inline)
	emit_union_inline_header ();
      else
	emit_target_tables (tables_target);
      if (fflush (stdout) || fclose (stdout))
	return FATAL_EXIT_CODE;
      return SUCCESS_EXIT_CODE;
    }

  printf ("%lu targets, %lu modes in the union\n",
	  (unsigned long) target_names.size (),
	  (unsigned long) union_modes.size ());
  for (size_t target = 0; target < target_names.size (); target++)
    {
      size_t count = 0;
      for (size_t i = 0; i < union_modes.size (); i++)
	if (union_modes[i].per_target[target].present)
	  count++;
      printf ("target %s: %lu modes\n", target_names[target].c_str (),
	      (unsigned long) count);
    }

  for (int c = 0; c < MAX_MODE_CLASS; c++)
    {
      size_t count = 0;
      for (size_t i = 0; i < union_modes.size (); i++)
	if (union_modes[i].cl == c)
	  count++;
      if (count)
	printf ("class %s: %lu modes\n", mode_class_names[c],
		(unsigned long) count);
    }

  size_t single_target = 0, linked_differently = 0;
  printf ("modes with non-uniform properties:\n");
  for (size_t i = 0; i < union_modes.size (); i++)
    {
      const union_mode &mode = union_modes[i];
      size_t contributors = 0;
      for (size_t target = 0; target < target_names.size (); target++)
	if (mode.per_target[target].present)
	  contributors++;
      if (contributors == 1)
	single_target++;
      if (links_vary_p (mode))
	linked_differently++;
      std::string variants = variant_properties (mode);
      if (!variants.empty ())
	printf ("  %s: %s\n", mode.name.c_str (), variants.c_str ());
    }
  printf ("%lu modes appear in a single target only\n",
	  (unsigned long) single_target);
  printf ("%lu modes have per-target link chains\n",
	  (unsigned long) linked_differently);

  if (fflush (stdout) || fclose (stdout))
    return FATAL_EXIT_CODE;
  return SUCCESS_EXIT_CODE;
}
