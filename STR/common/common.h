#pragma once

#ifndef STR_common_set_backend
  #if WITCH_LIBC
    #define STR_common_set_backend 0
  #else
    #define STR_common_set_backend 1
  #endif
#endif

#if STR_common_set_backend == 0
  #include _WITCH_PATH(STR/common/backend/0/0.h)
#elif STR_common_set_backend == 1
  #include _WITCH_PATH(STR/common/backend/1/1.h)
#else
  #error ?
#endif
