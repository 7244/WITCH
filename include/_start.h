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
      : : "i" (main)
    #elif defined(__x86_64__)
      "mov 0(%%rsp), %%rdi\n"
      "lea 8(%%rsp), %%rsi\n"
      "call %P0\n"
      : : "i" (main)
    #else
      #error ?
    #endif
  );

  #if defined(__compiler_clang) && __clang_major__ >= 19 && __clang_major__ <= 19
    /* ~poolaki~ */
  #elif defined(__compiler_gcc)
    /* gcc generates undefined instruction without unreachable */
    __unreachable();
  #else
    #error ?
  #endif
}
