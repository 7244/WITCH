#pragma once

#if defined(__platform_libc)
  #include <signal.h>

  typedef siginfo_t WITCH_signal_siginfo_t;
#else
  /* TODO last time i tried this backend: it didnt work well */
  /* i guess i need to learn signals raw-system-call-wise */
  
  typedef union WITCH_signal_sigval{
    int sival_int;
    void *sival_ptr;
  }WITCH_signal_sigval_t;

  #if defined(__x86_64__) || defined(__i386__)
    #define __ARCH_SI_BAND_T long
  #else
    #error check other archs
  #endif

  #define __ADDR_BND_PKEY_PAD  \
    (__alignof__(void *) < sizeof(short) ? sizeof(short) : __alignof__(void *))
  union _WITCH_signal_sifields{
    struct{
      int _pid;
      unsigned int _uid;
    }_kill;
    struct{
      int _tid;
      int _overrun;
      WITCH_signal_sigval_t _sigval;
      int _sys_private;
    }_timer;
    struct{
      int _pid;
      unsigned int _uid;
      WITCH_signal_sigval_t _sigval;
    }_rt;
    struct{
      int _pid;
      unsigned int _uid;
      int _status;
      uint64_t _utime;
      uint64_t _stime;
    }_sigchld;

    struct{
      void *_addr;
      union{
        int _trapno;
        short _addr_lsb;
        struct{
          char _dummy_bnd[__ADDR_BND_PKEY_PAD];
          void *_lower;
          void *_upper;
        }_addr_bnd;
        struct{
          char _dummy_pkey[__ADDR_BND_PKEY_PAD];
          uint32_t _pkey;
        }_addr_pkey;
        struct{
          unsigned long _data;
          uint32_t _type;
          uint32_t _flags;
        }_perf;
      };
    }_sigfault;

    struct{
      __ARCH_SI_BAND_T _band;
      int _fd;
    }_sigpoll;

    struct{
      void *_call_addr;
      int _syscall;
      unsigned int _arch;
    }_sigsys;
  };
  #undef __ADDR_BND_PKEY_PAD

  #if defined(__x86_64__) || defined(__i386__)
    #define _WITCH_PR_SIGINFO \
    struct { \
      int si_signo; \
      int si_errno; \
      int si_code; \
      union _WITCH_signal_sifields _sifields; \
    }
  #else
    #error check __ARCH_HAS_SWAPPED_SIGINFO for other systems
  #endif

  #define _WITCH_PR_SI_MAX_SIZE 128

  typedef struct{
    union{
      _WITCH_PR_SIGINFO;
      uint32_t _si_pad[_WITCH_PR_SI_MAX_SIZE / sizeof(uint32_t)];
    };
  }WITCH_signal_siginfo_t;

  #include _WITCH_PATH(include/syscall.h)
  #include <asm/signal.h>
  typedef void (*__sighandler_t)(int);
  typedef __sighandler_t sighandler_t;

  #ifdef __i386__
    struct WITCH_sigaction {
	    union {
	      __sighandler_t _sa_handler;
	      void (*_sa_sigaction)(int, WITCH_signal_siginfo_t *, void *);
	    } _u;
	    unsigned long sa_mask;
	    unsigned long sa_flags;
	    void (*sa_restorer)(void);
    };
  #else
    typedef struct{
	    __sighandler_t sa_handler;
	    unsigned long sa_flags;
	    __sigrestore_t sa_restorer;
	    unsigned long sa_mask;
    }WITCH_signal_sigaction_t;
  #endif

  static sighandler_t signal(sint32_t signum, sighandler_t handler){
    WITCH_signal_sigaction_t src;
    src.sa_handler = SIG_DFL;
    src.sa_mask = 0;
    src.sa_flags = 0;
    src.sa_restorer = 0;
    WITCH_signal_sigaction_t dst;
    dst.sa_handler = handler;
    dst.sa_mask = 0;
    dst.sa_flags = 0;
    dst.sa_restorer = 0;
    syscall4(__NR_rt_sigaction, signum, (uintptr_t)&dst, (uintptr_t)&src, 8);
    return 0;
  }
#endif
