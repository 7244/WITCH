#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(STR/common/common.h)

#define common(bit_m, ref_m) \
  CONCAT3(sint, bit_m, _t) res = 0; \
  CONCAT3(sint, bit_m, _t) sign; \
  switch(str[ref_m i]){ \
    case '-': \
      sign = -1; \
      (ref_m i)++; \
      break; \
    case '+': \
      sign = 1; \
      (ref_m i)++; \
      break; \
    default: \
      if(!STR_ischar_digit(str[ref_m i])) \
        return 0; \
      sign = 1; \
  }

static sint32_t STR_pss32(const uint8_t *str, uintptr_t size){
  if(!size)
    return 0;
  uintptr_t i = 0;
  common(32,)
  for(; i < size; i++)
    res = res * 10 + str[i] - '0';
  return res * sign;
}
#define STR_pss32(str_m, size_m) \
  STR_pss32((const uint8_t *)(str_m), (uintptr_t)(size_m))
static sint64_t STR_pss64(const uint8_t *str, uintptr_t size){
  if(!size)
    return 0;
  uintptr_t i = 0;
  common(64,)
  for(; i < size; i++)
    res = res * 10 + str[i] - '0';
  return res * sign;
}
#define STR_pss64(str_m, size_m) \
  STR_pss64((const uint8_t *)(str_m), (uintptr_t)(size_m))
#define STR_pss CONCAT(STR_pss, SYSTEM_BIT)

static sint32_t STR_pss32_iguess(const uint8_t *str, uintptr_t *i){
  common(32, *)
  for(; STR_ischar_digit(str[*i]); (*i)++)
    res = res * 10 + str[*i] - '0';
  return res * sign;
}
#define STR_pss32_iguess(str_m, i_m) \
  STR_pss32_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
static sint64_t STR_pss64_iguess(const uint8_t *str, uintptr_t *i){
  common(64, *)
  for(; STR_ischar_digit(str[*i]); (*i)++)
    res = res * 10 + str[*i] - '0';
  return res * sign;
}
#define STR_pss64_iguess(str_m, i_m) \
  STR_pss64_iguess((const uint8_t *)(str_m), (uintptr_t *)(i_m))
#define STR_pss_iguess CONCAT3(STR_pss, SYSTEM_BIT, _iguess)

static sint32_t STR_pss32_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
  common(32, *)
  for(; *i < size; (*i)++){
    if(!STR_ischar_digit(str[*i]))
      break;
    res = res * 10 + str[*i] - '0';
  }
  return res * sign;
}
#define STR_pss32_i(str_m, i_m, size_m) \
  STR_pss32_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
static sint64_t STR_pss64_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
  common(64, *)
  for(; *i < size; (*i)++){
    if(!STR_ischar_digit(str[*i]))
      break;
    res = res * 10 + str[*i] - '0';
  }
  return res * sign;
}
#define STR_pss64_i(str_m, i_m, size_m) \
  STR_pss64_i((const uint8_t *)(str_m), (uintptr_t *)(i_m), (const uintptr_t)(size_m))
#define STR_pss_i CONCAT3(STR_pss, SYSTEM_BIT, _i)

#undef common