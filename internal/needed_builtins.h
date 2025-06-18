#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14) || \
  (defined(__compiler_clang) && __clang_major__ >= 18 && __clang_major__ <= 19)

  __no_name_mangling
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
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14) || \
  (defined(__compiler_clang) && __clang_major__ >= 18 && __clang_major__ <= 19)

  __no_name_mangling
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

#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 15 && __GNUC__ <= 15) || \
  (defined(__compiler_clang) && __clang_major__ >= 19 && __clang_major__ <= 19)

  __no_name_mangling
  void memmove(
    void *dst,
    const void *src,
    uintptr_t size
  ){
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;

    if(d < s){
      while(size--){
        *d++ = *s++;
      }
    }
    else{
      d += size;
      s += size;

      while(size--){
        *--d = *--s;
      }
    }
  }

#endif

#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 10 && __GNUC__ <= 15) || \
  (defined(__compiler_clang) && __clang_major__ >= 19 && __clang_major__ <= 19)

  __no_name_mangling_begin
    uintptr_t strlen(
      const void *cstr
    ){
      uintptr_t i = 0;
      while(EXPECT(((const uint8_t *)cstr)[i], 1)){
        i++;
      }
      return i;
    }
  __no_name_mangling_end

#endif
