#ifndef __fast_8log2
  #define __fast_8log2 __fast_8log2
  __forceinline static uint8_t __fast_8log2(uint8_t v){
    return 31 - __clz32(v);
  }
#endif
#ifndef __fast_16log2
  #define __fast_16log2 __fast_16log2
  __forceinline static uint8_t __fast_16log2(uint16_t v){
    return 31 - __clz32(v);
  }
#endif
#ifndef __fast_32log2
  #define __fast_32log2 __fast_32log2
  __forceinline static uint8_t __fast_32log2(uint32_t v){
    return 31 - __clz32(v);
  }
#endif
#ifndef __fast_64log2
  #define __fast_64log2 __fast_64log2
  __forceinline static uint8_t __fast_64log2(uint64_t v){
    return 63 - __clz64(v);
  }
#endif
#ifndef __fast_log2
  #define __fast_log2 CONCAT3(__fast_,SYSTEM_BIT,log2)
#endif


