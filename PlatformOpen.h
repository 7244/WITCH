#ifdef _WITCH_libdefine_PlatformOpen
  #error PlatformOpen included multiple times.
#else
  #define _WITCH_libdefine_PlatformOpen

  #ifdef PRE
    #error PlatformOpen can't be called when PRE is defined.
  #endif

  #ifdef _WITCH_libdefine_PlatformClose
    #undef _WITCH_libdefine_PlatformClose
  #endif

  #ifdef _WITCH_libdefine_PR
    _PR_internal_open();
  #endif
  #ifdef _WITCH_libdefine_IO
    _IO_internal_open();
  #endif
  #ifdef _WITCH_libdefine_RAND
    _RAND_internal_open();
  #endif
#endif
