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

static uint32_t STR_psu32_iguess(const void *str, uintptr_t *i){
  uint32_t res = 0;
  for(; STR_ischar_digit(((uint8_t *)str)[*i]); (*i)++)
    res = res * 10 + ((uint8_t *)str)[*i] - '0';
  return res;
}
static uint64_t STR_psu64_iguess(const void *str, uintptr_t *i){
  uint64_t res = 0;
  for(; STR_ischar_digit(((uint8_t *)str)[*i]); (*i)++)
    res = res * 10 + ((uint8_t *)str)[*i] - '0';
  return res;
}
static uintptr_t STR_psu_iguess(const void *str, uintptr_t *i){
  return CONCAT3(STR_psu, SYSTEM_BIT, _iguess)(str, i);
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