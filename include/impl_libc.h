#pragma once

#define __platform_nostdlib
#define WITCH_PRE_is_not_allowed

#include "../WITCH.h"

#include _WITCH_PATH(PR/PR.h)

/* not needed but need for platform open */
#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(RAND/RAND.h)
#include _WITCH_PATH(T/T.h)

static int main(int, const char **);

static void _WITCH_impl_libc_main(uintptr_t argc, const uint8_t **argv){

  #include <WITCH/PlatformOpen.h>

  int ret = main(argc, (const char **)argv);

  PR_exit(ret);
  __unreachable_or();
}

#define _WITCH__start_func_name _WITCH_impl_libc_main
#include <WITCH/include/_start.h>
