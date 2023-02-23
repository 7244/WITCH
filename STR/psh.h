#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(STR/common/common.h)

uint8_t _STR_psh_c(uint8_t c){
  switch(c){
    case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
      return c - '0';
    case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
      return c - 'a' + 10;
    case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
      return c - 'A' + 10;
  }
}
#define _STR_psh_c(c_m) \
  _STR_psh_c((uint8_t)(c_m))

uint32_t STR_psh32(const uint8_t *str, uintptr_t size){
  if(size < 3)
    return 0;
  for(uintptr_t hi = 0; hi < 3; hi++){
    if(!STR_ischars_hex(str[hi], hi))
      return 0;
  }
  str += 2;
  uint32_t res = 0;
  for(uintptr_t i = 0; i < size; i++){
    res <<= 4;
    res |= _STR_psh_c(str[i]);
  }
  return res;
}
#define STR_psh32(str_m, size_m) \
  STR_psh32((const uint8_t *)(str_m), (uintptr_t)(size_m))
uint64_t STR_psh64(const uint8_t *str, uintptr_t size){
  if(size < 3)
    return 0;
  for(uintptr_t hi = 0; hi < 3; hi++){
    if(!STR_ischars_hex(str[hi], hi))
      return 0;
  }
  str += 2;
  uint64_t res = 0;
  for(uintptr_t i = 0; i < size; i++){
    res <<= 4;
    res |= _STR_psh_c(str[i]);
  }
  return res;
}
#define STR_psh64(str_m, size_m) \
  STR_psh64((const uint8_t *)(str_m), (uintptr_t)(size_m))
#define STR_psh CONCAT(STR_psh, SYSTEM_BIT)

uint32_t STR_psh32_digit(const uint8_t *str, uintptr_t size){
  uint32_t res = 0;
  for(uintptr_t i = 0; i < size; i++){
    res <<= 4;
    res |= _STR_psh_c(str[i]);
  }
  return res;
}
#define STR_psh32_digit(str_m, size_m) \
  STR_psh32_digit((const uint8_t *)(str_m), (uintptr_t)(size_m))
uint64_t STR_psh64_digit(const uint8_t *str, uintptr_t size){
  uint64_t res = 0;
  for(uintptr_t i = 0; i < size; i++){
    res <<= 4;
    res |= _STR_psh_c(str[i]);
  }
  return res;
}
#define STR_psh64_digit(str_m, size_m) \
  STR_psh64_digit((const uint8_t *)(str_m), (uintptr_t)(size_m))
#define STR_psh_digit CONCAT(STR_psh, _digit)

uint32_t STR_psh32_iguess(const uint8_t *str, uintptr_t *i){
  for(uintptr_t hi = 0; hi < 3; hi++){
    if(!STR_ischars_hex(str[(*i) + hi], hi))
      return 0;
  }
  (*i) += 2;
  uint32_t res = 0;
  for(; STR_ischar_hexdigit(str[*i]); (*i)++){
    res <<= 4;
    res |= _STR_psh_c(str[*i]);
  }
  return res;
}
#define STR_psh32_iguess(str_m, i_m) \
  STR_psh32_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
uint64_t STR_psh64_iguess(const uint8_t *str, uintptr_t *i){
  for(uintptr_t hi = 0; hi < 3; hi++){
    if(!STR_ischars_hex(str[(*i) + hi], hi))
      return 0;
  }
  (*i) += 2;
  uint64_t res = 0;
  for(; STR_ischar_hexdigit(str[*i]); (*i)++){
    res <<= 4;
    res |= _STR_psh_c(str[*i]);
  }
  return res;
}
#define STR_psh64_iguess(str_m, i_m) \
  STR_psh64_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
#define STR_psh_iguess CONCAT3(STR_psh, SYSTEM_BIT, _iguess)

uint32_t STR_psh32_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
  if(size < 3)
    return 0;
  for(uintptr_t hi = 0; hi < 3; hi++){
    if(!STR_ischars_hex(str[(*i) + hi], hi))
      return 0;
  }
  (*i) += 2;
  uint32_t res = 0;
  for(; *i < size; (*i)++){
    if(!STR_ischar_hexdigit(str[*i]))
      break;
    res <<= 4;
    res |= _STR_psh_c(str[*i]);
  }
  return res;
}
#define STR_psh32_i(str_m, i_m, size_m) \
  STR_psh32_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
uint64_t STR_psh64_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
  if(size < 3)
    return 0;
  for(uintptr_t hi = 0; hi < 3; hi++){
    if(!STR_ischars_hex(str[(*i) + hi], hi))
      return 0;
  }
  (*i) += 2;
  uint64_t res = 0;
  for(; *i < size; (*i)++){
    if(!STR_ischar_hexdigit(str[*i]))
      break;
    res <<= 4;
    res |= _STR_psh_c(str[*i]);
  }
  return res;
}
#define STR_psh64_i(str_m, i_m, size_m) \
  STR_psh64_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
#define STR_psh_i CONCAT3(STR_psh, SYSTEM_BIT, _i)
