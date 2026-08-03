/* Compile for several targets from concurrent threads: one thread
   per configured target, the default target included, each
   compiling a fresh context per iteration.  libgccjit serializes
   the compilations internally, but every interleaving of target
   activations must leave each thread's outputs byte-identical
   across its iterations.  The worker threads never touch the
   dejagnu harness, which is not thread-safe: they only record
   their results, and the main thread reports after joining.  */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libgccjit.h"

#define TEST_PROVIDES_MAIN
#define TEST_ESCHEWS_TEST_JIT
#define TEST_ESCHEWS_SET_OPTIONS
#include "harness.h"

#define ITERATIONS 5

struct thread_task
{
  const char *triple;		/* NULL compiles the default target.  */
  int index;
  int ok;
  char message[512];
  char paths[ITERATIONS][64];
};

static void
build_square (gcc_jit_context *ctxt)
{
  gcc_jit_type *int_type
    = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_param *param
    = gcc_jit_context_new_param (ctxt, NULL, int_type, "i");
  gcc_jit_function *fn
    = gcc_jit_context_new_function (ctxt, NULL,
				    GCC_JIT_FUNCTION_EXPORTED,
				    int_type, "square", 1, &param, 0);
  gcc_jit_block *block = gcc_jit_function_new_block (fn, NULL);
  gcc_jit_rvalue *expr
    = gcc_jit_context_new_binary_op (ctxt, NULL,
				     GCC_JIT_BINARY_OP_MULT, int_type,
				     gcc_jit_param_as_rvalue (param),
				     gcc_jit_param_as_rvalue (param));
  gcc_jit_block_end_with_return (block, NULL, expr);
}

static void *
compile_worker (void *data)
{
  struct thread_task *task = (struct thread_task *) data;
  int i;

  for (i = 0; i < ITERATIONS; i++)
    {
      gcc_jit_context *ctxt = gcc_jit_context_acquire ();
      const char *first_error;

      if (task->triple != NULL)
	gcc_jit_context_set_target (ctxt, task->triple);
      build_square (ctxt);
      snprintf (task->paths[i], sizeof (task->paths[i]),
		"tmp-threads-%d-%d.s", task->index, i);
      gcc_jit_context_compile_to_file (ctxt,
				       GCC_JIT_OUTPUT_KIND_ASSEMBLER,
				       task->paths[i]);
      first_error = gcc_jit_context_get_first_error (ctxt);
      if (first_error != NULL)
	{
	  snprintf (task->message, sizeof (task->message),
		    "iteration %d: %s", i, first_error);
	  task->ok = 0;
	  gcc_jit_context_release (ctxt);
	  return NULL;
	}
      gcc_jit_context_release (ctxt);
    }
  task->ok = 1;
  return NULL;
}

static char *
read_whole_file (const char *path, long *size_out)
{
  FILE *f = fopen (path, "rb");
  char *buffer;
  long size;
  if (f == NULL)
    return NULL;
  fseek (f, 0, SEEK_END);
  size = ftell (f);
  fseek (f, 0, SEEK_SET);
  buffer = (char *) malloc (size);
  if (buffer == NULL || fread (buffer, 1, size, f) != (size_t) size)
    {
      fclose (f);
      free (buffer);
      return NULL;
    }
  fclose (f);
  *size_out = size;
  return buffer;
}

static const char *
task_label (const struct thread_task *task)
{
  return task->triple != NULL ? task->triple : "(default)";
}

/* Check one joined thread's results: every iteration's output must
   be byte-identical to the first.  Returns the first iteration's
   output, or NULL after reporting a failure.  */

static char *
check_task (const struct thread_task *task, long *size_out)
{
  char *reference;
  long reference_size;
  int i;

  if (!task->ok)
    {
      fail ("thread for %s: %s", task_label (task), task->message);
      return NULL;
    }
  reference = read_whole_file (task->paths[0], &reference_size);
  if (reference == NULL)
    {
      fail ("thread for %s: reading %s back", task_label (task),
	    task->paths[0]);
      return NULL;
    }
  for (i = 1; i < ITERATIONS; i++)
    {
      long size;
      char *output = read_whole_file (task->paths[i], &size);
      if (output == NULL || size != reference_size
	  || memcmp (output, reference, size) != 0)
	{
	  fail ("thread for %s: iteration %d differs",
		task_label (task), i);
	  free (output);
	  free (reference);
	  return NULL;
	}
      free (output);
    }
  pass ("thread for %s: outputs stable across iterations",
	task_label (task));
  *size_out = reference_size;
  return reference;
}

int
main (int argc, char **argv)
{
  const char *secondaries = getenv ("JIT_MULTI_TARGET_SECONDARIES");
  char *list, *first, *second;
  struct thread_task tasks[3];
  pthread_t threads[3];
  char *outputs[3];
  long sizes[3];
  int count = 0, i, all_ok = 1;

  if (secondaries == NULL || secondaries[0] == '\0')
    {
      untested ("not a multi-target compiler");
      totals ();
      return 0;
    }
  list = strdup (secondaries);
  first = strtok (list, " ");
  second = strtok (NULL, " ");

  memset (tasks, 0, sizeof (tasks));
  tasks[count].triple = first;
  tasks[count].index = count;
  count++;
  if (second != NULL)
    {
      tasks[count].triple = second;
      tasks[count].index = count;
      count++;
    }
  /* One thread compiles the default target throughout.  */
  tasks[count].triple = NULL;
  tasks[count].index = count;
  count++;

  for (i = 0; i < count; i++)
    if (pthread_create (&threads[i], NULL, compile_worker,
			&tasks[i]) != 0)
      {
	fail ("creating thread %d", i);
	totals ();
	return 0;
      }
  for (i = 0; i < count; i++)
    pthread_join (threads[i], NULL);

  for (i = 0; i < count; i++)
    {
      outputs[i] = check_task (&tasks[i], &sizes[i]);
      if (outputs[i] == NULL)
	all_ok = 0;
    }

  /* The first secondary and the default target are different
     architectures: identical outputs would mean a selection was
     lost in the interleaving.  */
  if (all_ok)
    {
      if (sizes[0] == sizes[count - 1]
	  && memcmp (outputs[0], outputs[count - 1],
		     sizes[0]) == 0)
	fail ("secondary and default outputs are identical");
      else
	pass ("secondary and default outputs diverge");
    }

  totals ();
  return 0;
}
