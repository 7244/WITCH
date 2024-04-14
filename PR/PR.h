#ifndef _WITCH_libdefine_PR
#define _WITCH_libdefine_PR

#if defined(WOS_UNIX)
  #include "backend/unix/unix.h"
#elif defined(WITCH_PLATFORM_linux_kernel_module)
  #include "backend/linux_kernel_module/linux_kernel_module.h"
#elif defined(WOS_WINDOWS)
  #include "backend/windows/windows.h"
#else
  #error ?
#endif

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
#define __abort PR_abort

#endif
