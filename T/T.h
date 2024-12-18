#ifndef _WITCH_libdefine_T
#define _WITCH_libdefine_T

#include _WITCH_PATH(PR/PR.h)

#if defined(__platform_libc)
  #include <time.h>
#elif defined(__platform_linux_kernel_module)
  #include <linux/timekeeping.h>
#endif

#if defined(__platform_windows)
  #include _WITCH_PATH(include/windows/windows.h)
  #ifdef _MSC_VER
    /* rdtsc */
    #include <intrin.h>
  #endif
#endif

static uint64_t T_rdtsc(void){
  #ifdef _MSC_VER
    return __rdtsc();
  #elif defined(__ARM_ARCH)
    #if __ARM_ARCH >= 6
      uint32_t pmccntr;
      uint32_t pmuseren;
      uint32_t pmcntenset;
      asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(pmuseren));
      if(pmuseren & 1){
        asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcntenset));
        if(pmcntenset & 0x80000000ul){
          asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
          return ((uint64_t)pmccntr) * 64;
        }
        PR_abort();
      }
      PR_abort();
    #endif
  #elif SYSTEM_BIT == 32
    /* TODO check cpuarch */
    uint64_t x;
    __asm__ volatile(".byte 0x0f, 0x31" : "=A"(x));
    return x;
  #elif SYSTEM_BIT == 64
    /* TODO check cpuarch */
    uint32_t hi, lo;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)lo) | (((uint64_t)hi) << 32);
  #else
    #error ?
  #endif
}

#if defined(__platform_windows)
  inline uint64_t _T_time_freqi;
  inline f64_t    _T_time_freqf;
#endif

static uint64_t T_nowi(void){
  #if defined(__platform_unix) && defined(__platform_libc)
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return ((uint64_t)t.tv_sec * 1000000000) + t.tv_nsec;
  #elif defined(__platform_windows)
    LARGE_INTEGER t;
    if(QueryPerformanceCounter(&t) == 0){
      PR_abort();
    }
    return t.QuadPart * _T_time_freqi;
  #elif defined(__platform_linux_kernel_module)
    struct timespec64 ts;
    ktime_get_raw_ts64(&ts);
    return ((uint64_t)ts.tv_sec * 1000000000) + ts.tv_nsec;
  #else
    #error ?
  #endif
}

#ifndef WITCH_float_is_disabled
  static f64_t T_nowf(void){
    #if defined(__platform_unix) && defined(__platform_libc)
      struct timespec t;
      clock_gettime(CLOCK_MONOTONIC, &t);
      return (f64_t)t.tv_sec + (f64_t)t.tv_nsec / 1000000000;
    #elif defined(__platform_windows)
      LARGE_INTEGER t;
      if(QueryPerformanceCounter(&t) == 0){
        PR_abort();
      }
      return (f64_t)t.QuadPart / _T_time_freqf;
    #elif defined(__platform_linux_kernel_module)
      struct timespec64 ts;
      ktime_get_raw_ts64(&ts);
      return (f64_t)ts.tv_sec + (f64_t)ts.tv_nsec / 1000000000;
    #else
      #error ?
    #endif
  }
#endif

typedef struct{
  uint32_t yr;
  uint8_t mo, d, w, hr, min, s;
}T_date_t;
static bool T_date(T_date_t *date){
  #if defined(__platform_libc)
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    if(!tm){
      return 1;
    }
    date->yr = tm->tm_year + 1900;
    date->mo = tm->tm_mon + 1;
    date->d = tm->tm_mday;
    date->w = tm->tm_wday;
    date->hr = tm->tm_hour;
    date->min = tm->tm_min;
    date->s = tm->tm_sec;
    return 0;
  #else
    /* TODO implement rest */
    return 0;
  #endif
}

static void _T_internal_open(){
  #if defined(__platform_windows)
    LARGE_INTEGER t;
    if(QueryPerformanceFrequency(&t) == 0){
      PR_abort();
    }
    _T_time_freqi = 1000000000 / t.QuadPart;
    _T_time_freqf = t.QuadPart;
  #endif
}
static void _T_internal_close(){
  
}

#ifdef _WITCH_libdefine_PlatformOpen
  #error ?
#endif

#ifdef PRE
  PRE{
    _T_internal_open();
  }
#endif
/* TODO _T_internal_close needs to be called if its automaticly possible */

#endif
