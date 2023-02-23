#pragma once

/* all system calls are returning negative as error right? */
/* so it would make sense if we define errno constants with this include */

/* lets only define __ASSEMBLER__ when there is no libc */
/* otherwise some retard standard libs crying when they cant find errno variable */
#if !WITCH_LIBC
  #define __ASSEMBLER__
#endif
#include <errno.h>

#if defined(WOS_UNIX_LINUX)
  #if defined(__x86_64__)
    #ifndef _syscall_h
      #define _syscall_h <asm/unistd_64.h>
    #endif
    #define _syscall_use_linux_x86_64
  #elif defined(__i386__)
    #ifndef _syscall_h
      #define _syscall_h <asm/unistd_32.h>
    #endif
    #define _syscall_use_linux_i386
  #elif defined(__ARM_EABI__)
    #if SYSTEM_BYTE != 4
      /* is this possible or not? */
      #error ?
    #endif
    #ifndef _syscall_h
      #define _syscall_h <asm/unistd.h>
    #endif
    #define _syscall_use_linux_ARM_EABI
  #else
    #error ?
  #endif
#elif defined(WOS_UNIX_BSD)
  #ifndef _syscall_h
    #define _syscall_h <sys/syscall.h>
  #endif
  #if defined(__x86_64__)
    #define _syscall_use_bsd_x86_64
  #elif defined(__i386__)
    #define _syscall_use_bsd_i386
  #else
    #error ?
  #endif
#else
  #error ?
#endif

/* this is useful for cross compile to different kernel */
#include _syscall_h

#if defined(_syscall_use_linux_x86_64)
  static inline sintptr_t syscall0(uintptr_t nr){
    register sintptr_t _r __asm__("rax");
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall1(uintptr_t nr, uintptr_t p0){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall2(uintptr_t nr, uintptr_t p0, uintptr_t p1){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall3(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall4(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2), "r" (_p3)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall5(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    register uintptr_t _p4 __asm__("r8") = p4;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2), "r" (_p3), "r" (_p4)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall6(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    register uintptr_t _p4 __asm__("r8") = p4;
    register uintptr_t _p5 __asm__("r9") = p5;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2), "r" (_p3), "r" (_p4), "r" (_p5)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
#elif defined(_syscall_use_linux_i386)
  static inline sintptr_t syscall0(uintptr_t nr){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "int $0x80"
      : "=r" (_r)
      : "0" (nr)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall1(uintptr_t nr, uintptr_t p0){
    register sintptr_t _r __asm__("eax");
    register uintptr_t _p0 __asm__("ebx") = p0;
    __asm__ __volatile__(
      "int $0x80"
      : "=r" (_r)
      : "0" (nr), "r" (_p0)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall2(uintptr_t nr, uintptr_t p0, uintptr_t p1){
    register sintptr_t _r __asm__("eax");
    register uintptr_t _p0 __asm__("ebx") = p0;
    register uintptr_t _p1 __asm__("ecx") = p1;
    __asm__ __volatile__(
      "int $0x80"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall3(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2){
    register sintptr_t _r __asm__("eax");
    register uintptr_t _p0 __asm__("ebx") = p0;
    register uintptr_t _p1 __asm__("ecx") = p1;
    register uintptr_t _p2 __asm__("edx") = p2;
    __asm__ __volatile__(
      "int $0x80"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall4(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register sintptr_t _r __asm__("eax");
    register uintptr_t _p0 __asm__("ebx") = p0;
    register uintptr_t _p1 __asm__("ecx") = p1;
    register uintptr_t _p2 __asm__("edx") = p2;
    register uintptr_t _p3 __asm__("esi") = p3;
    __asm__ __volatile__(
      "int $0x80"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2), "r" (_p3)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall5(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4){
    register sintptr_t _r __asm__("eax");
    register uintptr_t _p0 __asm__("ebx") = p0;
    register uintptr_t _p1 __asm__("ecx") = p1;
    register uintptr_t _p2 __asm__("edx") = p2;
    register uintptr_t _p3 __asm__("esi") = p3;
    register uintptr_t _p4 __asm__("edi") = p4;
    __asm__ __volatile__(
      "int $0x80"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2), "r" (_p3), "r" (_p4)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall6(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5){
    register sintptr_t _r __asm__("eax");
    register uintptr_t _p0 __asm__("ebx") = p0;
    register uintptr_t _p1 __asm__("ecx") = p1;
    register uintptr_t _p2 __asm__("edx") = p2;
    register uintptr_t _p3 __asm__("esi") = p3;
    register uintptr_t _p4 __asm__("edi") = p4;
    register uintptr_t _p5 __asm__("ebp") = p5;
    __asm__ __volatile__(
      "int $0x80"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2), "r" (_p3), "r" (_p4), "r" (_p5)
      : "memory"
    );
    return _r;
  }
#elif defined(_syscall_use_linux_ARM_EABI)
  static inline sintptr_t syscall0(uintptr_t nr){
    register uintptr_t _nr __asm__("r7") = nr;
    register sintptr_t _r __asm__("r0");
    __asm__ __volatile__(
      "swi 0x0"
      : "=r" (_r)
      : "r" (_nr)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall1(uintptr_t nr, uintptr_t p0){
    register uintptr_t _nr __asm__("r7") = nr;
    register sintptr_t _r __asm__("r0"); /* also p0 */
    __asm__ __volatile__(
      "swi 0x0"
      : "=r" (_r)
      : "0" (p0), "r" (_nr)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall2(uintptr_t nr, uintptr_t p0, uintptr_t p1){
    register uintptr_t _nr __asm__("r7") = nr;
    register sintptr_t _r __asm__("r0"); /* also p0 */
    register uintptr_t _p1 __asm__("r1") = p1;
    __asm__ __volatile__(
      "swi 0x0"
      : "=r" (_r)
      : "0" (p0), "r" (_nr), "r" (_p1)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall3(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2){
    register uintptr_t _nr __asm__("r7") = nr;
    register sintptr_t _r __asm__("r0"); /* also p0 */
    register uintptr_t _p1 __asm__("r1") = p1;
    register uintptr_t _p2 __asm__("r2") = p2;
    __asm__ __volatile__(
      "swi 0x0"
      : "=r" (_r)
      : "0" (p0), "r" (_nr), "r" (_p1), "r" (_p2)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall4(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register uintptr_t _nr __asm__("r7") = nr;
    register sintptr_t _r __asm__("r0"); /* also p0 */
    register uintptr_t _p1 __asm__("r1") = p1;
    register uintptr_t _p2 __asm__("r2") = p2;
    register uintptr_t _p3 __asm__("r3") = p3;
    __asm__ __volatile__(
      "swi 0x0"
      : "=r" (_r)
      : "0" (p0), "r" (_nr), "r" (_p1), "r" (_p2), "r" (_p3)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall5(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4){
    register uintptr_t _nr __asm__("r7") = nr;
    register sintptr_t _r __asm__("r0"); /* also p0 */
    register uintptr_t _p1 __asm__("r1") = p1;
    register uintptr_t _p2 __asm__("r2") = p2;
    register uintptr_t _p3 __asm__("r3") = p3;
    register uintptr_t _p4 __asm__("r4") = p4;
    __asm__ __volatile__(
      "swi 0x0"
      : "=r" (_r)
      : "0" (p0), "r" (_nr), "r" (_p1), "r" (_p2), "r" (_p3), "r" (_p4)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall6(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5){
    register uintptr_t _nr __asm__("r7") = nr;
    register sintptr_t _r __asm__("r0"); /* also p0 */
    register uintptr_t _p1 __asm__("r1") = p1;
    register uintptr_t _p2 __asm__("r2") = p2;
    register uintptr_t _p3 __asm__("r3") = p3;
    register uintptr_t _p4 __asm__("r4") = p4;
    register uintptr_t _p5 __asm__("r5") = p5;
    __asm__ __volatile__(
      "swi 0x0"
      : "=r" (_r)
      : "0" (p0), "r" (_nr), "r" (_p1), "r" (_p2), "r" (_p3), "r" (_p4), "r" (_p5)
      : "memory"
    );
    return _r;
  }
#elif defined(_syscall_use_bsd_x86_64)
  static inline sintptr_t syscall0_noerr(uintptr_t nr){
    register sintptr_t _r __asm__("rax");
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr)
      : "rcx", "r8", "r9", "r10", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall1_noerr(uintptr_t nr, uintptr_t p0){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0)
      : "rcx", "r8", "r9", "r10", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall2_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1)
      : "rcx", "r8", "r9", "r10", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall3_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r8", "r9", "r10", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall3_carry(bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r), "=@ccc" (*carry)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r8", "r9", "r10", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall4_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r), "+r" (_p3)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r8", "r9", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall4_carry(bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r), "+r" (_p3), "=@ccc" (*carry)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r8", "r9", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall5_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    register uintptr_t _p4 __asm__("r8") = p4;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r), "+r" (_p3), "+r" (_p4)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r9", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall5_carry(bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    register uintptr_t _p4 __asm__("r8") = p4;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r), "+r" (_p3), "+r" (_p4), "=@ccc" (*carry)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r9", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall6_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    register uintptr_t _p4 __asm__("r8") = p4;
    register uintptr_t _p5 __asm__("r9") = p5;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r), "+r" (_p3), "+r" (_p4), "+r" (_p5)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall6_carry(bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    register uintptr_t _p4 __asm__("r8") = p4;
    register uintptr_t _p5 __asm__("r9") = p5;
    __asm__ __volatile__(
      "syscall"
      : "=r" (_r), "+r" (_p3), "+r" (_p4), "+r" (_p5), "=@ccc" (*carry)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall7_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5, uintptr_t p6){
    register sintptr_t _r __asm__("rax");
    register uintptr_t _p0 __asm__("rdi") = p0;
    register uintptr_t _p1 __asm__("rsi") = p1;
    register uintptr_t _p2 __asm__("rdx") = p2;
    register uintptr_t _p3 __asm__("r10") = p3;
    register uintptr_t _p4 __asm__("r8") = p4;
    register uintptr_t _p5 __asm__("r9") = p5;
    __asm__ __volatile__(
      "push %8\n"
      "push %%rax\n"
      "syscall\n"
      "add $16, %%rsp\n"
      : "=r" (_r), "+r" (_p3), "+r" (_p4), "+r" (_p5)
      : "0" (nr), "r" (_p0), "r" (_p1), "r" (_p2), "g" (p6)
      : "rcx", "r11", "memory"
    );
    return _r;
  }
#elif defined(_syscall_use_bsd_i386)
  static inline sintptr_t syscall0_noerr(uintptr_t nr){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %%eax\n"
      "int $0x80\n"
      "add $4, %%esp\n"
      : "=r" (_r)
      : "0" (nr)
    );
    return _r;
  }
  static inline sintptr_t syscall1_noerr(uintptr_t nr, uintptr_t p0){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %2\n"
      "push %%eax\n"
      "int $0x80\n"
      "add $8, %%esp\n"
      : "=r" (_r)
      : "0" (nr), "g" (p0)
    );
    return _r;
  }
  static inline sintptr_t syscall2_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %3\n"
      "push %2\n"
      "push %%eax\n"
      "int $0x80\n"
      "add $12, %%esp\n"
      : "=r" (_r)
      : "0" (nr), "g" (p0), "g" (p1)
    );
    return _r;
  }
  static inline sintptr_t syscall3_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %4\n"
      "push %3\n"
      "push %2\n"
      "push %%eax\n"
      "int $0x80\n"
      "add $16, %%esp\n"
      : "=r" (_r)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2)
    );
    return _r;
  }
  static inline sintptr_t syscall3_carry(bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %5\n"
      "push %4\n"
      "push %3\n"
      "push %%eax\n"
      "int $0x80\n"
      "setc %1\n"
      "add $16, %%esp\n"
      : "=r" (_r), "=g" (*carry)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2)
      : "memory"
    );
    return _r;
  }
  static inline sintptr_t syscall4_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %5\n"
      "push %4\n"
      "push %3\n"
      "push %2\n"
      "push %%eax\n"
      "int $0x80\n"
      "add $20, %%esp\n"
      : "=r" (_r)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2), "g" (p3)
    );
    return _r;
  }
  static inline sintptr_t syscall4_carry(bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %6\n"
      "push %5\n"
      "push %4\n"
      "push %3\n"
      "push %%eax\n"
      "int $0x80\n"
      "setc %1\n"
      "add $20, %%esp\n"
      : "=r" (_r), "=g" (*carry)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2), "g" (p3)
    );
    return _r;
  }
  static inline sintptr_t syscall4_extra_carry(sintptr_t *extra, bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %7\n"
      "push %6\n"
      "push %5\n"
      "push %4\n"
      "push %%eax\n"
      "int $0x80\n"
      "setc %1\n"
      "add $20, %%esp\n"
      : "=r" (_r), "=g" (*carry), "=d" (*extra)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2), "g" (p3)
    );
    return _r;
  }
  static inline sintptr_t syscall5_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %6\n"
      "push %5\n"
      "push %4\n"
      "push %3\n"
      "push %2\n"
      "push %%eax\n"
      "int $0x80\n"
      "add $24, %%esp\n"
      : "=r" (_r)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2), "g" (p3), "g" (p4)
    );
    return _r;
  }
  static inline sintptr_t syscall5_carry(bool *carry, uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %7\n"
      "push %6\n"
      "push %5\n"
      "push %4\n"
      "push %3\n"
      "push %%eax\n"
      "int $0x80\n"
      "setc %1\n"
      "add $24, %%esp\n"
      : "=r" (_r), "=g" (*carry)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2), "g" (p3), "g" (p4)
    );
    return _r;
  }
  static inline sintptr_t syscall6_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %7\n"
      "push %6\n"
      "push %5\n"
      "push %4\n"
      "push %3\n"
      "push %2\n"
      "push %%eax\n"
      "int $0x80\n"
      "add $28, %%esp\n"
      : "=r" (_r)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2), "g" (p3), "g" (p4), "g" (p5)
    );
    return _r;
  }
  static inline sintptr_t syscall7_noerr(uintptr_t nr, uintptr_t p0, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5, uintptr_t p6){
    register sintptr_t _r __asm__("eax");
    __asm__ __volatile__(
      "push %8\n"
      "push %7\n"
      "push %6\n"
      "push %5\n"
      "push %4\n"
      "push %3\n"
      "push %2\n"
      "push %%eax\n"
      "int $0x80\n"
      "add $32, %%esp\n"
      : "=r" (_r)
      : "0" (nr), "g" (p0), "g" (p1), "g" (p2), "g" (p3), "g" (p4), "g" (p5), "g" (p6)
    );
    return _r;
  }
#else
  #error ?
#endif
