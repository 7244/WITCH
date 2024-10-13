#include _WITCH_PATH(include/syscall.h)

#ifndef _PR_set_abort_print
  #if defined(__platform_libc)
    #define _PR_set_abort_print 1
  #else
    #define _PR_set_abort_print 0
  #endif
#endif

#if _PR_set_abort_print
  #if defined(__platform_libc)
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
  #endif
#endif

static void PR_exit(uint32_t num) {
  #if defined(WOS_UNIX_LINUX)
    syscall1(__NR_exit_group, num);
  #elif defined(WOS_UNIX_BSD)
    syscall1_noerr(SYS_exit, num);
  #endif
  __unreachable();
}

#if _PR_set_abort_print
  /* TODO checking libc and using printf and exit in here is kinda stupid */
  #if defined(__platform_libc)
    static void PR_abort(void) {
      printf("[PR] PR_abort() is called\n");
      _PR_DumpTrace();
      exit(1);
      __unreachable();
    }
  #else
    #error ?
  #endif
#else
  void PR_abort(void) {
    PR_exit(1);
  }
#endif

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

static void _PR_internal_open(){}
static void _PR_internal_close(){}
