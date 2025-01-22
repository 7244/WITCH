#if !defined(__platform_libc)
  #include <WITCH/include/syscall.h>

  #include <linux/mman.h>
#endif

#if !defined(__platform_libc)
  #define __generic_malloc_mmap_is_same
#endif

#if !defined(__generic_mmap)
  #if defined(__generic_mremap) || defined(__generic_munmap)
    #error you need to define all of your functions.
  #endif

  #define __generic_mmap __generic_mmap
  void *__generic_mmap(uintptr_t size){
    return (void *)syscall6(
      #if defined(__i386__)
        __NR_mmap2,
      #elif defined(__x86_64__)
        __NR_mmap,
      #else
        #error ?
      #endif
      (uintptr_t)NULL,
      size,
      PROT_READ | PROT_WRITE,
      MAP_ANONYMOUS | MAP_PRIVATE,
      -1,
      0
    );
  }

  #define __generic_mremap __generic_mremap
  void *__generic_mremap(void *ptr, uintptr_t old_size, uintptr_t new_size){
    return syscall5(
      __NR_mremap,
      (uintptr_t)ptr,
      old_size,
      new_size,
      MREMAP_MAYMOVE,
      (uintptr_t)NULL
    );
  }

  #define __generic_munmap __generic_munmap
  #if defined(__generic_alloc_confident)
    void __generic_munmap(void *ptr, uintptr_t size){
      syscall2(
        __NR_munmap,
        (uintptr_t)ptr,
        size
      );
    }
  #else
    sintptr_t __generic_munmap(void *ptr, uintptr_t size){
      return syscall2(
        __NR_munmap,
        (uintptr_t)ptr,
        size
      );
    }
  #endif
#endif

#if !defined(__generic_malloc)
  #if defined(__generic_realloc) || defined(__generic_free)
    #error you need to define all of your functions.
  #endif

  #define __generic_malloc __generic_malloc
  void *__generic_malloc(uintptr_t size){
    size += sizeof(uintptr_t);

    void *ret = __generic_mmap(size);
    if((uintptr_t)ret > (uintptr_t)-0x1000){
      #if defined(__generic_alloc_abort)
        __abort();
      #endif
      return NULL;
    }

    *(uintptr_t *)ret = size;

    return (void *)((uintptr_t *)ret + 1);
  }

  #define __generic_realloc __generic_realloc
  void *__generic_realloc(void *ptr, uintptr_t size){
    if(ptr == NULL){
      return __generic_malloc(size);
    }

    size += sizeof(uintptr_t);

    ptr = (void *)((uintptr_t *)ptr - 1);

    void *ret = __generic_mremap((uintptr_t)ptr, *(uintptr_t *)ptr, size);
    if((uintptr_t)ret > (uintptr_t)-0x1000){
      #if defined(__generic_alloc_abort)
        __abort();
      #endif
      return NULL;
    }

    *(uintptr_t *)ret = size;

    return (void *)((uintptr_t *)ret + 1);
  }

  #define __generic_free __generic_free
  void __generic_free(void *ptr){
    if(ptr == NULL){
      return;
    }

    ptr = (void *)((uintptr_t *)ptr - 1);

    #if defined(__generic_alloc_confident)
      __generic_munmap(ptr, *(uintptr_t *)ptr);
    #else
      sintptr_t err = __generic_munmap(ptr, *(uintptr_t *)ptr);
      if(err != 0){
        __abort();
      }
    #endif
  }
#endif
