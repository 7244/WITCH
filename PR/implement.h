#include _WITCH_PATH(include/syscall.h)

#ifndef _PR_set_abort_print
  #if defined(__platform_libc)
    #define _PR_set_abort_print 1
  #else
    #define _PR_set_abort_print 0
  #endif
#endif

#if _PR_set_abort_print
  #if defined(WOS_UNIX) && defined(__platform_libc)
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/wait.h>
    #include <unistd.h>
    #include <sys/prctl.h>

    static void _PR_DumpTrace(){
      char pid_buf[30];
      sprintf(pid_buf, "%d", getpid());
      char name_buf[512];
      name_buf[readlink("/proc/self/exe", name_buf, 511)] = 0;
      prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
      int child_pid = fork();
      if(!child_pid) {
        dup2(2, 1);
        execl("/usr/bin/gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "thread apply all bt", "-ex", "bt", name_buf, pid_buf, NULL);
        abort();
      }
      else {
        waitpid(child_pid,NULL,0);
      }
    }
  #elif defined(__language_cpp) && __language_cpp >= 202302L
    #include <iostream>
    #include <stacktrace>

    static void _PR_DumpTrace(){
      std::stacktrace st;
      std::cout << st.current();
    }
  #elif defined(WOS_WINDOWS)
    #include <stdio.h>
    #include <dbghelp.h>

    #if defined(__compiler_msvc)
      #pragma comment(lib, "Dbghelp.lib")
    #endif

    static void _PR_DumpTrace() {
      uint16_t i;
      uint16_t frames;
      void *stack[0xff];
      SYMBOL_INFO *symbol;
      HANDLE process;

      SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES);

      process = GetCurrentProcess();

      if (!SymInitialize(process, NULL, TRUE)) {
        int err = GetLastError();
        printf("[_PR_DumpTrace] SymInitialize failed %d", err);
      }

      frames = CaptureStackBackTrace(0, 0xff, stack, NULL);
      symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 1024 * sizeof(uint8_t), 1);
      symbol->MaxNameLen = 1023;
      symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

      for (i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        DWORD Displacement;
        IMAGEHLP_LINE64 Line;
        Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if (SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &Displacement, &Line)) {
          printf("%i: %s:%u\n", frames - i - 1, symbol->Name, Line.LineNumber);
        }
        else {
          printf("%i: %s:0x%x\n", frames - i - 1, symbol->Name, symbol->Address);
        }
      }

      free(symbol);
    }
  #else
    #error ?
  #endif

  #if defined(WOS_WINDOWS)
    #include <signal.h>

    static void _PR_SignalCatcher(int signal){
      if(signal == SIGINT){
        return;
      }
      /* TODO make some generic function instead of printf */
      printf("[PR] _PR_SignalCatcher(%d)\n", signal);
      _PR_DumpTrace();
    }
  #endif
#endif

static void PR_exit(uint32_t num){
  #if defined(WOS_UNIX_LINUX)
    syscall1(__NR_exit_group, num);
  #elif defined(WOS_UNIX_BSD)
    syscall1_noerr(SYS_exit, num);
  #elif defined(WOS_WINDOWS)
    exit(num);
  #elif defined(WITCH_PLATFORM_linux_kernel_module)
    while(1){}
  #elif defined(__platform_bpf)
    while(1){}
  #else
    #error ?
  #endif
}

static void PR_abort(void){
  #if defined(WITCH_PLATFORM_linux_kernel_module)
    panic("[PR] PR_abort()");
  #else
    /* TODO make some generic function instead of printf */
    #if defined(__platform_libc)
      printf("[PR] PR_abort() is called\n");
    #endif
    _PR_DumpTrace();
    PR_exit(1);
  #endif
}

#if defined(WOS_UNIX)
  typedef struct{
    sint32_t id;
  }PR_PID_t;

  static sint32_t PR_fork(PR_PID_t *pid){
    sint32_t r = syscall0(__NR_fork);
    if(r < 0){
      return r;
    }
    pid->id = r;
    return 0;
  }

  static bool PR_IsPIDParent(PR_PID_t *pid){
    return pid->id != 0;
  }
  static bool PR_IsPIDChild(PR_PID_t *pid){
    return pid->id == 0;
  }

  static sint32_t PR_exec(const char *PathName, const char **arg, char * const *envp, uint32_t flag){
    return syscall3(
      __NR_execve,
      (uintptr_t)PathName,
      (uintptr_t)arg,
      (uintptr_t)envp);
  }

  typedef siginfo_t PR_WaitPIDRV_t;

  static sint32_t PR_WaitPID(PR_PID_t *pid, PR_WaitPIDRV_t *WaitPIDRV, uint32_t flag){
    return syscall5(
      __NR_waitid,
      P_PID,
      pid->id,
      (uintptr_t)WaitPIDRV,
      flag,
      (uintptr_t)NULL);
  }
#else
  /* implement for more platforms */
#endif

static void _PR_internal_open(){
  #if defined(WOS_WINDOWS)
    #if _PR_set_abort_print
      typedef void (*SignalHandlerPointer)(int);
      SignalHandlerPointer PreviousHandler;
      PreviousHandler = signal(SIGABRT, _PR_SignalCatcher);
    #endif
  #endif
}
static void _PR_internal_close(){}
