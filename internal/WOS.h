#pragma once

#if defined(WITCH_PLATFORM_linux_kernel_module)
  #if defined(WITCH_LIBC)
    #if WITCH_LIBC != 0
      #error ?
    #endif
  #endif
  #define WITCH_LIBC 0
  #define WITCH_unsigned_types_will_be_defined
#elif defined(__linux__)
  #define WOS_UNIX 1
  #define WOS_UNIX_LINUX 1
  #ifndef WITCH_LIBC
    #define WITCH_LIBC 1
  #endif
#elif defined(__FreeBSD__)
  #define WOS_UNIX 1
  #define WOS_UNIX_BSD 1
  #ifndef WITCH_LIBC
    #define WITCH_LIBC 1
  #endif
#elif defined(_WIN32) || defined(_WIN64)
  #define WOS_WINDOWS 1
  #ifndef WITCH_LIBC
    #define WITCH_LIBC 1
  #endif
#endif

#if defined(WOS_UNIX)
  #if defined(WOS_WINDOWS)
    #if defined(__WINE__)
      #undef WOS_UNIX
    #else
      #error defined(WOS_UNIX) && defined(WOS_WINDOWS)
    #endif
  #endif

  #if defined(_FILE_OFFSET_BITS)
    #if _FILE_OFFSET_BITS != 64
      #error ?
    #endif
  #else
    #define _FILE_OFFSET_BITS 64
  #endif
  #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
  #endif
#endif

#if defined(WOS_WINDOWS)
  #define WIN32_LEAN_AND_MEAN
  #define WIN64_LEAN_AND_MEAN
  #define _CRT_SECURE_NO_WARNINGS
  #ifdef _MSC_VER
    #include <intrin.h>
  #endif
#endif

#if defined(WOS_UNIX) || defined(WOS_WINDOWS)
  #define FD_IN 0
  #define FD_OUT 1
  #define FD_ERR 2
#endif
