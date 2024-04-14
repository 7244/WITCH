#pragma once

#ifndef MEM_set_backend
  #if WITCH_LIBC
    #define MEM_set_backend 0
  #else
    #define MEM_set_backend 1
  #endif
#endif

#if MEM_set_backend == 0
  #include _WITCH_PATH(MEM/backend/0/0.h)
#elif MEM_set_backend == 1
  #include _WITCH_PATH(MEM/backend/1/1.h)
#else
  #error ?
#endif
