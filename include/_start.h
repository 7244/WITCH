#ifndef _WITCH__start_func_name
  #define _WITCH__start_func_name main
#endif

__no_name_mangling
__attribute__((noreturn))
__attribute((naked))
void
_start(void *stack){
  __asm__ __volatile__(
    #if defined(__i386__)
      "mov 0(%%esp), %%esi\n"
      "lea 4(%%esp), %%ecx\n"
      "sub $8, %%esp\n"
      "push %%ecx\n"
      "push %%esi\n"
      "call %P0\n"
      : : "i" (_WITCH__start_func_name)
    #elif defined(__x86_64__)
      "mov 0(%%rsp), %%rdi\n"
      "lea 8(%%rsp), %%rsi\n"
      "call %P0\n"
      : : "i" (_WITCH__start_func_name)
    #else
      #error ?
    #endif
  );

  #include _WITCH_PATH(include/end_of_naked.h)
}

#undef _WITCH__start_func_name
