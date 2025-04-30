#pragma once

#ifdef TH_set_backend
  #error TH_set_backend is deprecated. now its automatic, dont define it.
#endif

#if defined(__platform_unix)
  #if defined(__platform_libc)
    #include "backend/0/0.h"
  #else
    #include "backend/unix/impl.h"
  #endif
#elif defined(__platform_windows)
  #include "backend/1/1.h"
#else
  #error failed to find any backend for this platform
#endif
