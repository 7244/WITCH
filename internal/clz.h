#ifndef __clz32
  #if defined(__compiler_clang) || \
    defined(__compiler_gcc) || \
    defined(__compiler_tinyc)

    #define __clz32 __builtin_clz
  #elif defined(__compiler_msvc)

    #define __clz32 __clz32

    static
    uint8_t
    __clz32(uint32_t p0){
      unsigned long trailing_zero;
      if(_BitScanReverse(&trailing_zero, p0)){
        return 31 - trailing_zero;
      }
      else{
        return 32;
      }
    }
  #else
    #error ?
  #endif
#endif

#ifndef __clz64
  #if defined(__compiler_clang) || \
    defined(__compiler_gcc) || \
    defined(__compiler_tinyc)

    #define __clz64 __builtin_clzll
  #elif defined(__compiler_msvc)

    #define __clz64 __clz64

    static
    uint8_t
    __clz64(uint64_t p0){
      unsigned long trailing_zero;
      if(_BitScanReverse64(&trailing_zero, p0)){
        return 63 - trailing_zero;
      }
      else{
        return 64;
      }
    }
  #else
    #error ?
  #endif
#endif

#ifndef __clz
  #define __clz CONCAT(__clz,SYSTEM_BIT)
#endif
