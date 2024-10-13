#pragma once

#if defined(__platform_libc)
  #include <signal.h>
#else
  /* TODO last time i tried this backend: it didnt work well */
  /* i guess i need to learn signals raw-system-call-wise */

  #include _WITCH_PATH(include/syscall.h)
  #include <asm/signal.h>
  typedef void (*__sighandler_t)(int);
  typedef __sighandler_t sighandler_t;
  sighandler_t signal(sint32_t signum, sighandler_t handler){
    struct sigaction src;
    src.sa_handler = SIG_DFL;
    src.sa_mask = 0;
    src.sa_flags = 0;
    src.sa_restorer = 0;
    struct sigaction dst;
    dst.sa_handler = handler;
    dst.sa_mask = 0;
    dst.sa_flags = 0;
    dst.sa_restorer = 0;
    syscall4(__NR_rt_sigaction, signum, &dst, &src, 8);
    return 0;
  }
#endif
