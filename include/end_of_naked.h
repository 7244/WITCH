#if defined(__compiler_clang) && __clang_major__ >= 19 && __clang_major__ <= 20
  /* ~poolaki~ */
#elif defined(__compiler_gcc)
  /* gcc generates undefined instruction without unreachable */
  __unreachable();
#elif defined(__compiler_tinyc)
  __unreachable();
#else
  #error ?
#endif
