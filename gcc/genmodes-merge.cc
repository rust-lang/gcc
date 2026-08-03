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

   For now the tool reports the union; the emission of the union
   headers and the per-target value tables comes with the multi-target
   build integration.

   Usage: genmodes-merge <name>=<dump> <name>=<dump> ... > report.  */

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
  std::vector<target_mode> per_target;
};

static std::vector<std::string> target_names;
static std::vector<union_mode> union_modes;
static std::map<std::string, size_t> union_index_by_name;

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

/* Order the union class-major, then by name.  The final enum layout is
   decided by the emission step; this order only serves the report.  */

static bool
union_order (const union_mode &first, const union_mode &second)
{
  if (first.cl != second.cl)
    return first.cl < second.cl;
  return first.name < second.name;
}

int
main (int argc, char **argv)
{
  progname = argv[0];
  if (argc < 2)
    fatal ("usage: %s <name>=<dump> <name>=<dump> ... > report", progname);

  std::vector<std::string> dump_files;
  for (int i = 1; i < argc; i++)
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
