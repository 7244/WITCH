#if defined(__compiler_gcc) || defined(__compiler_clang)
  /* nothing to do */
#elif defined(__compiler_msvc)
  /* TODO check c11 */

  #include <stdatomic.h>

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

  #define __define_convert__ATOMIC_RELAXED memory_order_relaxed
  #define __define_convert__ATOMIC_CONSUME memory_order_consume
  #define __define_convert__ATOMIC_ACQUIRE memory_order_acquire
  #define __define_convert__ATOMIC_RELEASE memory_order_release
  #define __define_convert__ATOMIC_ACQ_REL memory_order_acq_rel
  #define __define_convert__ATOMIC_SEQ_CST memory_order_seq_cst

  #ifndef __atomic_load_n
    #define __atomic_load_n(ptr, order) \
      atomic_load_explicit(ptr, __define_convert##order)
  #endif

  #undef __define_convert__ATOMIC_RELAXED
  #undef __define_convert__ATOMIC_CONSUME
  #undef __define_convert__ATOMIC_ACQUIRE
  #undef __define_convert__ATOMIC_RELEASE
  #undef __define_convert__ATOMIC_ACQ_REL
  #undef __define_convert__ATOMIC_SEQ_CST
#else
  #error ?
#endif
