#pragma once

#ifndef PAGE_SIZE
  #define PAGE_SIZE 4096
#endif

/* defining THREADS in compile time can help optimize more */
/* but it also removes portability of executable */
#if defined(_WITCH_num_online_cpus)
  #define WITCH_num_online_cpus() _WITCH_num_online_cpus
#else
  #if defined(WOS_UNIX) && WITCH_LIBC
    #include _WITCH_PATH(PR/PR.h)
    #include <unistd.h>
    static uint32_t WITCH_num_online_cpus(void){
      sint32_t ret = sysconf(_SC_NPROCESSORS_ONLN);
      if(ret < 0){
        PR_abort();
      }
      return ret;
    }
  #elif defined(WOS_WINDOWS)
    #include _WITCH_PATH(include/windows/windows.h)
    static uint32_t WITCH_num_online_cpus(void){
      SYSTEM_INFO sysinfo;
      GetSystemInfo(&sysinfo);
      return sysinfo.dwNumberOfProcessors;
    }
  #elif defined(WITCH_PLATFORM_linux_kernel_module)
    static uint32_t WITCH_num_online_cpus(void){
      return num_online_cpus();
    }
  #else
    #error ?
  #endif
#endif
