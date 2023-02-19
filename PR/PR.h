#pragma once

#if defined(WOS_UNIX)
  #include "backend/unix/unix.h"
#elif defined(WITCH_PLATFORM_linux_kernel_module)
  #include "backend/linux_kernel_module/linux_kernel_module.h"
#elif defined(WOS_WINDOWS)
  #include "backend/windows/windows.h"
#else
  #error ?
#endif
