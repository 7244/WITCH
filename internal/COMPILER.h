#pragma once

/* c99 o nada */
#define WL_C 9000
#ifdef __cplusplus
  #define WL_CPP __cplusplus
#endif

#include _WITCH_PATH(internal/WOS.h)
#ifndef WITCH_PRE_is_not_allowed
  #include _WITCH_PATH(internal/PRE.h)
#endif

#ifndef __compiler
  #if defined(__clang__)
    #define __compiler_clang
  #elif defined(__GNUC__) || defined(__GNUG__)
    #define __compiler_gcc
  #elif defined(_MSC_VER)
    #define __compiler_msvc
  #else
    #error failed to find __compiler
  #endif
  #define __compiler
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
  #elif defined(WOS_WINDOWS)
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

#ifndef WITCH_LIBCPP
  #if defined(__has_include)
    #if __has_include(<iostream>)
      #define WITCH_LIBCPP 1
    #else
      #define WITCH_LIBCPP 0
    #endif
  #else
    #error define WITCH_LIBCPP
  #endif
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

#define EMPTY
#define NUL (1 - 1)
#ifndef __ca__
  #define __ca__ ,
#endif

#ifndef WITCH_PLATFORM_linux_kernel_module
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
#else
  #error failed to find platform
#endif

#if WITCH_LIBC
  #include <stdint.h>
  #define sintptr_t intptr_t
  #define sint8_t int8_t
  #define sint16_t int16_t
  #define sint32_t int32_t
  #define sint64_t int64_t
  #define auint32_t uint32_t
  #define asint32_t sint32_t
#else
  #if SYSTEM_BIT == 64
    typedef signed long sintptr_t;
    typedef signed char sint8_t;
    typedef signed short sint16_t;
    typedef signed int sint32_t;
    typedef sintptr_t sint64_t;
    typedef sint32_t asint32_t;
    #ifndef WITCH_unsigned_types_will_be_defined
      typedef unsigned long uintptr_t;
      typedef unsigned char uint8_t;
      typedef unsigned short uint16_t;
      typedef unsigned int uint32_t;
      typedef uintptr_t uint64_t;
      typedef uint32_t auint32_t;
    #endif
  #elif SYSTEM_BIT == 32
    typedef signed long sintptr_t;
    typedef signed char sint8_t;
    typedef signed short sint16_t;
    typedef sintptr_t sint32_t;
    typedef signed long long sint64_t;
    typedef sint32_t asint32_t;
    #ifndef WITCH_unsigned_types_will_be_defined
      typedef unsigned long uintptr_t;
      typedef unsigned char uint8_t;
      typedef unsigned short uint16_t;
      typedef uintptr_t uint32_t;
      typedef unsigned long long uint64_t;
      typedef uint32_t auint32_t;
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

#define CLAMP(val, lo, hi) \
  ((val) <= (lo) ? (lo) : (val) > (hi) ? (hi) : (val))
#define ABS(num_m) \
  ((num_m) < 0 ? -(num_m) : (num_m))
#define FLOOR \
  (f_t)(sintptr_t)
#define FMOD(num_m, mod_m) \
  ((num_m) - (FLOOR((num_m) / (mod_m)) * (mod_m)))
#define SIGN(_m) \
  (sintptr_t)(-(_m < 0) | (_m > 0))
#define RSIGN(_m) \
  ((_m) * SIGN(_m))

uint32_t LOG32(uintptr_t num, uint8_t base){
  uint32_t log = 0;
  while(num){
    num /= base;
    log++;
  }
  return log;
}
#define LOG32(num_m, base_m) \
  LOG32((uintptr_t)(num_m), (uint8_t)(base_m))
uint32_t LOG64(uint64_t num, uint8_t base){
  uint32_t log = 0;
  while(num){
    num /= base;
    log++;
  }
  return log;
}
#define LOG64(num_m, base_m) \
  LOG64((uint64_t)(num_m), (uint8_t)(base_m))
#define LOG CONCAT(LOG, SYSTEM_BIT)

#ifndef OFFSETLESS
  #define OFFSETLESS(ptr_m, t_m, d_m) \
    ((t_m *)((uint8_t *)(ptr_m) - offsetof(t_m, d_m)))
#endif

#ifndef __unreachable
  #if defined(__compiler_clang) || defined(__compiler_gcc)
    #define __unreachable() __builtin_unreachable()
  #elif defined(__compiler_msvc)
    #define __unreachable() __assume(0)
  #else
    #error ?
  #endif
#endif

#if defined(WL_CPP)
  #define WITCH_c(_m) _m
#else
  #define WITCH_c(_m) (_m)
#endif

#if defined(WL_CPP)
  #ifndef __is_type_same
    template <typename, typename>
    inline constexpr bool __is_type_same = false;
    template <typename _Ty>
    inline constexpr bool __is_type_same<_Ty, _Ty> = true;
    #define __is_type_same __is_type_same
  #endif
  #ifndef __return_type_of
    #define __return_type_of __return_type_of
    #include <functional>
    template<typename Callable>
    using __return_type_of = typename decltype(std::function{ std::declval<Callable>() })::result_type;
  #endif
  #ifndef __compiletime_str
    #define __compiletime_str __compiletime_str
    template<std::size_t count>
    struct __compiletime_str
    {
      char buffer[count + 1]{};
      int length = count;

      constexpr __compiletime_str(char const* string)
      {
        for (std::size_t i = 0; i < count; ++i) {
          buffer[i] = string[i];
        }
      }
      constexpr operator char const* () const { return buffer; }
    };
    template<std::size_t count>
    __compiletime_str(char const (&)[count])->__compiletime_str<count - 1>;
  #endif
  #ifndef __conditional_value
    #define __conditional_value __conditional_value
    template <bool _Test, uintptr_t _Ty1, uintptr_t _Ty2>
    struct __conditional_value {
      static constexpr auto value = _Ty1;
    };
    template <uintptr_t _Ty1, uintptr_t _Ty2>
    struct __conditional_value<false, _Ty1, _Ty2> {
      static constexpr auto value = _Ty2;
    };
    template <bool _Test, uintptr_t _Ty1, uintptr_t _Ty2>
    struct __conditional_value_t {
      static constexpr auto value = __conditional_value<_Test, _Ty1, _Ty2>::value;
    };
  #endif
  #ifndef __ofof
    #define __ofof __ofof
    #pragma pack(push, 1)
      template <typename Member, std::size_t O>
      struct __Pad_t {
        char pad[O];
        Member m;
      };
    #pragma pack(pop)

    template<typename Member>
    struct __Pad_t<Member, 0> {
      Member m;
    };

    template <typename Base, typename Member, std::size_t O>
    struct __MakeUnion_t {
      union U {
        char c;
        Base base;
        __Pad_t<Member, O> pad;
        constexpr U() noexcept : c{} {};
      };
      constexpr static U u{};
    };

    template <typename Member, typename Base, typename Orig>
    struct __ofof_impl {
      template<std::size_t off, auto union_part = &__MakeUnion_t<Base, Member, off>::u>
      static constexpr std::ptrdiff_t offset2(Member Orig::* member) {
        if constexpr (off > sizeof(Base)) {
          throw 1;
        }
        else {
          const auto diff1 = &((static_cast<const Orig*>(&union_part->base))->*member);
          const auto diff2 = &union_part->pad.m;
          if (diff1 > diff2) {
            constexpr auto MIN = sizeof(Member) < alignof(Orig) ? sizeof(Member) : alignof(Orig);
            return offset2<off + MIN>(member);
          }
          else {
            return off;
          }
        }
      }
    };

    template<class Member, class Base>
    std::tuple<Member, Base> __get_types(Member Base::*);

    template <class TheBase = void, class TT>
    inline constexpr std::ptrdiff_t __ofof(TT member) {
      using T = decltype(__get_types(std::declval<TT>()));
      using Member = std::tuple_element_t<0, T>;
      using Orig = std::tuple_element_t<1, T>;
      using Base = std::conditional_t<std::is_void_v<TheBase>, Orig, TheBase>;
      return __ofof_impl<Member, Base, Orig>::template offset2<0>(member);
    }

    template <auto member, class TheBase = void>
    inline constexpr std::ptrdiff_t __ofof() {
      return __ofof<TheBase>(member);
    }
  #endif
  #ifndef __dme
    template <__compiletime_str StringName = "", typename type = long double>
    struct __dme_t{
      const char *sn = StringName;
      // data type
      using dt = type;
      /* data struct size */
      uint32_t m_DSS = __conditional_value<
          std::is_same<
            type, 
            long double
          >::value, 
          0, 
            __conditional_value<
              std::is_empty<dt>::value,
              0,
              sizeof(dt)
            >::value
        >::value;
    };
    #define __dme(p0, p1) \
      using CONCAT(p0,_t) = __dme_t<STR(p0), __return_type_of<decltype([] { \
        struct{p1} v; \
        return v; \
      })>>; \
      CONCAT(p0,_t) p0
    template <typename inherited_t>
    struct __dme_inherit{
      static constexpr uint32_t GetMemberAmount(){
        return sizeof(inherited_t) / sizeof(__dme_t<>);
      }

      /* number to address */
      __dme_t<> *NA(uintptr_t CI){
        return (__dme_t<> *)((uint8_t *)this + CI * sizeof(__dme_t<>));
      }

      /* address to number */
      static constexpr uintptr_t AN(auto inherited_t:: *C){
        return __ofof(C) / sizeof(__dme_t<>);
      }

      /* is number invalid */
      bool ICI(uintptr_t CI){
        return CI * sizeof(__dme_t<>) > sizeof(inherited_t);
      }
    };
  #endif
#endif

/* src, dst, size */
#ifndef __MemoryCopy
  #if defined(__compiler_clang) || defined(__compiler_gcc)
    #define __MemoryCopy(src, dst, size) __builtin_memcpy(dst, src, size)
  #elif defined(__compiler_msvc)
    #define __MemoryCopy(src, dst, size) memcpy(dst, src, size)
  #else
    #error ?
  #endif
#endif
