#pragma once

#ifdef __STDC_VERSION__
  #define __language_c __STDC_VERSION__
#endif
#ifdef __cplusplus
  #define __language_cpp __cplusplus

  #if __language_cpp <= 202002L
    /* cpp has bug where `new (ptr) type` requires you to include <new> */
    /* `new` is listed as keyword. it shouldnt need a include to work. */
    #include <new>
  #else
    #warning does this bug still exists?
  #endif
#endif

#ifndef __compiler
  #if defined(__clang__)
    #define __compiler_clang
  #elif defined(__TINYC__)
    #define __compiler_tinyc
  #elif defined(__GNUC__) || defined(__GNUG__)
    #define __compiler_gcc
  #elif defined(_MSC_VER)
    #define __compiler_msvc
  #else
    #error failed to find __compiler
  #endif
  #define __compiler
#endif

#include _WITCH_PATH(internal/WOS.h)

#if defined(__compiler_clang)
  #if !defined(__clang_major__)
    #warning cant get clang major version. expect brokenness
  #elif __clang_major__ == 18
    #if !defined(__platform_libc)
      /* clang still tries to use memset memcpy even when it doesnt exists */

      #if defined(__platform_windows)
        #error how to say uintptr_t in windows's clang?
      #endif

      void memset(
        void * dst,
        unsigned char byte,
        const unsigned long size
      ){
        for(unsigned long i = 0; i < size; i++){
          ((unsigned char *)dst)[i] = byte;
        }
      }

      void memcpy(
        void * restrict dst,
        const void * restrict src,
        const unsigned long size
      ){
        for(unsigned long i = 0; i < size; i++){
          ((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
        }
      }
    #endif
  #endif
#endif

#ifndef WITCH_PRE_is_not_allowed
  #ifndef PRE
    #include _WITCH_PATH(internal/PRE.h)
  #endif
#endif

#ifndef __sanit
  #if defined(__SANITIZE_ADDRESS__)
    #define __sanit 1
  #endif
#endif

#ifndef __sanit
  #if defined(__has_feature)
    #if __has_feature(address_sanitizer)
      #define __sanit 1
    #endif
  #endif
#endif

#ifndef __sanit
  #define __sanit 0
#endif

#ifndef ENDIAN
  #if defined(__BYTE_ORDER)
    #if __BYTE_ORDER == __BIG_ENDIAN
      #define ENDIAN 0
    #elif __BYTE_ORDER == __LITTLE_ENDIAN
      #define ENDIAN 1
    #else
      #error ?
    #endif
  #elif defined(__BYTE_ORDER__)
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      #define ENDIAN 0
    #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      #define ENDIAN 1
    #else
      #error ?
    #endif
  #elif defined(__x86_64__) && __x86_64__ == 1
    #define ENDIAN 1
  #elif defined(__platform_windows)
    #define ENDIAN 1
  #else
    #error ?
  #endif
#endif

/* naming got from kernel code */
/* e stands for expand */
#if ENDIAN == 0
  #define arg_u32e(v) (v) >> 32, (v)
#elif ENDIAN == 1
  #define arg_u32e(v) (v), (v) >> 32
#endif

#ifndef __has_builtin
  #define __has_builtin(expr) 0
#endif

#ifndef EXPECT
  #if defined(__clang__)
    #define EXPECT(expr, val) __builtin_expect((expr), (val))
  #elif defined(__GNUC__)
    #define EXPECT(expr, val) __builtin_expect((expr), (val))
  #else
    #define EXPECT(expr, val) (expr)
  #endif
#endif

#ifndef __ca__
  #define __ca__ ,
#endif

#ifndef __platform_linux_kernel_module
  #include <stddef.h>
  #include <stdbool.h>
  #include <stdarg.h>
  #include <float.h>
#endif

#if defined(__x86_64__) || defined(_M_AMD64)
  #define SYSTEM_BIT 64
  #define SYSTEM_BYTE 8
#elif defined(__i386__) || defined(__arm__)
  #define SYSTEM_BIT 32
  #define SYSTEM_BYTE 4
#elif defined(__SIZEOF_POINTER__)
  #if __SIZEOF_POINTER__ == 8
    #define SYSTEM_BIT 64
    #define SYSTEM_BYTE 8
  #elif __SIZEOF_POINTER__ == 4
    #define SYSTEM_BIT 32
    #define SYSTEM_BYTE 4
  #else
    #error ?
  #endif
#else
  #error ?
#endif

#if defined(__platform_libc)
  #include <stdint.h>
  typedef intptr_t sintptr_t;
  typedef int8_t sint8_t;
  typedef int16_t sint16_t;
  typedef int32_t sint32_t;
  typedef int64_t sint64_t;
#else
  #if SYSTEM_BIT == 64
    typedef signed long sintptr_t;
    typedef signed char sint8_t;
    typedef signed short sint16_t;
    typedef signed int sint32_t;
    typedef sintptr_t sint64_t;
    #ifndef WITCH_unsigned_types_will_be_defined
      typedef unsigned long uintptr_t;
      typedef unsigned char uint8_t;
      typedef unsigned short uint16_t;
      typedef unsigned int uint32_t;
      typedef unsigned long long uint64_t;
    #endif
  #elif SYSTEM_BIT == 32
    typedef signed long sintptr_t;
    typedef signed char sint8_t;
    typedef signed short sint16_t;
    typedef sintptr_t sint32_t;
    typedef signed long long sint64_t;
    #ifndef WITCH_unsigned_types_will_be_defined
      typedef unsigned long uintptr_t;
      typedef unsigned char uint8_t;
      typedef unsigned short uint16_t;
      typedef unsigned int uint32_t;
      typedef unsigned long long uint64_t;
    #endif
  #else
    #error ?
  #endif
#endif

/* float */
typedef float f32_t;
typedef double f64_t;
typedef long double f128_t;
typedef CONCAT3(f, SYSTEM_BIT, _t) f_t;

#define _LITERAL_uint8_t u
#define _LITERAL_uint16_t u
#define _LITERAL_uint32_t lu
#define _LITERAL_uint64_t llu
#define _LITERAL_uintptr_t CONCAT3(_LITERAL_uint, SYSTEM_BIT, _t)
#define _LITERAL_sint8_t
#define _LITERAL_sint16_t
#define _LITERAL_sint32_t l
#define _LITERAL_sint64_t ll
#define _LITERAL_sintptr_t CONCAT3(_LITERAL_sint, SYSTEM_BIT, _t)
#define _LITERAL_f32_t f
#define _LITERAL_f64_t L
#define _LITERAL_f128_t L
#define _LITERAL_f_t CONCAT3(_LITERAL_f, SYSTEM_BIT, _t)
#define LITERAL(type_m, num_m) \
  (type_m)CONCAT(num_m, CONCAT(_LITERAL_, type_m))

#ifndef CLAMP
  #define CLAMP(val, lo, hi) \
    ((val) <= (lo) ? (lo) : (val) > (hi) ? (hi) : (val))
#endif
#ifndef ABS
  #define ABS(num_m) \
    ((num_m) < 0 ? -(num_m) : (num_m))
#endif
#ifndef FLOOR
  #define FLOOR \
    (f_t)(sintptr_t)
#endif
#ifndef FMOD
  #define FMOD(num_m, mod_m) \
    ((num_m) - (FLOOR((num_m) / (mod_m)) * (mod_m)))
#endif
#ifndef SIGN
  #define SIGN(_m) \
    (sintptr_t)(-(_m < 0) | (_m > 0))
#endif
#ifndef RSIGN
  #define RSIGN(_m) \
    ((_m) * SIGN(_m))
#endif

static uint32_t LOG32(uintptr_t num, uint8_t base){
  uint32_t log = 0;
  while(num){
    num /= base;
    log++;
  }
  return log;
}
static uint64_t LOG64(uint64_t num, uint8_t base){
  uint64_t log = 0;
  while(num){
    num /= base;
    log++;
  }
  return log;
}
static uintptr_t LOG(uintptr_t num, uint8_t base){
  return CONCAT(LOG, SYSTEM_BIT)(num, base);
}

#ifndef OFFSETLESS
  #define OFFSETLESS(ptr_m, t_m, d_m) \
    ((t_m *)((uint8_t *)(ptr_m) - offsetof(t_m, d_m)))
#endif

#ifndef __unreachable
  #if defined(__compiler_clang) || defined(__compiler_gcc)
    #define __unreachable() __builtin_unreachable()
  #elif defined(__compiler_tinyc)
    #warning __compiler_tinyc doesnt support __unreachable(). expect future warning/error
    #define __unreachable()
  #elif defined(__compiler_msvc)
    #define __unreachable() __assume(0)
  #else
    #error ?
  #endif
#endif

#if defined(__language_cpp)
  #define WITCH_c(_m) _m
#else
  #define WITCH_c(_m) (_m)
#endif

/* compile time assert */
#ifndef __cta
  #if defined(__language_cpp)
    #define __cta(X) static_assert(X)
  #else
    /* taken from stackoverflow.com/a/3385694 */
    #define __cta_static_assert(COND,MSG) typedef char __cta_##MSG[(!!(COND))*2-1]
    #define __cta_3(X,L) __cta_static_assert(X,static_assertion_at_line_##L)
    #define __cta_2(X,L) __cta_3(X,L)
    #define __cta(X) __cta_2(X,__LINE__)
  #endif
#endif

#include "atomic.h"

#ifndef __empty_struct
  #define __empty_struct __empty_struct
  typedef struct{
  }__empty_struct;
#endif

#if defined(__language_cpp)
  #ifndef __is_type_same
    template <typename, typename>
    inline constexpr bool __is_type_same = false;
    template <typename _Ty>
    inline constexpr bool __is_type_same<_Ty, _Ty> = true;
    #define __is_type_same __is_type_same
  #endif

  #ifndef __dme
    #pragma pack(push, 1)

    template<typename T, const char* str, uintptr_t dt_size>
    struct __dme_t : T {
      const char *sn = str;
      uint32_t m_DSS = dt_size;
    };

    template<typename T>
    using return_type_of_t = decltype((*(T*)nullptr)());

    inline char __dme_empty_string[] = "";

    #define __dme(varname, ...) \
      struct varname##structed_dt{ \
        __VA_ARGS__ \
      }; \
      struct varname##_t : varname##structed_dt{ \
        constexpr operator uintptr_t() const { return __COUNTER__ - DME_INTERNAL__BEG - 1; } \
        static inline constexpr uintptr_t dss = (sizeof(#__VA_ARGS__) > 1) * sizeof(varname##structed_dt); \
      }; \
      inline static struct varname##_t varname; \
      static inline constexpr char varname##_str[] = #varname; \
      __dme_t<value_type, varname##_str, varname##_t::dss> varname##_ram

    template <typename main_t, uintptr_t index, typename T = __empty_struct>
    struct __dme_inherit_t{
      using value_type = T;
      using dme_type_t = __dme_t<value_type, __dme_empty_string, 0>;
      constexpr auto* NA(uintptr_t I) const { return &((dme_type_t *)this)[I]; }
      static constexpr uintptr_t GetMemberAmount() { return sizeof(main_t) / sizeof(dme_type_t); }
      static constexpr auto DME_INTERNAL__BEG = index;
    };

    #define __dme_inherit(main_t, ...) __dme_inherit_t<main_t, __COUNTER__, ##__VA_ARGS__>

    #pragma pack(pop)
  #endif
#endif

/* TODO make memorycopy dst src */
/* src, dst, size */
#ifndef __MemoryCopy
  #if defined(__compiler_clang) || defined(__compiler_gcc) || defined(__compiler_tinyc)
    #define __MemoryCopy(src, dst, size) __builtin_memcpy(dst, src, size)
  #elif defined(__compiler_msvc)
    #define __MemoryCopy(src, dst, size) memcpy(dst, src, size)
  #else
    #error ?
  #endif
#endif
/* TODO make memoryset dst src */
/* byte, dst, size */
#ifndef __MemorySet
  #if defined(__compiler_clang) || defined(__compiler_gcc) || defined(__compiler_tinyc)
    #define __MemorySet(byte, dst, size) __builtin_memset(dst, byte, size)
  #elif defined(__compiler_msvc)
    #define __MemorySet(byte, dst, size) memset(dst, byte, size)
  #else
    #error ?
  #endif
#endif

#ifndef __MemoryMove
  #if defined(__compiler_clang) || defined(__compiler_gcc) || defined(__compiler_tinyc)
    #define __MemoryMove(dst, src, size) __builtin_memmove(dst, src, size)
  #elif defined(__compiler_msvc)
    #define __MemoryMove(dst, src, size) memmove(dst, src, size)
  #else
    #error ?
  #endif
#endif

#ifndef __abort
  #define __abort() do{ \
    __simplest_abort(); \
    __unreachable(); \
  }while(0)
  static void __simplest_abort(){
    #if defined(__platform_unix) || defined(__platform_windows)
      /* write to kernel owned address from userside. should guarantee crash. */
      *(uintptr_t *)((uintptr_t)1 << SYSTEM_BIT - 1) = 0;
    #elif defined(__platform_bpf)
      /* should die at verifier */
      while(1){}
    #else
      #error ?
    #endif
  }
#endif

#define lstd_preprocessor_get_argn(p0, p1, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, n, ...) n
#define lstd_preprocessor_get_arg_count(...) EXPAND(lstd_preprocessor_get_argn(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#ifndef lstd_preprocessor_ignore_first_of_every_2
  #define _lstd_preprocessor_ignore_first_of_every_2_2(p0, p1) p1
  #define _lstd_preprocessor_ignore_first_of_every_2_4(p0, p1, ...) p1, EXPAND(_lstd_preprocessor_ignore_first_of_every_2_2(__VA_ARGS__))
  #define _lstd_preprocessor_ignore_first_of_every_2_6(p0, p1, ...) p1, EXPAND(_lstd_preprocessor_ignore_first_of_every_2_4(__VA_ARGS__))
  #define _lstd_preprocessor_ignore_first_of_every_2_8(p0, p1, ...) p1, EXPAND(_lstd_preprocessor_ignore_first_of_every_2_6(__VA_ARGS__))
  #define _lstd_preprocessor_ignore_first_of_every_2_10(p0, p1, ...) p1, EXPAND(_lstd_preprocessor_ignore_first_of_every_2_8(__VA_ARGS__))
  #define _lstd_preprocessor_ignore_first_of_every_2_12(p0, p1, ...) p1, EXPAND(_lstd_preprocessor_ignore_first_of_every_2_10(__VA_ARGS__))
  #define _lstd_preprocessor_ignore_first_of_every_2_14(p0, p1, ...) p1, EXPAND(_lstd_preprocessor_ignore_first_of_every_2_12(__VA_ARGS__))
  #define _lstd_preprocessor_ignore_first_of_every_2_16(p0, p1, ...) p1, EXPAND(_lstd_preprocessor_ignore_first_of_every_2_14(__VA_ARGS__))
  #define _lstd_preprocessor_ignore_first_of_every_2_start(n, ...) CONCAT(_lstd_preprocessor_ignore_first_of_every_2_,n(__VA_ARGS__))
  #define lstd_preprocessor_ignore_first_of_every_2(...) _lstd_preprocessor_ignore_first_of_every_2_start(lstd_preprocessor_get_arg_count(__VA_ARGS__), __VA_ARGS__)
#endif

#ifndef lstd_preprocessor_combine_every_2
  #define _lstd_preprocessor_combine_every_2_2(p0, p1) p0 p1
  #define _lstd_preprocessor_combine_every_2_4(p0, p1, ...) p0 p1, EXPAND(_lstd_preprocessor_combine_every_2_2(__VA_ARGS__))
  #define _lstd_preprocessor_combine_every_2_6(p0, p1, ...) p0 p1, EXPAND(_lstd_preprocessor_combine_every_2_4(__VA_ARGS__))
  #define _lstd_preprocessor_combine_every_2_8(p0, p1, ...) p0 p1, EXPAND(_lstd_preprocessor_combine_every_2_6(__VA_ARGS__))
  #define _lstd_preprocessor_combine_every_2_10(p0, p1, ...) p0 p1, EXPAND(_lstd_preprocessor_combine_every_2_8(__VA_ARGS__))
  #define _lstd_preprocessor_combine_every_2_12(p0, p1, ...) p0 p1, EXPAND(_lstd_preprocessor_combine_every_2_10(__VA_ARGS__))
  #define _lstd_preprocessor_combine_every_2_14(p0, p1, ...) p0 p1, EXPAND(_lstd_preprocessor_combine_every_2_12(__VA_ARGS__))
  #define _lstd_preprocessor_combine_every_2_16(p0, p1, ...) p0 p1, EXPAND(_lstd_preprocessor_combine_every_2_14(__VA_ARGS__))
  #define _lstd_preprocessor_combine_every_2_start(n, ...) CONCAT(_lstd_preprocessor_combine_every_2_,n(__VA_ARGS__))
  #define lstd_preprocessor_combine_every_2(...) _lstd_preprocessor_combine_every_2_start(lstd_preprocessor_get_arg_count(__VA_ARGS__), __VA_ARGS__)
#endif

#ifdef __language_cpp
  #ifndef lstd_defastruct
    using lstd_current_type = void;
    #define lstd_defastruct(name, ...) \
      struct CONCAT(name,_t){ \
        using lstd_parent_type = lstd_current_type; \
        using lstd_current_type = CONCAT(name,_t); \
        struct lstd_parent_t{ \
          auto* operator->(){ \
            auto current = OFFSETLESS(this, CONCAT(name,_t), lstd_parent); \
            return OFFSETLESS(current, lstd_parent_type, name); \
          } \
        }lstd_parent; \
        __VA_ARGS__ \
      }name;
  #endif
  #ifndef lstd_defstruct
    #define lstd_defstruct(type_name) \
      struct type_name{ \
        using lstd_current_type = type_name;
  #endif

  auto& lstd_variadic_get_first(auto& f, auto&&... r){
    return f;
  }

  auto& _lstd_variadic_get_last(auto &p0, auto &p1, auto p2){
    return p2;
  }
  auto& _lstd_variadic_get_last(auto &p0, auto &p1){
    return p1;
  }
  auto& _lstd_variadic_get_last(auto &p0){
    return p0;
  }
  auto& lstd_variadic_get_last(auto&&... args){
    return _lstd_variadic_get_last(args...);
  }
#endif
