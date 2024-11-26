#pragma once

#ifndef FS_set_backend
  #if defined(__platform_unix_linux)
    #define FS_set_backend 0
  #elif defined(__platform_windows)
    #define FS_set_backend 1
  #elif defined(__platform_unix_freebsd)
    #define FS_set_backend 2
  #else
    #error FS_set_backend is not defined
  #endif
#endif

#if FS_set_backend == 0
  #include _WITCH_PATH(FS/backend/0/0.h)
#elif FS_set_backend == 1
  #include _WITCH_PATH(FS/backend/1/1.h)
#elif FS_set_backend == 2
  #include _WITCH_PATH(FS/backend/2/2.h)
#else
  #error ?
#endif
