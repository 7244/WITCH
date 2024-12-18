#pragma once

#ifndef __platform
  #define __platform

  #if defined(__linux__)
    #define __platform_unix
    #define __platform_unix_linux
  #elif defined(__FreeBSD__)
    #define __platform_unix
    #define __platform_unix_freebsd
  #elif defined(_WIN32) || defined(_WIN64)
    #define __platform_windows
  #elif defined(__bpf__) || defined(__BPF__) || defined(__BPF_CPU_VERSION__)
    #define __platform_bpf
  #else
    #undef __platform
  #endif
#endif

#if defined(__platform)
  /* TODO how to detect __platform_linux_kernel_module ? */
  #if defined(__platform_linux_kernel_module)
    #define WITCH_unsigned_types_will_be_defined
  #elif defined(__platform_bpf)
    #define __platform_nothread
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

#if defined(__platform_unix)
  #if defined(__platform_windows)
    #error broken compiler. probably wine compiler.
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

#if defined(__platform_windows)
  #define WIN32_LEAN_AND_MEAN
  #define WIN64_LEAN_AND_MEAN
  #define _CRT_SECURE_NO_WARNINGS
  #ifdef _MSC_VER
    #include <intrin.h>
  #endif
#endif

#if defined(__platform_unix) || defined(__platform_windows)
  #define FD_IN 0
  #define FD_OUT 1
  #define FD_ERR 2
#endif
