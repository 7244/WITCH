#pragma once

#ifndef __platform
  #if defined(__linux__)
    #define WOS_UNIX 1
    #define WOS_UNIX_LINUX 1
  #elif defined(__FreeBSD__)
    #define WOS_UNIX 1
    #define WOS_UNIX_BSD 1
  #elif defined(_WIN32) || defined(_WIN64)
    #define WOS_WINDOWS 1
  #endif

  #define __platform
#endif

#if defined(__platform)
  /* TODO how to detect WITCH_PLATFORM_linux_kernel_module ? */
  #if defined(WITCH_PLATFORM_linux_kernel_module)
    #define WITCH_unsigned_types_will_be_defined
  #else
    #ifndef __platform_stdlib
      #define __platform_stdlib
    #endif
  #endif
#else
  #ifndef __platform_stdlib
    #define __platform_stdlib
  #endif
#endif

#ifdef __platform_nostdlib
  #ifdef __platform_stdlib
    #undef __platform_stdlib
  #endif
#endif

#ifdef __platform_stdlib
  #ifndef __platform_nolibc
    #ifndef __platform_libc
      #define __platform_libc
    #endif
  #endif

  #ifndef __platform_nolibcpp
    #ifdef __cplusplus
      #ifndef __platform_libcpp
        #define __platform_libcpp
      #endif
    #endif
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
