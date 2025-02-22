#if \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14) || \
  (defined(__compiler_clang) && __clang_major__ >= 18 && __clang_major__ <= 19)

  void memset(
    void *dst,
    uint8_t byte,
    uintptr_t size
  ){
    for(uintptr_t i = 0; i < size; i++){
      ((uint8_t *)dst)[i] = byte;
    }
  }

#endif

#if \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14) || \
  (defined(__compiler_clang) && __clang_major__ >= 18 && __clang_major__ <= 19)

  void memcpy(
    void * __restrict_or_nothing dst,
    const void * __restrict_or_nothing src,
    uintptr_t size
  ){
    for(uintptr_t i = 0; i < size; i++){
      ((uint8_t *)dst)[i] = ((uint8_t *)src)[i];
    }
  }

#endif