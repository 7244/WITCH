#pragma once

#include _WITCH_PATH(STR/common/common.h)

static uint32_t STR_psu32(const uint8_t *str, uintptr_t size){
  uint32_t res = 0;
  for(uintptr_t i = 0; i < size; i++)
    res = res * 10 + str[i] - '0';
  return res;
}
static uint64_t STR_psu64(const uint8_t *str, uintptr_t size){
  uint64_t res = 0;
  for(uintptr_t i = 0; i < size; i++)
    res = res * 10 + str[i] - '0';
  return res;
}
static uintptr_t STR_psu(const uint8_t *str, uintptr_t size){
  return CONCAT(STR_psu, SYSTEM_BIT)(str, size);
}

static uint32_t STR_psu32_iguess(const uint8_t *str, uintptr_t *i){
  uint32_t res = 0;
  for(; STR_ischar_digit(str[*i]); (*i)++)
    res = res * 10 + str[*i] - '0';
  return res;
}
static uint64_t STR_psu64_iguess(const uint8_t *str, uintptr_t *i){
  uint64_t res = 0;
  for(; STR_ischar_digit(str[*i]); (*i)++)
    res = res * 10 + str[*i] - '0';
  return res;
}
static uintptr_t STR_psu_iguess(const uint8_t *str, uintptr_t *i){
  return CONCAT(STR_psu, _iguess)(str, i);
}

static uint32_t STR_psu32_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
  uint32_t res = 0;
  for(; *i < size; (*i)++){
    if(!STR_ischar_digit(str[*i]))
      break;
    res = res * 10 + str[*i] - '0';
  }
  return res;
}
static uint64_t STR_psu64_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
  uint64_t res = 0;
  for(; *i < size; (*i)++){
    if(!STR_ischar_digit(str[*i]))
      break;
    res = res * 10 + str[*i] - '0';
  }
  return res;
}
static uintptr_t STR_psu_i(const uint8_t *str, uintptr_t *i, const uintptr_t size){
  return CONCAT3(STR_psu, SYSTEM_BIT, _i)(str, i, size);
}