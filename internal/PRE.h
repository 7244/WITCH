#pragma once

#if defined(__compiler_clang) || defined(__compiler_gcc) || defined(__compiler_tinyc)
  #define PRE \
    static void __attribute__((constructor))CONCAT(_PRE_main, __COUNTER__)()
#elif defined(__language_cpp)
  #define _PRE_DEEP2(f) \
    static void f(void); \
    struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
    static void f(void)
  #define _PRE_DEEP(f) _PRE_DEEP2(f)
  #define PRE _PRE_DEEP(CONCAT(_PRE_main, __COUNTER__))
#elif defined(__compiler_msvc)
  #pragma section(".CRT$XCU",read)
  #define _PRE_DEEP2(f, p) \
    static void f(void); \
    __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
    __pragma(comment(linker,"/include:" p #f "_")) \
    static void f(void)
  #define _PRE_DEEP(f, p) _PRE_DEEP2(f, p)
  #ifdef _WIN64
    #define PRE _PRE_DEEP(CONCAT(_PRE_main, __COUNTER__), "")
  #else
    #define PRE _PRE_DEEP(CONCAT(_PRE_main, __COUNTER__), "_")
  #endif
#else
  #error ?
#endif
