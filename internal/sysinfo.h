#pragma once

#ifndef PAGE_SIZE
  #define PAGE_SIZE 4096
#endif

#ifndef __platform_nothread
  /* defining THREADS in compile time can help optimize more */
  /* but it also removes portability of executable */
  #if defined(_WITCH_num_online_cpus)
    #define WITCH_num_online_cpus() _WITCH_num_online_cpus
  #else
    #if defined(__platform_unix) && defined(__platform_libc)
      #include <unistd.h>
      #include <errno.h>

      static sintptr_t WITCH_num_online_cpus(void){
        sint32_t ret = sysconf(_SC_NPROCESSORS_ONLN);
        if(ret < 0){
          return errno;
        }
        return ret;
      }
    #elif defined(__platform_windows)
      #include _WITCH_PATH(include/windows/windows.h)
      static sintptr_t WITCH_num_online_cpus(void){
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
      }
    #elif defined(__platform_linux_kernel_module)
      static sintptr_t WITCH_num_online_cpus(void){
        return num_online_cpus();
      }
    #elif defined(__platform_unix_linux)
      #include _WITCH_PATH(include/cpu_set.h)
      #include _WITCH_PATH(include/syscall.h)

      static sintptr_t WITCH_num_online_cpus(void){
        __WITCH_cpu_set_t cpu_set;
        sintptr_t err = syscall3(
          __NR_sched_getaffinity,
          0,
          sizeof(cpu_set),
          (uintptr_t)&cpu_set
        );
        if(err < 0){
          return err;
        }

        return __WITCH_cpuset_cpucount(err, &cpu_set);
      }
    #else
      #error ?
    #endif
  #endif
#endif
