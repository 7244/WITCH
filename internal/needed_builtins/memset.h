#pragma once

#define __WITCH_defined_libc_function_memset

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
