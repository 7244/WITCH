#include _WITCH_PATH(generic_alloc.h)

#include _WITCH_PATH(T/T.h)

#if defined(__x86_64__)
  __attribute((naked))
  static sintptr_t _TH_newthread_orphan_entry(void *stack){
    __asm__ __volatile__(
        "pop %%rdi\n"
        "pop %%rax\n"
        "jmp *%%rax\n"
        : : :
    );

    #include _WITCH_PATH(include/end_of_naked.h)
  }
#endif

__attribute((naked))
static sintptr_t _TH_newthread_orphan(void *stack){
  __asm__ __volatile__(
    #if defined(__i386__)
      "mov 4(%%esp), %%ecx\n"
      "mov $0x50f00, %%ebx\n"
      "mov $120, %%eax\n"
      "int $0x80\n"
      "test %%eax, %%eax\n"
      "jz gt\n"
      "ret\n"
      "gt:\n"
      "jmp *(%%esp)\n"
      : : : "eax", "ecx", "ebx", "edx", "memory"
    #elif defined(__x86_64__)
      "mov %%rdi, %%rsi\n"
      "mov $0x50f00, %%edi\n"
      "mov $56, %%eax\n"
      "syscall\n"
      "ret\n"
      : : : "rax", "rcx", "rsi", "r11", "memory"
    #else
      #error ?
    #endif
  );

  #include _WITCH_PATH(include/end_of_naked.h)
}

static void *_TH_newstack_orphan(uintptr_t size){
  #if defined(__i386__)
    void *p = __generic_mmap(size);
    return (void *)((uintptr_t)p + size - sizeof(void *) * 2);
  #elif defined(__x86_64__)
    size -= sizeof(void *);
    void *p = __generic_mmap(size);
    return (void *)((uintptr_t)p + size - sizeof(void *) * 3);
  #else
    #error ?
  #endif
}

static sint32_t TH_newthread_orphan(void(*func)(void*), void *param){
  void *stack = _TH_newstack_orphan(0x8000);

  #if defined(__i386__)
    ((void **)stack)[0] = (void *)func;
    ((void **)stack)[1] = param;
  #elif defined(__x86_64__)
    ((void **)stack)[0] = (void *)_TH_newthread_orphan_entry;
    ((void **)stack)[1] = param;
    ((void **)stack)[2] = (void *)func;
  #else
    #error ?
  #endif

  return _TH_newthread_orphan(stack);
}

static void TH_sleep(uintptr_t ns){
  _T_timespec_t timespec;
  timespec.tv_sec = ns / 1000000000;
  timespec.tv_nsec = ns % 1000000000;
  syscall1(__NR_nanosleep, (uintptr_t)&timespec);
}
