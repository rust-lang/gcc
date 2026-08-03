/* Exercise the documented multi-target plugin contract: targetm is
   reachable under its usual name, its hooks answer, and it refers
   to the same target for the whole compilation.  */

#include "config.h"
#include "gcc-plugin.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "target.h"
#include "diagnostic-core.h"
#include "plugin-version.h"

int plugin_is_GPL_compatible;

/* What the compilation started with.  A multi-target compiler
   reaches targetm through the active target's pointer, and one
   invocation serves exactly one target, so neither the address nor
   the state behind it may change.  */
static const void *initial_targetm_address;
static const char *initial_open_paren;

static void
record_target (void *, void *)
{
  initial_targetm_address = (const void *) &targetm;
  initial_open_paren = targetm.asm_out.open_paren;
}

static void
check_target (void *, void *)
{
  if (initial_targetm_address == NULL)
    error ("multi-target plugin: start-unit callback never ran");
  else if (initial_targetm_address != (const void *) &targetm)
    error ("multi-target plugin: targetm moved during compilation");
  else if (initial_open_paren == NULL
	   || initial_open_paren != targetm.asm_out.open_paren)
    error ("multi-target plugin: target state changed");
}

int
plugin_init (struct plugin_name_args *plugin_info,
	     struct plugin_gcc_version *version)
{
  if (!plugin_default_version_check (version, &gcc_version))
    return 1;
  register_callback (plugin_info->base_name, PLUGIN_START_UNIT,
		     record_target, NULL);
  register_callback (plugin_info->base_name, PLUGIN_FINISH,
		     check_target, NULL);
  return 0;
}
