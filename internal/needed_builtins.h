#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14) || \
  (defined(__compiler_clang) && __clang_major__ >= 18 && __clang_major__ <= 19)

  #include "needed_builtins/memset.h"
#endif

#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14) || \
  (defined(__compiler_clang) && __clang_major__ >= 18 && __clang_major__ <= 19)

  #include "needed_builtins/memcpy.h"
#endif

#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 15 && __GNUC__ <= 15) || \
  (defined(__compiler_clang) && __clang_major__ >= 19 && __clang_major__ <= 19)

  #include "needed_builtins/memmove.h"
#endif

#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 10 && __GNUC__ <= 15) || \
  (defined(__compiler_clang) && __clang_major__ >= 19 && __clang_major__ <= 19)

  #include "needed_builtins/strlen.h"
#endif
