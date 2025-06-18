#pragma once

#define __WITCH_defined_libc_function_strlen

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
