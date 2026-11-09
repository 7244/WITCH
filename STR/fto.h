#pragma once

#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(STR/uto.h)

#if SYSTEM_BIT == 8
  #define bdn10 2
#elif SYSTEM_BIT == 16
  #define bdn10 4
#elif SYSTEM_BIT == 32
  #define bdn10 9
#elif SYSTEM_BIT == 64
  #define bdn10 19
#else
  #error ?
#endif

static bool STR_fto128_base10(f128_t v, uintptr_t precision, uint8_t *data, uintptr_t *size){
  const uintptr_t _STR_fto_digit10[bdn10 + 1] = {
    0u,
    #if SYSTEM_BIT >= 8
      10u,
      100u
    #endif
    #if SYSTEM_BIT >= 16
      ,
      1000u,
      10000u
    #endif
    #if SYSTEM_BIT >= 32
      ,
      100000u,
      1000000u,
      10000000u,
      100000000u,
      1000000000u
    #endif
    #if SYSTEM_BIT >= 64
      ,
      10000000000u,
      100000000000u,
      1000000000000u,
      10000000000000u,
      100000000000000u,
      1000000000000000u,
      10000000000000000u,
      100000000000000000u,
      1000000000000000000u,
      10000000000000000000u
    #endif
  };

  const uint8_t *origdata = data;
  if(v < 0)
    *data++ = '-';
  v = ABS(v);
  uintptr_t left = v;
  v -= left;
  {
    uint8_t *utostr = data;
    uintptr_t utosize;
    if(STR_uto(left, 10, &utostr, &utosize))
      return 1;
    __builtin_memmove(data, utostr, utosize);
    data += utosize;
  }
  if(precision)
    *data++ = '.';
  while(precision){
    uintptr_t n = precision > bdn10 ? bdn10 : precision;
    precision -= n;
    v *= _STR_fto_digit10[n];
    left = v;
    v -= left;
    __builtin_memset(data, '0', n);
    if(STR_uto_wise(left, 10, n - 1, data))
      return 1;
    data += n;
  }
  *size = data - origdata;
  return 0;
}

#undef bdn10
