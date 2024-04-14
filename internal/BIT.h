#pragma once

#if defined(_MSC_VER)
  #include <intrin.h>
#endif

#ifndef __clz
  #define __clz __clz
  static
  uint8_t
  __clz32(uint32_t p0){
    #if defined(__GNUC__)
      return __builtin_clz(p0);
    #elif defined(_MSC_VER)
      DWORD trailing_zero;
      if(_BitScanReverse(&trailing_zero, p0)){
        return 31 - trailing_zero;
      }
      else{
        return 32;
      }
    #else
      #error ?
    #endif
  }
  static
  uint8_t
  __clz64(uint64_t p0){
    #if defined(__GNUC__)
      return __builtin_clzll(p0);
    #elif defined(_MSC_VER)
      DWORD trailing_zero;
      if(_BitScanReverse64(&trailing_zero, p0)){
        return 63 - trailing_zero;
      }
      else{
        return 64;
      }
    #else
      #error ?
    #endif
  }
  static
  uint8_t
  __clz(uintptr_t p0){
    #if SYSTEM_BIT == 32
      return __clz32(p0);
    #elif SYSTEM_BIT == 64
      return __clz64(p0);
    #else
      #error ?
    #endif
  }
#endif

static uint8_t CTZ32(uint32_t x){
  #if defined(__GNUC__)
    return __builtin_ctzl(x);
  #elif defined(_MSC_VER)
    unsigned long ret;
    _BitScanForward(&ret, x);
    return ret;
  #else
    for(uint8_t i = 0; x & 1; x >>= 1){
      return i;
    }
  #endif
}
static uint8_t CTZ64(uint64_t x){
  #if defined(__GNUC__)
    return __builtin_ctzll(x);
  #elif defined(_MSC_VER)
    unsigned long ret;
    _BitScanForward64(&ret, x);
    return ret;
  #else
    for(uint8_t i = 0; x & 1; x >>= 1){
      return i;
    }
  #endif
}

#if defined(__GLIBC__)
  #include <byteswap.h>
  #define byteswap16(_m) bswap_16(_m)
  #define byteswap32(_m) bswap_32(_m)
  #define byteswap64(_m) bswap_64(_m)
#elif defined(_MSC_VER)
  #include <stdlib.h>
  #define byteswap16(_m) _byteswap_ushort(_m)
  #define byteswap32(_m) _byteswap_ulong(_m)
  #define byteswap64(_m) _byteswap_uint64(_m)
#else
  static uint16_t byteswap16(uint16_t v){
    return (v >> 8) | (v << 8);
  }
  static uint32_t byteswap32(uint32_t v){
    return 
      ((v >> 24) & 0x000000ff) |
      ((v >> 8 ) & 0x0000ff00) |
      ((v << 8 ) & 0x00ff0000) |
      ((v << 24) & 0xff000000)
    ;
  }
  static uint64_t byteswap64(uint64_t v){
    return 
      ((v >> 56) & 0x00000000000000ff) |
      ((v >> 48) & 0x000000000000ff00) |
      ((v >> 40) & 0x0000000000ff0000) |
      ((v >> 8 ) & 0x00000000ff000000) |
      ((v << 8 ) & 0x000000ff00000000) |
      ((v << 16) & 0x0000ff0000000000) |
      ((v << 24) & 0x00ff000000000000) |
      ((v << 56) & 0xff00000000000000)
    ;
  }
#endif

#if ENDIAN == 0
  #define e0swap16(_m) (_m)
  #define e0swap32(_m) (_m)
  #define e0swap64(_m) (_m)
  #define e1swap16(_m) byteswap16(_m)
  #define e1swap32(_m) byteswap32(_m)
  #define e1swap64(_m) byteswap64(_m)
#else
  #define e0swap16(_m) byteswap16(_m)
  #define e0swap32(_m) byteswap32(_m)
  #define e0swap64(_m) byteswap64(_m)
  #define e1swap16(_m) (_m)
  #define e1swap32(_m) (_m)
  #define e1swap64(_m) (_m)
#endif

static uint8_t bitswap8(uint8_t v){
  v = (v & 0xf0) >> 4 | (v & 0x0f) << 4;
  v = (v & 0xcc) >> 2 | (v & 0x33) << 2;
  v = (v & 0xaa) >> 1 | (v & 0x55) << 1;
  return v;
}
static uint16_t bitswap16(uint16_t v){
  v = byteswap16(v);
  uint8_t *n = (uint8_t *)&v;
  n[0] = bitswap8(n[0]);
  n[1] = bitswap8(n[1]);
  return v;
}
static uint32_t bitswap32(uint32_t v){
  v = byteswap32(v);
  uint8_t *n = (uint8_t *)&v;
  n[0] = bitswap8(n[0]);
  n[1] = bitswap8(n[1]);
  n[2] = bitswap8(n[2]);
  n[3] = bitswap8(n[3]);
  return v;
}
static uint64_t bitswap64(uint64_t v){
  v = byteswap64(v);
  uint8_t *n = (uint8_t *)&v;
  n[0] = bitswap8(n[0]);
  n[1] = bitswap8(n[1]);
  n[2] = bitswap8(n[2]);
  n[3] = bitswap8(n[3]);
  n[4] = bitswap8(n[4]);
  n[5] = bitswap8(n[5]);
  n[6] = bitswap8(n[6]);
  n[7] = bitswap8(n[7]);
  return v;
}
