#pragma once

#define __WITCH_defined_libc_function_memmove

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
