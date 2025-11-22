#ifndef _WITCH_libdefine_PR
#define _WITCH_libdefine_PR

#include "implement.h"

#ifdef _WITCH_libdefine_PlatformOpen
  #error ?
#endif

#ifdef PRE
  PRE{
    _PR_internal_open();
  }
#endif
/* TODO _PR_internal_close needs to be called if its automaticly possible */

#ifdef __abort
  #undef __abort
#endif

#define __abort() do{ \
  PR_abort(); \
  __unreachable_or(); \
}while(0)

#endif
