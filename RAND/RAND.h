#ifndef _WITCH_libdefine_RAND
#define _WITCH_libdefine_RAND

/* bob jenkins prng */
static uint32_t RAND_bjprng32(uint32_t p){
  p += (p << 10);
  p ^= (p >> 6);
  p += (p << 3);
  p ^= (p >> 11);
  p += (p << 15);
  return p;
}

static uint32_t RAND_csoft3uint32_uint32(uint32_t p0, uint32_t p1, uint32_t p2){
  uint32_t r = 0;
  r = RAND_bjprng32(r + p0);
  r = RAND_bjprng32(r + p1);
  r = RAND_bjprng32(r + p2);
  return r;
}

#if defined(__platform_libc)
  /* TODO for no libc */

  #if defined(__platform_unix)
    #include <sys/random.h>
  #elif defined(__platform_windows)
    #include <bcrypt.h>
    #pragma comment(lib, "bcrypt.lib")
  #endif

  static void RAND_hard_ram(void *ptr, uintptr_t size){
    #if defined(__platform_unix)
      if(getrandom(ptr, size, GRND_RANDOM) < 0){
        __abort();
      }
    #elif defined(__platform_windows)
      NTSTATUS status = BCryptGenRandom(
        NULL,                   
        (PUCHAR)ptr,            
        (ULONG)size,         
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
      );
    
      if (!BCRYPT_SUCCESS(status)) {
        __abort();
      }
    #else
      #error ?
    #endif
  }

  static uint8_t RAND_hard_8(void){
    uint8_t r;
    RAND_hard_ram(&r, 1);
    return r;
  }
  static uint16_t RAND_hard_16(void){
    uint16_t r;
    RAND_hard_ram(&r, 2);
    return r;
  }
  static uint32_t RAND_hard_32(void){
    uint32_t r;
    RAND_hard_ram(&r, 4);
    return r;
  }
  static uint64_t RAND_hard_64(void){
    uint64_t r;
    RAND_hard_ram(&r, 8);
    return r;
  }
  static uintptr_t RAND_hard_0(void){
    return CONCAT(RAND_hard_, SYSTEM_BIT)();
  }

  static sint8_t RAND_hard_snum8(sint8_t min, sint8_t max){
    return RAND_hard_8() % ((max - min) + 1) + min;
  }
  static sint16_t RAND_hard_snum16(sint16_t min, sint16_t max){
    return RAND_hard_16() % ((max - min) + 1) + min;
  }
  static sint32_t RAND_hard_snum32(sint32_t min, sint32_t max){
    return RAND_hard_32() % ((max - min) + 1) + min;
  }
  static sint64_t RAND_hard_snum64(sint64_t min, sint64_t max){
    return RAND_hard_64() % ((max - min) + 1) + min;
  }
  static sintptr_t RAND_hard_snum(sintptr_t min, sintptr_t max){
    return RAND_hard_0() % ((max - min) + 1) + min;
  }

#endif

static void _RAND_internal_open(){

}
static void _RAND_internal_close(){

}

#ifdef _WITCH_libdefine_PlatformOpen
  #error ?
#endif

#ifdef PRE
  PRE{
    _RAND_internal_open();
  }
#endif
/* TODO _RAND_internal_close needs to be called if its automaticly possible */

#endif
