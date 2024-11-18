#if defined(__compiler_gcc) || defined(__compiler_clang)
  /* nothing to do */
#elif defined(__compiler_msvc)
  #include <intrin.h>

  #ifndef __ATOMIC_RELAXED
    #define __ATOMIC_RELAXED 0
  #endif
  #ifndef __ATOMIC_CONSUME
    #define __ATOMIC_CONSUME 1
  #endif
  #ifndef __ATOMIC_ACQUIRE
    #define __ATOMIC_ACQUIRE 2
  #endif
  #ifndef __ATOMIC_RELEASE
    #define __ATOMIC_RELEASE 3
  #endif
  #ifndef __ATOMIC_ACQ_REL
    #define __ATOMIC_ACQ_REL 4
  #endif
  #ifndef __ATOMIC_SEQ_CST
    #define __ATOMIC_SEQ_CST 5
  #endif

  __cta(__ATOMIC_RELAXED != __ATOMIC_CONSUME);
  __cta(__ATOMIC_RELAXED != __ATOMIC_ACQUIRE);
  __cta(__ATOMIC_RELAXED != __ATOMIC_RELEASE);
  __cta(__ATOMIC_RELAXED != __ATOMIC_ACQ_REL);
  __cta(__ATOMIC_RELAXED != __ATOMIC_SEQ_CST);

  __cta(__ATOMIC_CONSUME != __ATOMIC_ACQUIRE);
  __cta(__ATOMIC_CONSUME != __ATOMIC_RELEASE);
  __cta(__ATOMIC_CONSUME != __ATOMIC_ACQ_REL);
  __cta(__ATOMIC_CONSUME != __ATOMIC_SEQ_CST);

  __cta(__ATOMIC_ACQUIRE != __ATOMIC_RELEASE);
  __cta(__ATOMIC_ACQUIRE != __ATOMIC_ACQ_REL);
  __cta(__ATOMIC_ACQUIRE != __ATOMIC_SEQ_CST);

  __cta(__ATOMIC_RELEASE != __ATOMIC_ACQ_REL);
  __cta(__ATOMIC_RELEASE != __ATOMIC_SEQ_CST);

  __cta(__ATOMIC_ACQ_REL != __ATOMIC_SEQ_CST);

  #if !defined(WL_CPP)
    #error too pain
  #endif

  #define __define_in(name, size, value) \
    else if constexpr(sizeof(*ptr) == size){ \
      if(order == __ATOMIC_ACQUIRE){ \
        return name ## _acq(ptr, value); \
      } \
      else if(order == __ATOMIC_RELEASE){ \
        return name ## _rel(ptr, value); \
      } \
      else{ \
        return name(ptr, value); \
      } \
    }

  #ifndef __atomic_load_n
    auto __atomic_load_n(auto *ptr, uintptr_t order){
      if constexpr(0){}
      __define_in(_InterlockedXor8, 1, 0)
      __define_in(_InterlockedXor16, 2, 0)
      __define_in(_InterlockedXor, 4, 0)
      __define_in(_InterlockedXor64, 8, 0)
      else{
        __cta(false);
      }
    }
  #endif

  #undef __define_in
#else
  #error ?
#endif
