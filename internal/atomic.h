#if defined(__compiler_gcc) || defined(__compiler_clang)
  /* nothing to do */
#elif defined(__compiler_msvc)
  /* TODO check c++11 */

  #include <atomic>

  #define __atomic_orderconvert__ATOMIC_RELAXED std::memory_order_relaxed
  #define __atomic_orderconvert__ATOMIC_CONSUME std::memory_order_consume
  #define __atomic_orderconvert__ATOMIC_ACQUIRE std::memory_order_acquire
  #define __atomic_orderconvert__ATOMIC_RELEASE std::memory_order_release
  #define __atomic_orderconvert__ATOMIC_ACQ_REL std::memory_order_acq_rel
  #define __atomic_orderconvert__ATOMIC_SEQ_CST std::memory_order_seq_cst

  #ifndef __atomic_load_n
    #define __atomic_load_n(ptr, order) \
      std::atomic_load_explicit(ptr, __atomic_orderconvert##order)
  #endif
#else
  #error ?
#endif
