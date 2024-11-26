#pragma once

#ifndef TH_set_backend
  #if defined(__platform_unix)
    #define TH_set_backend 0
  #elif defined(__platform_windows)
    #define TH_set_backend 1
  #else
    #error TH_set_backend is not defined
  #endif
#endif

#if TH_set_backend == 0
  #include _WITCH_PATH(TH/backend/0/0.h)
#elif TH_set_backend == 1
  #include _WITCH_PATH(TH/backend/1/1.h)
#else
  #error ?
#endif
