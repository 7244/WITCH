#pragma once

#define __WITCH_defined_libc_function_memcpy

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
