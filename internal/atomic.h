#if defined(__compiler_gcc) || defined(__compiler_clang) || defined(__compiler_tinyc)
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
      [a = ptr]{ \
        std::atomic<std::remove_pointer_t<decltype(a)>> v{}; \
        v.store(*a, __atomic_orderconvert##order); \
        return v.load(); \
      }()
  #endif

  #ifndef __atomic_exchange_n
    #define __atomic_exchange_n(dst_ptr, src, order) \
      [&]{ \
        auto *v = (std::atomic<std::remove_pointer_t<decltype(dst_ptr)>> *)dst_ptr; \
        return v->exchange(src, __atomic_orderconvert##order); \
      }()
  #endif
  
  #ifndef __atomic_store_n
    #define __atomic_store_n __atomic_exchange_n
  #endif
#else
  #error ?
#endif
