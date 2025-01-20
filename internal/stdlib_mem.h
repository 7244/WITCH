/* this file is included when compiler may gonna use mem* functions of stdlib */
/* even when there is no stdlib, even when these functions doesnt exists */

#if defined(__platform_windows)
  #error how to say uintptr_t in windows's compilers?
#endif

void memset(
  void * dst,
  unsigned char byte,
  const unsigned long size
){
  for(unsigned long i = 0; i < size; i++){
    ((unsigned char *)dst)[i] = byte;
  }
}

void memcpy(
  void * __restrict_or_nothing dst,
  const void * __restrict_or_nothing src,
  const unsigned long size
){
  for(unsigned long i = 0; i < size; i++){
    ((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
  }
}
