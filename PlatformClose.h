#ifdef _WITCH_libdefine_PlatformClose
  #error PlatformClose included multiple times.
#else
  #define _WITCH_libdefine_PlatformClose

  #ifndef _WITCH_libdefine_PlatformOpen
    #error PlatformClose included before PlatformOpen
  #endif
  #undef _WITCH_libdefine_PlatformOpen

  #ifdef _WITCH_libdefine_PR
    _PR_internal_close();
  #endif
  #ifdef _WITCH_libdefine_IO
    _IO_internal_close();
  #endif
  #ifdef _WITCH_libdefine_RAND
    _RAND_internal_close();
  #endif
#endif
