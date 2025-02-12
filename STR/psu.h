#pragma once

#include _WITCH_PATH(STR/common/common.h)

static uint32_t STR_psu32(const void *str, uintptr_t size){
  uint32_t res = 0;
  for(uintptr_t i = 0; i < size; i++)
    res = res * 10 + ((uint8_t *)str)[i] - '0';
  return res;
}
static uint64_t STR_psu64(const void *str, uintptr_t size){
  uint64_t res = 0;
  for(uintptr_t i = 0; i < size; i++)
    res = res * 10 + ((uint8_t *)str)[i] - '0';
  return res;
}
static uintptr_t STR_psu(const void *str, uintptr_t size){
  return CONCAT(STR_psu, SYSTEM_BIT)(str, size);
}

#define STR_def_psu_iguess(bit) \
  static uint ## bit ## _t STR_psu ## bit ## _iguess(const void *str, uintptr_t *i){ \
    uint ## bit ## _t res = 0; \
    for(; STR_ischar_digit(((uint8_t *)str)[*i]); (*i)++){ \
      res = res * 10 + ((uint8_t *)str)[*i] - '0'; \
    } \
    return res; \
  }
STR_def_psu_iguess(8)
STR_def_psu_iguess(16)
STR_def_psu_iguess(32)
STR_def_psu_iguess(64)
#undef STR_def_psu_iguess
static __forceinline uintptr_t STR_psu_iguess(const void *str, uintptr_t *i){
  return CONCAT3(STR_psu, SYSTEM_BIT, _iguess)(str, i);
}

#define STR_def_psu_iguess_abort(bit) \
  static uint ## bit ## _t STR_psu ## bit ##_iguess_abort(const void *str, uintptr_t *i){ \
    uint ## bit ## _t res = 0; \
    uintptr_t start = *i; \
    for(; STR_ischar_digit(((uint8_t *)str)[*i]); (*i)++){ \
      res = res * 10 + ((uint8_t *)str)[*i] - '0'; \
    } \
    if(start == *i){ \
      __abort(); \
    } \
    return res; \
  }
STR_def_psu_iguess_abort(8)
STR_def_psu_iguess_abort(16)
STR_def_psu_iguess_abort(32)
STR_def_psu_iguess_abort(64)
#undef STR_def_psu_iguess_abort
static __forceinline uintptr_t STR_psu_iguess_abort(const void *str, uintptr_t *i){
  return CONCAT3(STR_psu, SYSTEM_BIT, _iguess_abort)(str, i);
}

static uint32_t STR_psu32_i(const void *str, uintptr_t *i, const uintptr_t size){
  uint32_t res = 0;
  for(; *i < size; (*i)++){
    if(!STR_ischar_digit(((uint8_t *)str)[*i]))
      break;
    res = res * 10 + ((uint8_t *)str)[*i] - '0';
  }
  return res;
}
static uint64_t STR_psu64_i(const void *str, uintptr_t *i, const uintptr_t size){
  uint64_t res = 0;
  for(; *i < size; (*i)++){
    if(!STR_ischar_digit(((uint8_t *)str)[*i]))
      break;
    res = res * 10 + ((uint8_t *)str)[*i] - '0';
  }
  return res;
}
static uintptr_t STR_psu_i(const void *str, uintptr_t *i, const uintptr_t size){
  return CONCAT3(STR_psu, SYSTEM_BIT, _i)(str, i, size);
}